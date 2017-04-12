#include "rstar.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "map.h"
#include "node.h"
#include "opencontainer.h"
#include "list.h"
#include <unordered_set>
#include <ctime>
#include <cmath>
#include <algorithm>
#include <chrono>

#include "isearch.h"
#include "astar.h"
#include "searchresult.h"


const double PI = 3.1415926;


Rstar::Rstar()
{

}

Rstar::Rstar(double weight, int BT, int SL, int distance_to_successors,
             int number_of_successors, int local_search_step_limit)
{
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;

    this->distance_to_successors = distance_to_successors;
    this->number_of_successors = number_of_successors;
    this->local_search_step_limit = local_search_step_limit;

    std::srand(std::chrono::duration_cast<std::chrono::milliseconds>
                   (std::chrono::system_clock::now().time_since_epoch()).count());
}

Rstar::~Rstar()
{

}

SearchResult Rstar::startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::system_clock::now();

    generateCirleOfSuccessors();

    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max", false);


    Node start;
    start.i = map.start_i;
    start.j = map.start_j;
    start.g = 0;

    Node goal;
    goal.i = map.goal_i;
    goal.j = map.goal_j;

    open.push(start);

    Node current_node, child_node;

    SearchResult localSearchResult;

    while(!open.empty()) {
        current_node = open.pop();

        if (current_node.AVOID && start != current_node) {
            localSearchResult = findLocalPath(current_node, *current_node.parent, map, logger, options, -1);
            // no step limit if already marked AVOID
        }
        else if (start != current_node) {
            localSearchResult = findLocalPath(current_node, *current_node.parent, map,
                                           logger, options, local_search_step_limit);
        }


        if(localSearchResult.pathfound || start == current_node) {
            if (current_node != start) {
                // Update g from current node with found path
                current_node.g = current_node.parent->g + localSearchResult.pathlength;
            }

            if (current_node == goal) {
                sresult.pathfound = true;
                break;
            }

            auto current_node_iterator = closed.insert(current_node).first;

            for (auto child_coords : generateSuccessors(current_node, map)) {
                child_node = Node(child_coords.first, child_coords.second);

                child_node.parent = &(*current_node_iterator); // FIXME: not sure all this is a must

                // First we put this in g - path length to parent
                // plus distance_to_successors value, as we won't get
                // there faster
                child_node.g = current_node.g + distance_to_successors;

                calculateHeuristic(child_node, map, options);

                open.push(child_node);
            }
        }
        else if(!current_node.AVOID) {
            current_node.AVOID = true;
            // Recount heuristics
            current_node.F += local_search_step_limit;
            open.push(current_node);
        }
    }

    auto end_time = std::chrono::system_clock::now();
    sresult.time = (std::chrono::duration<double>(end_time - start_time)).count();

    if (sresult.pathfound) {
        sresult.hppath = new NodeList();
        sresult.lppath = new NodeList();

        SearchResult localSearchResult;
        while (current_node.parent != nullptr) {
            localSearchResult = findLocalPath(current_node, *current_node.parent,
                                              map, logger, options, -1);
            sresult.JoinLpPaths( localSearchResult );
            sresult.hppath->push_front(current_node);
            current_node = *current_node.parent;
        }

        sresult.hppath->push_front(current_node); // add start node
    }

    return sresult;
}

SearchResult Rstar::findLocalPath(const Node &node, const Node &parent_node, const Map &map,
                          ILogger *logger, const EnvironmentOptions &options, int localSearchStepLimit)
{
    Astar localSearch(hweight, breakingties, localSearchStepLimit);
    localSearch.setAlternativePoints(parent_node, node);
    SearchResult localSearchResult = localSearch.startSearch(logger, map, options);

    return localSearchResult;
}

void Rstar::calculateHeuristic(Node & a, const Map &map, const EnvironmentOptions &options)
{
    a.F = a.g;
    int di = abs(a.i - map.goal_i),
        dj = abs(a.j - map.goal_j);

    // Normalizing heuristics with linecost
    if(options.metrictype == CN_SP_MT_EUCL) a.H = sqrt(di * di + dj * dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_MANH) a.H = (di + dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_DIAG) a.H = std::min(di, dj) * options.diagonalcost +
                                                                        abs(di - dj) * options.linecost;
    else a.H = std::max(di, dj) * options.linecost;

    a.F += hweight * a.H;
}

std::vector<std::pair<int, int> > Rstar::generateSuccessors(const Node &node, const Map &map)
{
    std::vector< std::pair<int, int> > successors;
    std::vector< std::pair<int, int> > predecessors;

    Node parent;
    bool has_parent = false;
    if (node.parent != nullptr) {
        parent = *node.parent;
        has_parent = true;
    }

    std::pair<int, int> successor;

    auto it = successors_circle.begin();
    for(; it < successors_circle.end(); it++) {
        successor = *it;

        successor.first = node.i + successor.first;
        successor.second = node.j + successor.second;

        if(!map.CellOnGrid(successor.first, successor.second)) {
            continue;
        }
        if(map.CellIsObstacle(successor.first, successor.second)) {
            continue;
        }
        if (has_parent) {
            if (parent.i == successor.first && parent.j == successor.second) {
                continue;
            }
        }

        successors.push_back(successor);
    }

    int element_to_erase;
    while(successors.size() > number_of_successors) {
        element_to_erase = rand() % successors.size();

        successors.erase(successors.begin() + element_to_erase);
    }

    if ((node.i - map.goal_i) * (node.i - map.goal_i) +
            (node.j - map.goal_j) * (node.j - map.goal_j) <=
                (int) (distance_to_successors * distance_to_successors)) {
        successors.push_back(std::pair<int, int> (map.goal_i, map.goal_j));
    }

    return successors;
}

void Rstar::generateCirleOfSuccessors()
{
    successors_circle.resize(0);
    int di = distance_to_successors,
        dj = 0;

    for(; di >= 0 && di >= dj; di--) {
        while (dj * dj + di * di <= distance_to_successors * distance_to_successors) {
            successors_circle.push_back(std::pair<int,int>(di, dj));
            successors_circle.push_back(std::pair<int,int>(dj, di));
            successors_circle.push_back(std::pair<int,int>(-di, dj));
            successors_circle.push_back(std::pair<int,int>(-dj, di));
            successors_circle.push_back(std::pair<int,int>(di, -dj));
            successors_circle.push_back(std::pair<int,int>(dj, -di));
            successors_circle.push_back(std::pair<int,int>(-di, -dj));
            successors_circle.push_back(std::pair<int,int>(-dj, -di));
            dj++;
        }
    }

    std::sort(successors_circle.begin(), successors_circle.end());
    successors_circle.erase(std::unique(successors_circle.begin(), successors_circle.end()),
                            successors_circle.end());
}

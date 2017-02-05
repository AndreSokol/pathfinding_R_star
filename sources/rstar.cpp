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

    std::srand(time(NULL));
    //distribution = std::uniform_real_distribution<double>(0, 1);
}

Rstar::~Rstar()
{

}

SearchResult Rstar::startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options) {
    auto start_time = std::chrono::system_clock::now();

    generateCirleOfSuccessors();

    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max");


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

            if (current_node == goal) {
                sresult.pathfound = true;
                break;
            }

            auto current_node_iterator = closed.insert(current_node).first;

            for (auto child_coords : generateSuccessors(current_node, map, closed)) {
                child_node = Node(child_coords.first, child_coords.second);

                child_node.parent = &(*current_node_iterator); // FIXME: not sure all this is a must

                child_node.g = current_node.g + localSearchResult.pathlength;

                calculateHeuristic(child_node, map, options);

                open.push(child_node);
            }
        }
        else if(!current_node.AVOID) {
            current_node.AVOID = true;
            // FIXME: recount heuristics
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

std::vector<std::pair<int, int> > Rstar::generateSuccessors(const Node &node, const Map &map,
                                                            const std::unordered_set<Node> &closed)
{
    std::vector< std::pair<int, int> > successors;
    std::vector< std::pair<int, int> > predecessors;

    Node parent = node;

    for (; parent.parent != nullptr; parent = *parent.parent) {
        predecessors.push_back(std::pair<int,int>(parent.i, parent.j));
    }
    predecessors.push_back(std::pair<int,int>(parent.i, parent.j));

    long double angle;

    int successor_di,
        successor_dj;
    std::pair<int, int> successor;

    if ((node.i - map.goal_i) * (node.i - map.goal_i) +
            (node.j - map.goal_j) * (node.j - map.goal_j) <=
                (int) (distance_to_successors * distance_to_successors)) {
        successors.push_back(std::pair<int, int> (map.goal_i, map.goal_j));
    }

    if (map.height < distance_to_successors && map.width < distance_to_successors) {
        std::cerr << "[WARNING] Distance to successors is bigger than the map \n";
        return successors;
    }

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
        if (std::find(successors.begin(), successors.end(), successor) != successors.end()) {
            continue;
        }
        if (std::find(predecessors.begin(), predecessors.end(), successor) != predecessors.end()) {
            continue;
        }
        if (closed.count(Node(successor.first, successor.second)) != 0) {
            continue;
        }

        successors.push_back(successor);
    }

    int element_to_erase;
    while(successors.size() > number_of_successors) {
        element_to_erase = rand() % successors.size();

        successors.erase(successors.begin() + element_to_erase);
    }

    return successors;
}

/*long double Rstar::generateRandomValue()
{

    return distribution(random_engine);
}*/

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

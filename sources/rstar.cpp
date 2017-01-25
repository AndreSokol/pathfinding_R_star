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

Rstar::Rstar(double weight, int BT, int SL)
{
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;

    distribution = std::uniform_real_distribution<double>(0, 2 * PI);
}

Rstar::~Rstar()
{

}

SearchResult Rstar::startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options) {
    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max");

    auto start_time = std::chrono::system_clock::now();

    Node start;
    start.i = map.start_i;
    start.j = map.start_j;
    start.g = 0;

    Node goal;
    goal.i = map.goal_i;
    goal.j = map.goal_j;

    open.push(start);

    Node current_node, child_node;

    bool localPathFound;

    while(!open.empty()) {
        current_node = open.pop();

        if (current_node.AVOID && start != current_node) {
            localPathFound = findLocalPath(current_node, *current_node.parent, map, logger, options, -1).pathfound;
            // no step limit if already marked AVOID
        }
        else if (start != current_node) {
            localPathFound = findLocalPath(current_node, *current_node.parent, map,
                                           logger, options, local_search_step_limit).pathfound;
        }


        if(localPathFound || start == current_node) {

            if (current_node == goal) {
                sresult.pathfound = true;
                break;
            }

//          std::cerr << "Closed size: " << closed.size() + 1 << ", open size: " << open.size() << "\n";

            auto current_node_iterator = closed.insert(current_node).first;

            for (auto child_coords : generateSuccessors(current_node, map)) {
                child_node = Node(child_coords.first, child_coords.second);

                child_node.parent = &(*current_node_iterator); // FIXME: not sure all this is a must

                calculateHeuristic(child_node, map, options);

                open.push(child_node);
            }
        }
        else if(!current_node.AVOID) {
            current_node.AVOID = true;
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

    for(size_t i = 0; i < number_of_successors; i++) {
        angle = generateRandomAngle();
        successor_di = ((long double) distance_to_successors) * std::cos(angle);
        successor_dj = ((long double) distance_to_successors) * std::sin(angle);

        successor.first = node.i + successor_di;
        successor.second = node.j + successor_dj;

        if(!map.CellOnGrid(successor.first, successor.second)) {
            i--;
            continue;
        }
        if(map.CellIsObstacle(successor.first, successor.second)) {
            i--;
            continue;
        }
        if (std::find(successors.begin(), successors.end(), successor) != successors.end()) {
            continue;
        }

        successors.push_back(successor);
    }

    return successors;
}

long double Rstar::generateRandomAngle()
{

    return distribution(random_engine);
}

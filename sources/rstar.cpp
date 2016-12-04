#include "rstar.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "map.h"
#include "node.h"
#include "opencontainer.h"
#include <unordered_set>
#include <random>
#include <ctime>
#include <cmath>
#include <algorithm>

#include "isearch.h"
#include "astar.h"
#include "searchresult.h"

Rstar::Rstar()
{

}

Rstar::Rstar(double weight, int BT, int SL)
{
    hweight = weight;
    breakingties = BT;
    sizelimit = SL;
}

Rstar::~Rstar()
{

}

SearchResult Rstar::startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options) {
    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max");

    //size_t NUMBER_OF_CHILDREN = 10;
    //size_t STEPS_UNTIL_STOP = 100;

    //auto start_time = std::chrono::system_clock::now();

    Node start;
    start.i = map.start_i;
    start.j = map.start_j;
    start.g = 0;

    Node goal;
    goal.i = map.goal_i;
    goal.j = map.goal_j;

    open.push(start);

    sresult.pathfound = false;
    sresult.numberofsteps = 0;

    Node current_node, child_node;

    bool localPathFound;

    while(!open.empty()) {
        current_node = open.pop();
        //if (closed.count(current_node) != 0) continue;

        if (current_node.AVOID) {
            localPathFound = findLocalPath(current_node, *current_node.parent, map, logger, options, -1);
            // no step limit if already marked AVOID
        }
        else {
            localPathFound = findLocalPath(current_node, *current_node.parent, map,
                                           logger, options, local_search_step_limit);
        }


        if(localPathFound) {
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

    return sresult;
}

bool Rstar::findLocalPath(const Node &node, const Node &parent_node, const Map &map,
                          ILogger *logger, const EnvironmentOptions &options, int localSearchStepLimit)
{
    Astar localSearch(hweight, breakingties, localSearchStepLimit);
    localSearch.setAlternativePoints(parent_node, node);
    SearchResult localSearchResult = localSearch.startSearch(logger, map, options);
    return localSearchResult.pathfound;
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

    std::random_device random_device;
    std::mt19937 generator(random_device());
    std::uniform_real_distribution<> distribution(-1, 1);

    long double angle;

    int successor_di,
        successor_dj;
    std::pair<int, int> successor;

    for(size_t i = 0; i < number_of_successors; i++) {
        angle = distribution(generator);
        successor_di = ((long double) distance_to_successors) * std::acos(angle);
        successor_dj = ((long double) distance_to_successors) * std::asin(angle);

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

/*void Rstar::reevaluateNode(Node &node)
{
    findLocalPath(node);
    if (!node.local_path_found) {
        return;
    }
}*/

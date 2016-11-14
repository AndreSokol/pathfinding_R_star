#include "jpsearch.h"
#include "map.h"
#include "ilogger.h"
#include "environmentoptions.h"
#include <set>
#include <chrono>
#include "opencontainer.h"
#include <cmath>

JPSearch::JPSearch(double w, int BT, int SL)
{
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

SearchResult JPSearch::startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options)
{
    std::set<Node> closed;

    OpenContainer<Node> open("g-max"); // must be import from map, haven't found it yet

    auto start_time = std::chrono::system_clock::now();

    Node start;
    start.i = Map.start_i;
    start.j = Map.start_j;
    start.g = 0;

    Node goal;
    goal.i = Map.goal_i;
    goal.j = Map.goal_j;

    open.push(start);

    sresult.pathfound = false;
    sresult.numberofsteps = 0;

    Node current_node, new_node;
    bool is_diagonal;
    while (!open.empty()) {
        current_node = open.pop();
        if (closed.count(current_node) != 0) continue;

        auto current_node_iterator = closed.insert(current_node).first;

        int i, j;

        for (auto delta : generateCanonicalOrdering(current_node, Map, options)) {
            i = delta.first;
            j = delta.second;

            if (i * j != 0) { // this means i != 0 and j != 0
                if (options.allowdiagonal == 0) continue;
                is_diagonal = true;
            }
            else {
                is_diagonal = false;
            }

            if (!Map.CellOnGrid(current_node.i + i, current_node.j + j)) continue;
            if (Map.CellIsObstacle(current_node.i + i, current_node.j + j)) continue;
            if (is_diagonal) {
                if (Map.CellIsObstacle(current_node.i + i, current_node.j) &&
                            Map.CellIsObstacle(current_node.i, current_node.j + j) &&
                            !options.allowsqueeze) continue;
            }

            new_node = Node(current_node.i + i, current_node.j + j);

            std::pair<bool,Node> jump_result = jump(new_node, i, j, Map, goal, options);

            if(jump_result.first) {
                new_node.i = jump_result.second.i;
                new_node.j = jump_result.second.j;
                new_node.parent = &(*current_node_iterator);
                calculateHeuristic(new_node, Map, options);

                open.push(new_node);

                if(new_node == goal) {
                    sresult.pathfound = true;
                    break;
                }
            }
        }
        sresult.numberofsteps++;
        if(sresult.pathfound) break;
    }

    auto end_time = std::chrono::system_clock::now();
    sresult.time = (std::chrono::duration<double>(end_time - start_time)).count();

    sresult.nodescreated = open.size() + closed.size();

    if (sresult.pathfound) {
        sresult.pathlength = 0;

        current_node = new_node;

        sresult.lppath = new NodeList();
        sresult.hppath = new NodeList();
        sresult.lppath->push_front(current_node);
        sresult.hppath->push_front(current_node);

        int di, dj;

        while (!(current_node == start)) {
            new_node = *current_node.parent;
            sresult.hppath->push_front(current_node);

            if      (current_node.i - new_node.i > 0)  di = -1;
            else if (current_node.i - new_node.i == 0) di = 0;
            else                                       di = 1;

            if      (current_node.j - new_node.j > 0)  dj = -1;
            else if (current_node.j - new_node.j == 0) dj = 0;
            else                                       dj = 1;

            while (!(current_node == new_node)) {
                sresult.lppath->push_front(current_node);
                current_node.i += di;
                current_node.j += dj;
                if (di * dj == 0)
                    sresult.pathlength += options.linecost;
                else
                    sresult.pathlength += options.diagonalcost;
            }

            current_node = *current_node.parent;
        }

        // Adding start node to path
        sresult.lppath->push_front(current_node);
        sresult.hppath->push_front(current_node);
    }
    else {
        sresult.lppath = NULL;
        sresult.hppath = NULL;
        sresult.pathlength = 0;
    }

    return sresult;
}

std::pair<bool, Node> JPSearch::jump(const Node &node, int di, int dj, const Map &map, const Node &goal, const EnvironmentOptions &options)
{
    // just to be sure, some problems there
    if (map.CellIsObstacle(node.i, node.j)) return std::pair<bool, Node>(false, node);

    // goal point is also the jump point
    if (node == goal) return std::pair<bool, Node>(true, node);

    if (di * dj == 0) { // moving along lines
        if (map.CellIsObstacle(node.i + di, node.j + dj)) return std::pair<bool, Node>(false, node);

        // check if we found jump point
        if (dj == 0) {
            if (map.CellIsObstacle(node.i, node.j + 1) &&
                !map.CellIsObstacle(node.i + di, node.j + 1)) return std::pair<bool, Node>(true, node);
            if (map.CellIsObstacle(node.i, node.j - 1) &&
                !map.CellIsObstacle(node.i + di, node.j - 1)) return std::pair<bool, Node>(true, node);
        }

        if (di == 0) {
            if (map.CellIsObstacle(node.i + 1, node.j) &&
                !map.CellIsObstacle(node.i + 1, node.j + dj)) return std::pair<bool, Node>(true, node);
            if (map.CellIsObstacle(node.i - 1, node.j) &&
                !map.CellIsObstacle(node.i - 1, node.j + dj)) return std::pair<bool, Node>(true, node);
        }

        // check if we reached border
        if (!map.CellOnGrid(node.i + di, node.j + dj)) return std::pair<bool, Node>(false, node);

        // can move forward in any other case
        return jump(Node(node.i + di, node.j + dj), di, dj, map, goal, options);
    }
    else { // moving diagonal
        if (map.CellIsObstacle(node.i - di, node.j) &&
                !map.CellIsObstacle(node.i - di, node.j + dj)) return std::pair<bool,Node>(true, node);
        if (map.CellIsObstacle(node.i, node.j - dj) &&
                !map.CellIsObstacle(node.i + di, node.j - dj)) return std::pair<bool,Node>(true, node);


        if (jump(Node(node.i + di, node.j), di, 0, map, goal, options).first) return std::pair<bool,Node>(true, node);
        if (jump(Node(node.i, node.j + dj), 0, dj, map, goal, options).first) return std::pair<bool,Node>(true, node);

        if (map.CellIsObstacle(node.i + di, node.j + dj)) return std::pair<bool, Node>(false, node);

        if (map.CellIsObstacle(node.i + di, node.j) && map.CellIsObstacle(node.i, node.j + dj) &&
                !options.allowsqueeze) return std::pair<bool, Node>(false, node);

        // check if we reached border
        if (!map.CellOnGrid(node.i + di, node.j + dj)) return std::pair<bool, Node>(false, node);

        return jump(Node(node.i + di, node.j + dj), di, dj, map, goal, options);
    }
}

void JPSearch::calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options)
{

    int di = abs(a.i - a.parent->i),
        dj = abs(a.j - a.parent->j);

    a.g = a.parent->g;
    if(options.allowdiagonal)
        a.g += std::min(di, dj) * options.diagonalcost + abs(di - dj) * options.linecost;
    else
        a.g += (di + dj) * options.linecost;


    a.F = a.g;

    di = abs(a.i - map.goal_i);
    dj = abs(a.j - map.goal_j);

    // Normalizing heuristics with linecost
    if (options.metrictype == CN_SP_MT_EUCL)      a.H = sqrt(di * di + dj * dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_MANH) a.H = (di + dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_DIAG) a.H = std::min(di, dj) * options.diagonalcost +
                                                                        abs(di - dj) * options.linecost;
    else a.H = std::max(di, dj) * options.linecost;

    a.F += hweight * a.H;
}

std::vector<std::pair<int, int> > JPSearch::generateCanonicalOrdering(const Node &current_node, const Map &map, const EnvironmentOptions &options)
{
    typedef std::pair<int, int> int_pair;

    std::vector<int_pair> directions;

    // if current_node == start_node
    if (current_node.parent == nullptr) {
        directions = {int_pair(-1, 0),
                      int_pair(1, 0),
                      int_pair(0, -1),
                      int_pair(0, 1)};

        if (options.allowdiagonal) {
            directions.push_back(int_pair(-1, -1));
            directions.push_back(int_pair(-1, 1));
            directions.push_back(int_pair(1, -1));
            directions.push_back(int_pair(1, 1));
        }
        return directions;
    }

    int di, dj;

    if      (current_node.i - current_node.parent->i > 0)  di = 1;
    else if (current_node.i - current_node.parent->i == 0) di = 0;
    else                                                   di = -1;

    if      (current_node.j - current_node.parent->j > 0)  dj = 1;
    else if (current_node.j - current_node.parent->j == 0) dj = 0;
    else                                                   dj = -1;

    if (di * dj != 0) { // we came from diagonal direction
        directions.push_back(int_pair(di, 0));
        directions.push_back(int_pair(0, dj));
        directions.push_back(int_pair(di, dj));

        if(map.CellIsObstacle(current_node.i - di, current_node.j)) directions.push_back(int_pair(-di, dj));
        if(map.CellIsObstacle(current_node.i, current_node.j - dj)) directions.push_back(int_pair(di, -dj));
    }
    else { // we came from cardinal direction
        directions.push_back(int_pair(di, dj)); // just continue first

        if (di == 0) {
            if (map.CellIsObstacle(current_node.i + 1, current_node.j)) directions.push_back(int_pair(1, dj));
            if (map.CellIsObstacle(current_node.i - 1, current_node.j)) directions.push_back(int_pair(-1, dj));
        }
        else {
            if (map.CellIsObstacle(current_node.i, current_node.j + 1)) directions.push_back(int_pair(di, 1));
            if (map.CellIsObstacle(current_node.i, current_node.j - 1)) directions.push_back(int_pair(di, -1));
        }
    }

    return directions;
}

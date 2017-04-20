#include "astar.h"
#include <set>
#include "opencontainer.h"
#include <chrono>
#include <cmath>

Astar::Astar()
{

}

Astar::Astar(double w, int BT, int SL)
{
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

void Astar::setAlternativePoints(const Node &start, const Node &finish)
{
    this->start = start;
    this->goal = finish;
    alt_points_set = true;
}

Astar::~Astar()
{
}

SearchResult Astar::startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options)
{
    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max"); // must be import from map, haven't found it yet

    auto start_time = std::chrono::system_clock::now();

    if (!alt_points_set) {
        start.i = Map.start_i;
        start.j = Map.start_j;

        goal.i = Map.goal_i;
        goal.j = Map.goal_j;
    }
    start.g = 0;

    open.push(start);

    sresult.pathfound = false;
    sresult.numberofsteps = 0;

    Node current_node, new_node;
    bool is_diagonal;
    while (!open.empty() && sresult.numberofsteps < sizelimit) {
        current_node = open.pop();
        if (closed.count(current_node) != 0) continue;

        auto current_node_iterator = closed.insert(current_node).first;
        sresult.max_F = std::max(sresult.max_F, current_node.F);

        for (int i = -1; i <= 1; i++) {
            for (int j = -1; j <= 1; j++) {
                if (i == 0 && j == 0) continue;

                if (i * j != 0) { // this means i != 0 and j != 0
                    if (options.allowdiagonal == 0) continue;
                    is_diagonal = true;
                }
                else {
                    is_diagonal = false;
                }

                new_node.parent = &(*current_node_iterator);
                new_node.i = current_node.i + i;
                new_node.j = current_node.j + j;

                if (!Map.CellOnGrid(new_node.i, new_node.j)) continue;
                if (Map.CellIsObstacle(new_node.i, new_node.j)) continue;
                if (closed.count(new_node) != 0) continue;
                if (is_diagonal)
                    if (!options.allowsqueeze &&
                            (Map.CellIsObstacle(new_node.i, current_node.j) && Map.CellIsObstacle(current_node.i, new_node.j)))
                                    continue;

                if (is_diagonal)
                    new_node.g = current_node.g + options.diagonalcost;
                else
                    new_node.g = current_node.g + options.linecost;

                updateParent(new_node, Map, options); // placed for ThetaStar search, do nothing here
                calculateHeuristic(new_node, Map, options);

                open.push(new_node);

                if(new_node == goal) {
                    sresult.pathfound = true;
                    break;
                }
            }
            if(sresult.pathfound) break;
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

        while (!(current_node == start)) {
            sresult.pathlength += getHopLength(current_node, options);

            sresult.lppath->push_front(current_node);
            sresult.hppath->push_front(current_node);

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

void Astar::updateParent(Node &node, const Map &map, const EnvironmentOptions &options)
{

}

void Astar::calculateHeuristic(Node & a, const Map &map, const EnvironmentOptions &options)
{
    a.F = a.g;
    int di = abs(a.i - goal.i),
        dj = abs(a.j - goal.j);

    // Normalizing heuristics with linecost
    if (options.metrictype == CN_SP_MT_EUCL) a.H = sqrt(di * di + dj * dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_MANH) a.H = (di + dj) * options.linecost;
    else if (options.metrictype == CN_SP_MT_DIAG) a.H = std::min(di, dj) * options.diagonalcost +
                                                                        abs(di - dj) * options.linecost;
    else a.H = std::max(di, dj) * options.linecost;

    a.F += hweight * a.H;
}

double Astar::getHopLength(const Node &a, const EnvironmentOptions &options)
{
    if ((a.i - a.parent->i) * (a.j - a.parent->j) == 0) return options.linecost;

    return options.diagonalcost;
}

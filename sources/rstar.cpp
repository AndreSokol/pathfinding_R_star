#include "rstar.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "map.h"
#include "node.h"
#include "opencontainer.h"
#include <unordered_set>

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

SearchResult Rstar::startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options) {
    std::unordered_set<Node> closed;

    OpenContainer<Node> open("g-max");

    size_t NUMBER_OF_CHILDREN = 10;
    size_t STEPS_UNTIL_STOP = 100;

    //auto start_time = std::chrono::system_clock::now();

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


    Node current_node;
    while(!open.empty()) {
        current_node = open.pop();
    }

    return sresult;
}

void Rstar::findLocalPath(const Node &node, const Node &parent_node, const OpenContainer &open)
{

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

#include "rstar.h"
#include "environmentoptions.h"
#include "ilogger.h"
#include "map.h"
#include "node.h"
#include "opencontainer.h"
#include <unordered_set>

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

    size_t NUMBER_OF_CHILDREN = 10;
    size_t STEPS_UNTIL_STOP = 100;

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


    Node current_node;

    findLocalPath(goal, start, map, open, logger, options);

    return sresult;
}

void Rstar::findLocalPath(Node &node, const Node &parent_node, const Map &map, const OpenContainer<Node> &open,
                          ILogger *logger, const EnvironmentOptions &options)
{
    Astar localSearch(hweight, breakingties, -1/*local_search_step_limit*/);
    localSearch.setAlternativePoints(parent_node, node);
    SearchResult localSearchResult = localSearch.startSearch(logger, map, options);
    sresult = localSearchResult;
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

/*void Rstar::reevaluateNode(Node &node)
{
    findLocalPath(node);
    if (!node.local_path_found) {
        return;
    }
}*/

#ifndef RSTAR_H
#define RSTAR_H

#include "isearch.h"
#include "searchresult.h"
#include "node.h"
#include "map.h"
#include "environmentoptions.h"
#include "opencontainer.h"

#include <vector>
#include <utility>

class Rstar : public ISearch
{
public:
    Rstar();
    Rstar(double weight, int BT, int SL);
    ~Rstar();
    SearchResult startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options);
    //bool F_cmp(Node, Node);

protected:
    SearchResult findLocalPath(const Node &node, const Node &parent_node, const Map &map,
                       ILogger *logger, const EnvironmentOptions &options, int localSearchStepLimit);
    void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    //void updateNode(Node &node, const Map &map, const EnvironmentOptions &options);
    //void reevaluateNode(Node &node);
    //virtual double getHopLength(const Node &a, const EnvironmentOptions &options);

    std::vector< std::pair<int,int> > generateSuccessors(const Node &node, const Map &map);
    int local_search_step_limit = 100000;
    size_t number_of_successors = 10;
    size_t distance_to_successors = 50;
};

#endif // RSTAR_H

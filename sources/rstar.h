#ifndef RSTAR_H
#define RSTAR_H

#include "isearch.h"
#include "searchresult.h"
#include "node.h"
#include "map.h"
#include "environmentoptions.h"
#include "opencontainer.h"

class Rstar : public ISearch
{
public:
    Rstar();
    Rstar(double weight, int BT, int SL);
    ~Rstar();
    SearchResult startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options);
    //bool F_cmp(Node, Node);

protected:
    void findLocalPath(Node &node, const Node &parent_node, const Map &map, const OpenContainer<Node> &open,
                       ILogger *logger, const EnvironmentOptions &options);
    void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    //void updateNode(Node &node, const Map &map, const EnvironmentOptions &options);
    //void reevaluateNode(Node &node);
    //virtual double getHopLength(const Node &a, const EnvironmentOptions &options);

    size_t number_of_children = 10;
    size_t local_search_step_limit = 100;
};

#endif // RSTAR_H

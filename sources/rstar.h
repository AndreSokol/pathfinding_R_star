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
    SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);
    //bool F_cmp(Node, Node);

protected:
    void findLocalPath(const Node &node, const Node &parent_node, const OpenContainer &open);
    void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    void updateNode(Node &node, const Map &map, const EnvironmentOptions &options);
    //virtual double getHopLength(const Node &a, const EnvironmentOptions &options);

};

#endif // RSTAR_H

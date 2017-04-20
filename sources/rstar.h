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
#include <random>
#include <cmath>

class Rstar : public ISearch
{
public:
    Rstar();
    Rstar(double weight, int BT, int SL, int distance_to_successors,
          int number_of_successors, int local_search_step_limit);
    ~Rstar();
    SearchResult startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options);

protected:
    SearchResult findLocalPath(const Node &node, const Node &parent_node, const Map &map,
                       ILogger *logger, const EnvironmentOptions &options, int localSearchStepLimit);
    void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    bool isNodePerpective(Node &a, const Map &map, const EnvironmentOptions &options);

    std::vector< std::pair<int,int> > generateSuccessors(const Node &node, const Map &map);

    int local_search_step_limit = 3000;
    int number_of_successors = 10;
    int distance_to_successors = 2;

    std::vector< std::pair<int,int> > successors_circle;

    void generateCirleOfSuccessors();
};

#endif // RSTAR_H

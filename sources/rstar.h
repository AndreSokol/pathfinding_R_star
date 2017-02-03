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
    long double generateRandomValue();

    int local_search_step_limit = 3000;
    size_t number_of_successors = 10;
    size_t distance_to_successors = 2;

    std::default_random_engine random_engine;
    std::uniform_real_distribution<double> distribution;

    std::vector< std::pair<int,int> > successors_circle;

private:
    void generateCirleOfSuccessors();
};

#endif // RSTAR_H

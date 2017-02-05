#ifndef RJPSEARCH_H
#define RJPSEARCH_H

#include "isearch.h"
#include "searchresult.h"
#include "node.h"
#include "map.h"
#include "environmentoptions.h"
#include "opencontainer.h"


class RJPSearch : public ISearch
{
public:
    RJPSearch();
    RJPSearch(double weight, int BT, int SL, int distance_to_successors,
          int number_of_successors, int local_search_step_limit);
    ~RJPSearch();
    SearchResult startSearch(ILogger *logger, const Map &map, const EnvironmentOptions &options);

private:
    SearchResult findLocalPath(const Node &node, const Node &parent_node, const Map &map,
                       ILogger *logger, const EnvironmentOptions &options, int localSearchStepLimit);
    void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    std::vector< std::pair<int,int> > generateSuccessors(const Node &node, const Map &map,
                                                         const std::unordered_set<Node> &closed);
    void generateCirleOfSuccessors();
    bool lineOfSight(const Node &p, const Node &q, const Map &map);

    std::vector< std::pair<int,int> > successors_circle;
    int local_search_step_limit = 3000;
    int number_of_successors = 10;
    int distance_to_successors = 2;
};

#endif // RJPSEARCH_H

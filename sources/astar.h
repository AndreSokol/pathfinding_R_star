#ifndef ASTAR_H
#define ASTAR_H
#include "gl_const.h"
#include "isearch.h"
#include "ilogger.h"
#include <stdlib.h>
#include <math.h>

class Astar : public ISearch
{
public:
    Astar();
    Astar(double weight, int BT, int SL);
    Astar(double weight, int BT, int SL, const Node &start, const Node &finish);
    ~Astar();
    SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);
    bool F_cmp(Node, Node);

    void setAlternativePoints(const Node &start, const Node &finish);

protected:
    virtual void updateParent(Node &node, const Map &map, const EnvironmentOptions &options);
    virtual void calculateHeuristic(Node &a, const Map &map, const EnvironmentOptions &options);
    virtual double getHopLength(const Node &a, const EnvironmentOptions &options);

private:
    Node start;
    Node goal;
    bool alt_points_set = false;
};

#endif

#ifndef ISEARCH_H
#define ISEARCH_H
#include "list.h"
#include "map.h"
#include "ilogger.h"
#include "searchresult.h"
#include "environmentoptions.h"
#include <unordered_map>
#include <math.h>
class ISearch
{
    public:
        ISearch();
        virtual ~ISearch(void);
        virtual SearchResult startSearch(ILogger *Logger, const Map &Map, const EnvironmentOptions &options);

    protected:
        SearchResult    sresult;
        int     sizelimit; //ограничение на размер OPEN
        float   hweight; //вес эвристики
        int     breakingties; //критерий выбора очередной вершины из OPEN, когда F-значения равны

};
#endif

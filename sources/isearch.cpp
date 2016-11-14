#include "gl_const.h"
#include "isearch.h"
#include <iostream>
#include <vector>
#ifdef __linux__
    #include <sys/time.h>
#else
    #include <windows.h>
#endif


ISearch::ISearch()
{
}

ISearch::~ISearch(void)
{
}

SearchResult ISearch::startSearch(ILogger *Logger, const Map &map, const EnvironmentOptions &options)
{
    return sresult;
}


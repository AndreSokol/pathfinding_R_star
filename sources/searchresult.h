#ifndef SEARCHRESULT_H
#define SEARCHRESULT_H

#include "list.h"
#include <ctime>
#include <unordered_map>

struct SearchResult
{
        bool pathfound; //���� ������
        float pathlength; //����� ����, ���� ���� �� ������, �� ����� 0
        /*const*/ NodeList* lppath; //���� � ���� ������������������ ������
        /*const*/ NodeList* hppath; //���� �������� �� ������
        unsigned int nodescreated; //|OPEN| + |CLOSE| = ����� ��������� ����� (= ���������� ��������� ������������� ������)
        unsigned int numberofsteps; //����� �������� (= ���������� ��������� ������������� ������)
        double time; //���������� ����������� ����� (= "����������" (�� ��������� �� ���������� ����������) ���������� ��������� ������������� ������)
        double max_F = 0; // maximum F-value (for R*)

        SearchResult()
        {
            pathfound = false;
            pathlength = 0;
            lppath = NULL;
            hppath = NULL;
            nodescreated = 0;
            numberofsteps = 0;
            time = 0;
        }

        void JoinLpPaths (SearchResult to_append) {
            pathlength += to_append.pathlength;

            auto it = to_append.lppath->List.end();
            it--;
            for (; it != to_append.lppath->List.begin(); it--) {
                lppath->push_front(*it);
            }
            lppath->push_front(*it);
        }
};

#endif

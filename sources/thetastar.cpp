#include "thetastar.h"

ThetaStar::ThetaStar(double w, int BT, int SL)
{
    hweight = w;
    breakingties = BT;
    sizelimit = SL;
}

void ThetaStar::updateParent(Node &node, const Map &map, const EnvironmentOptions &options)
{
    if (node.parent == nullptr) return;

    while (node.parent->parent != nullptr) {
        if (!lineOfSight(node, *(node.parent->parent), map)) break;
        node.parent = node.parent->parent;
    }

    int di = abs(node.i - node.parent->i),
        dj = abs(node.j - node.parent->j);

    node.g = node.parent->g + sqrt(di * di + dj * dj) * options.linecost;
}

double ThetaStar::getHopLength(const Node &a, const EnvironmentOptions &options)
{
    int di = abs(a.i - a.parent->i),
        dj = abs(a.j - a.parent->j);

    return sqrt(di * di + dj * dj) * options.linecost;
}

bool ThetaStar::lineOfSight(const Node &p, const Node &q, const Map &map)
{
    int x_1 = p.i,
        y_1 = p.j,
        x_2 = q.i,
        y_2 = q.j;

    if (x_2 < x_1) {
        // to ensure x grows from x_1 to x_2
        int temp = x_1;
        x_1 = x_2;
        x_2 = temp;

        temp = y_1;
        y_1 = y_2;
        y_2 = temp;
    }

    int dx = x_2 - x_1,
        dy = y_2 - y_1;

    // a * x + b * y + c = 0, equotation of the line through (x1, y1) and (x2, y2)
    // multiplying by 2 to check grid intersection points easily (add 1 instead of 0.5 to each of coordinates, ints only)
    int a = 2 * dy,
        b = - 2 * dx,
        c = 0 - a * x_1 - b * y_1;

    int growth_y;
    if (dy > 0) growth_y = 1;
    else        growth_y = -1;

    int current_x = x_1,
        current_y = y_1;

    while (current_x != x_2 || current_y != y_2) {
        if (map.CellIsObstacle(current_x, current_y)) return false;

        // let's check if the line (a, b, c) will cross the Ox-parallel line of grid in this cell
        if (a * current_x + a / 2 + b * current_y + b * growth_y / 2 + c == 0) {
            // this happens when we're on the intersection, just skip forward, we can squeeze around corner
            current_x += 1;
            current_y += growth_y;
        }
        else if (growth_y * (a * current_x + a / 2 + b * current_y + b * growth_y / 2 + c) < 0)
                        current_x += 1; // not crossing at current x, move horizontally
        else            current_y += growth_y; // crossing, move vertically
    }

    return true;
}

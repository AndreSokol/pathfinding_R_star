#ifndef NODE_H
#define NODE_H

#include <cstdio>
#include <functional>
#include <utility>

struct Node
{
    int i, j;
    mutable double F, g, H;
    mutable const Node *parent = nullptr;
    std::pair<int,int> parent_coords;
    bool AVOID = false;
    bool local_path_found = false;
    Node(){}
    Node(int i, int j){this->i=i;this->j=j;}

    void update(const Node &a) {
        if (g > a.g) {
            g = a.g;
            F = a.F;
            H = a.H;
            parent = a.parent;
        }
    }

    friend bool operator==(const Node & a, const Node & b) {
        return a.i == b.i && a.j == b.j;
    }

    friend bool operator!=(const Node & a, const Node & b) {
        return !(a == b);
    }

    friend bool operator<(const Node & a, const Node & b) {
        if(a.i == b.i) return a.j < b.j;
        return a.i < b.i;
    }

    friend bool operator>(const Node & a, const Node & b) {
        if(a.i == b.i) return a.j > b.j;
        return a.i > b.i;
    }
};

namespace std {
    template<> struct hash<Node> {
        std::size_t operator()(Node const& k) const {
            return std::hash<int>()(k.i) ^ (std::hash<int>()(k.j) << 1);
        }
    };
}

#endif

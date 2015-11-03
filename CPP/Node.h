#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <set>
#include <math.h>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>

namespace std
{
    class Node
    {
        public:
            int row, col;
            double gScore, fScore, time;
            Node *prev;

            Node(int r, int c, double fS, Node *p, double t);
            Node(int r, int c, double fS, double gS, Node *p, double t);
            Node(int r, int c);

            bool operator ==(const Node &node) const;
            bool operator !=(const Node &node) const;
            bool operator <(const Node &node) const;
            bool operator >(const Node &node) const;
            bool operator <=(const Node &node) const;
            bool operator >=(const Node &node) const;
            bool operator ()(const Node &lhs, const Node &rhs) const;
    };
};

#endif
#ifndef __NODE_H__
#define __NODE_H__

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <math.h>

namespace std
{
    class Node
    {
        public:
            int row, col, time;
            double gScore, fScore;
            Node *prev;

            Node(int r, int c, double fS, Node *p, int t);
            Node(int r, int c, double fS, double gS, Node *p, int t);
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
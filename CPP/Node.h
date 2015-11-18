#ifndef __NODE_H__
#define __NODE_H__

namespace std
{
    class Node
    {
        public:
            int row, col;
            int uNum;
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
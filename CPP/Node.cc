#include "Node.h"

namespace std
{
    Node::Node(int r, int c, double fS, Node *p, int t)
    {
        row = r;
        col = c;
        fScore = fS;
        gScore = 0;
        prev = p;
        time = 0;
    }

    Node::Node(int r, int c, double fS, double gS, Node *p, int t)
    {
        row = r;
        col = c;
        fScore = fS;
        gScore = gS;
        prev = p;
        time = 0;
    }

    Node::Node(int r, int c)
    {
        row = r;
        col = c;
        fScore = 999999;
        gScore = 999999;
        prev = NULL;
        time = 0;
    }

    bool Node::operator ==(const Node &node) const
    {
        return this->row == node.row && this->col == node.col;
    }

    bool Node::operator !=(const Node &node) const
    {
        return this->row != node.row || this->col != node.col;
    }

    bool Node::operator <(const Node &node) const
    {
        return this->fScore < node.fScore;
    }

    bool Node::operator >(const Node &node) const
    {
        return this->fScore > node.fScore;
    }

    bool Node::operator <=(const Node &node) const
    {
        return this->fScore <= node.fScore;
    }

    bool Node::operator >=(const Node &node) const
    {
        return this->fScore >= node.fScore;
    }

    bool Node::operator () (const Node &lhs, const Node &rhs) const
    {
        return lhs.fScore < rhs.fScore;
    }

    template <>
    struct hash<pair<int, Node>>
    {
        size_t operator()(const pair<int, Node> &temporalNode) const
        {
            return (size_t)temporalNode.first ^ (size_t)temporalNode.second.row ^ (size_t)temporalNode.second.col % 17;
        }
    };

    template <>
    struct hash<Node>
        {
        size_t operator()(const Node &node) const
        {
            return (size_t)node.row ^ (size_t)node.col % 17;
        }
    };

    struct NodePtrComp
    {
        bool operator()(const Node* lhs, const Node* rhs) const  
        {
            if(lhs->fScore != rhs->fScore)
            {
                return lhs->fScore < rhs->fScore;
            }
            else
            {
                if(lhs->col != rhs->col)
                {
                    return lhs->col < rhs->col;
                }
                else
                {
                    return (lhs->row == rhs->row);
                }
            }
        }
    };
};
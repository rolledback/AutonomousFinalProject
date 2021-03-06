#include "Node.h"

namespace std
{
    Node::Node(int r, int c, double fS, Node *p, double t)
    {
        row = r;
        col = c;
        fScore = fS;
        gScore = 0;
        prev = p;
        time = t;
    }

    Node::Node(int r, int c, double fS, double gS, Node *p, double t)
    {
        row = r;
        col = c;
        fScore = fS;
        gScore = gS;
        prev = p;
        time = t;
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

#ifdef BOOST_HAS_TR1_HASH
namespace std {
    namespace tr1 {
#else
namespace boost {
#endif // def BOOST_HAS_TR1_HASH
    template <>
    struct hash<pair<int, Node> >
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
  
#ifdef BOOST_HAS_TR1_HASH
    }; // namespace tr1
}; // namespace std
  #else
}; // namespace boost
  #endif // def BOOST_HAS_TR1_HASH


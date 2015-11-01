#ifndef __PATHFIND_H__
#define __PATHFIND_H__

#define WIDTH 30
#define HEIGHT 20
#define GRANULARITY 5
#define GRID_WIDTH (WIDTH * GRANULARITY)
#define GRID_HEIGHT (HEIGHT * GRANULARITY)
#define MAX_TIME 5000
#define TIME_INTERVAL 1000

#include <vector>
#include <utility>
#include <queue>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <math.h>

using namespace std;

namespace std
{
    class Node
    {
        public:
            int row, col, gScore, fScore, time;
            Node *prev;

            Node(int r, int c, int fS, Node *p, int t)
            {
                row = r;
                col = c;
                fScore = fS;
                gScore = 0;
                prev = p;
                time = 0;
            }

            Node(int r, int c, int fS, int gS, Node *p, int t)
            {
                row = r;
                col = c;
                fScore = fS;
                gScore = gS;
                prev = p;
                time = 0;
            }

            Node(int r, int c)
            {
                row = r;
                col = c;
                fScore = 999999;
                gScore = 999999;
                prev = NULL;
                time = 0;
            }
    };

    struct NodeCompare
    {
        bool operator() (const Node *lhs, const Node *rhs) const
        {
            return lhs->fScore < rhs->fScore;
        }
    };

    template<> class hash<Node>
    {
        public:
            size_t operator() (const Node *node) const
            {
                return (size_t)node->row ^ (size_t)node->col % 17;
            }
    };

    template<> class hash<pair<int, Node>>
    {
        public:
            size_t operator() (const pair<int, Node> &temporalNode) const
            {
                return (size_t)temporalNode.first ^ (size_t)temporalNode.second.row ^ (size_t)temporalNode.second.col % 17;
            }
    };
};

namespace PathFinding
{
    class Compare
    {
        public:
            bool operator() (Node v1, Node v2)
            {
                if(v1.fScore < v2.fScore)
                {
                    return true;
                }
                else
                {
                    return false;
                }
            }
    };

    bool grid[GRID_HEIGHT][GRID_WIDTH];
    unordered_set<pair<int, Node>> reservationTable;

    vector<Node> makePath(Node *final)
    {
        vector<Node> retVector;
        while(final != NULL)
        {
            retVector.push_back(*final);
            final = final->prev;
        }
    }

    int heuristic(int r1, int c1, int r2, int c2)
    {
        int dr = r1 - r2;
        int dc = c1 - c2;
        return abs(r1 - r2) + abs(c1 - c2);
    }

    vector<Node> aStar(int sR, int sC, int gR, int gC, int t)
    {
        vector<Node> path;

        // data structures which contain nodes
        Node *nodes[GRID_HEIGHT * GRID_WIDTH];
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { nodes[n] = NULL; }

        unordered_set<Node*> closedSet;
        set<Node*, NodeCompare> openSet;

        // create starting node in the nodes array
        // 1. construct it
        // 2. set g score to 0
        // 3. set f score to the heuristic to the goal
        nodes[sR * GRID_WIDTH + sC] = new Node(sR, sC, 999999, 999999, NULL, t);
        nodes[sR * GRID_WIDTH + sC]->gScore = 0;
        nodes[sR * GRID_WIDTH + sC]->fScore = heuristic(sR, sC, gR, gC);

        // add node to open set data structures
        openSet.insert(nodes[sR * GRID_WIDTH + sC]);

        // do the A*
        while(openSet.size() != 0)
        {
            // get and remove top node from our open set queue
            Node current = **openSet.begin();
            openSet.erase(&current);

            // add it to the closed set
            closedSet.insert(&current);

            // iterate through the neighbors in all 8 directions
            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
                    // coordinates and time of the neighbor
                    int newRow = current.row + i;
                    int newCol = current.col + j;
                    int newTime = current.time + TIME_INTERVAL;

                    if(nodes[newRow * GRID_WIDTH + newCol] == NULL)
                    {
                        // not really sure about the logic of this
                        nodes[newRow * GRID_WIDTH + newCol] = new Node(newRow, newCol);
                    }

                    // temporary objects for lookup in the reservation table
                    pair<int, Node> tempTN(newTime, *nodes[newRow * GRID_WIDTH + newCol]);

                    if(newRow == gR && newCol == gC)
                    {
                        nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                        nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                        path = makePath(nodes[newRow * GRID_WIDTH + newCol]);

                        // free the node array
                        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

                        // found the goal
                        return path;
                    }

                    if(newRow < 0 || newRow > GRID_HEIGHT || newCol < 0 || newCol > GRID_WIDTH)
                    {
                        // out of bounds
                        continue;
                    }
                    else if(closedSet.find(nodes[newRow * GRID_WIDTH + newCol]) != closedSet.end())
                    {
                        // if it is in the closed set, we have already visited here
                        continue;
                    }
                    else if (reservationTable.find(tempTN) != reservationTable.end())
                    {
                        // if it already reserved, then we can't go here
                        continue;
                    }

                    int tentativeGScore = current.gScore + 1;

                    if(openSet.find(nodes[newRow * GRID_WIDTH + newCol]) == openSet.end())
                    {
                        // we have discovered a new node
                        openSet.insert(nodes[newRow * GRID_WIDTH + newCol]);
                    }
                    else if(tentativeGScore >= nodes[newRow * GRID_WIDTH + newCol]->gScore)
                    {
                        // not a better path
                        continue;
                    }

                    // best we have so far, record it
                    nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                    nodes[newRow * GRID_WIDTH + newCol]->gScore = tentativeGScore;
                    nodes[newRow * GRID_WIDTH + newCol]->fScore = tentativeGScore + heuristic(newRow, newCol, gR, gC);
                    nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                }
            }
        }

        // free the node array
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

        // return the path, it will have size 0
        return path;
    }

    void resetGrid()
    {
        for(int i = 0; i < HEIGHT * GRANULARITY; i++)
        {
            for(int j = 0; j < WIDTH * GRANULARITY; j++)
            {
                grid[i][j] = true;
            }
        }
    }

    void convertToGridCoordinates(float x, float y, int &r, int &c)
    {

    }

    void convertToRoboCoordinates(float &x, float &y, int r, int c)
    {

    }

    void drawGrid()
    {

    }

    void drawPath(vector<pair<int, int>> path)
    {

    }

    int distanceFormula(int r1, int c1, int r2, int c2)
    {
        return (int)sqrt(((r1 - r2) * (r1 - r2)) + ((c1 - c2) * (c1 - c2)));
    }
}

#endif
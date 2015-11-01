#ifndef __PATHFIND__
#define __PATHFIND__

#define WIDTH 30
#define HEIGHT 20
#define GRANULARITY 1
#define GRID_WIDTH (WIDTH * GRANULARITY)
#define GRID_HEIGHT (HEIGHT * GRANULARITY)
#define MAX_TIME 5000
#define TIME_INTERVAL 1000
#define D 1
#define D2 sqrt(2)
#define DIAGONALS true

#include <iostream>
#include <fstream>
#include <vector>
#include <utility>
#include <queue>
#include <unordered_set>
#include <set>
#include <unordered_map>
#include <math.h>
#include "Node.cc"

using namespace std;

namespace PathFinding
{
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
        return retVector;
    }

    double octileDistance(int r1, int c1, int r2, int c2)
    {
        double dr = abs(r1 - r2);
        double dc = abs(c1 - c2);
        return D * (dr + dc) + (D2 - 2 * D) * min(dr, dc);
    }

    double manhattanDistance(int r1, int c1, int r2, int c2)
    {
        double dr = abs(r1 - r2);
        double dc = abs(c1 - c2);
        return D * (dr + dc);
    }

    double heuristic(int r1, int c1, int r2, int c2)
    {
        return octileDistance(r1, c1, r2, c2);
    }

    vector<Node> aStar(int sR, int sC, int gR, int gC, int t)
    {
        vector<Node> path;

        // data structures which contain nodes
        Node *nodes[GRID_HEIGHT * GRID_WIDTH];
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { nodes[n] = NULL; }

        unordered_set<Node> closedSet;
        set<Node*, NodePtrComp> openSet;

        // create starting node in the nodes array
        // 1. construct it
        // 2. set g score to 0
        // 3. set f score to the heuristic to the goal
        nodes[sR * GRID_WIDTH + sC] = new Node(sR, sC, heuristic(sR, sC, gR, gC), 0, NULL, t);

        // add node to open set data structures
        openSet.insert(nodes[sR * GRID_WIDTH + sC]);

        // do the A*
        while(openSet.size() != 0)
        {
            // get and remove top node from our open set queue
            Node current = **openSet.begin();
            openSet.erase(openSet.begin()); // this isn't working

            // add it to the closed set
            closedSet.insert(current);

            // iterate through the neighbors in all 8 directions
            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
                    if((i == 0 && j == 0) || (abs(j) == abs(i) && !DIAGONALS))
                    {
                        continue;
                    }
                    
                    // coordinates and time of the neighbor
                    int newRow = current.row + i;
                    int newCol = current.col + j;
                    int newTime = current.time + TIME_INTERVAL;
                    if(newRow < 0 || newRow > GRID_HEIGHT - 1 || newCol < 0 || newCol > GRID_WIDTH - 1)
                    {
                        // out of bounds
                        continue;
                    }

                    if(nodes[newRow * GRID_WIDTH + newCol] == NULL)
                    {
                        nodes[newRow * GRID_WIDTH + newCol] = new Node(newRow, newCol);
                    }

                    // temporary objects for lookup in the reservation table
                    pair<int, Node> tempTN(newTime, Node(newRow, newCol));

                    if(newRow == gR && newCol == gC)
                    {
                        // found the goal
                        nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                        nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                        path = makePath(nodes[newRow * GRID_WIDTH + newCol]);

                        // free the node array
                        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

                        // found the goal
                        return path;
                    }

                    else if(closedSet.find(*nodes[newRow * GRID_WIDTH + newCol]) != closedSet.end())
                    {
                        // if it is in the closed set, we have already visited here
                        continue;
                    }
                    else if (reservationTable.find(tempTN) != reservationTable.end())
                    {
                        // if it already reserved, then we can't go here
                        continue;
                    }

                    double tentativeGScore = 0;
                    if(abs(i) == abs(j))
                    {
                        tentativeGScore = D2;
                    }
                    else
                    {
                        tentativeGScore = D;
                    }
                    tentativeGScore += current.gScore;


                    if(openSet.find(nodes[newRow * GRID_WIDTH + newCol]) == openSet.end())
                    {
                        // we have discovered a new node
                    }
                    else if(tentativeGScore >= nodes[newRow * GRID_WIDTH + newCol]->gScore)
                    {
                        // not a better path
                        continue;
                    }
                    else
                    {
                        openSet.erase(nodes[newRow * GRID_WIDTH + newCol]);
                    }

                    // best we have so far, record it
                    nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                    nodes[newRow * GRID_WIDTH + newCol]->gScore = tentativeGScore;
                    nodes[newRow * GRID_WIDTH + newCol]->fScore = tentativeGScore + heuristic(newRow, newCol, gR, gC);
                    nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                    openSet.insert(nodes[newRow * GRID_WIDTH + newCol]);
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
};

#endif
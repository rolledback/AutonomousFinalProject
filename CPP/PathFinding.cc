#ifndef __PATHFIND__
#define __PATHFIND__

#define WIDTH 30
#define HEIGHT 20
#define GRANULARITY 2
#define GRID_WIDTH (WIDTH * GRANULARITY)
#define GRID_HEIGHT (HEIGHT * GRANULARITY)
#define MAX_TIME 5000
#define TIME_INTERVAL .6
#define D 1
#define D2 sqrt(2)
#define DIAGONALS true

#include <string>
#include <sstream>
#include <vector>
#include <boost/unordered_set.hpp>
#include <boost/unordered_map.hpp>
#include <set>
#include <queue>
#include <math.h>
#include "Node.cc"
#include "audio.h"
#include "naobehavior.h"

using namespace std;

namespace PathFinding
{
    bool grid[GRID_HEIGHT][GRID_WIDTH];

    void resetGrid()
    {
        for(int i = 0; i < HEIGHT * GRANULARITY; i++)
        {
            for(int j = 0; j < WIDTH * GRANULARITY; j++)
            {
                grid[i][j] = false;
            }
        }
    }

    void getParams(double &width, double &height, double &granularity)
    {
        width = GRID_WIDTH;
        height = GRID_HEIGHT;
        granularity = GRANULARITY;
    }

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

    // Defines octile distance
    double octileDistance(int r1, int c1, int r2, int c2)
    {
        double dr = abs(r1 - r2);
        double dc = abs(c1 - c2);
        return D * (dr + dc) + (D2 - 2 * D) * min(dr, dc);
    }

    // Defines manhattan distance
    double manhattanDistance(int r1, int c1, int r2, int c2)
    {
        double dr = abs(r1 - r2);
        double dc = abs(c1 - c2);
        return D * (dr + dc);
    }

    // Defines the heuristic that one may or may not use
    double heuristic(int r1, int c1, int r2, int c2)
    {
        return octileDistance(r1, c1, r2, c2);
    }

    // An agent may or may not be reserved in a column that may or may not use time that may or may not be in a time interval. may or may not
    bool isReserved(vector<Node> &reservationTable, int row, int col, double time, double interval)
    {
        for(uint i = 0; i < reservationTable.size(); i++)
        {
            if(reservationTable[i].row == row && reservationTable[i].col == col)
            {
                // if(abs(reservationTable[i].time - time) < (interval * 2.0))
                {
                    return true;
                }
            }
        }

        return false;
    }

    // Can one even reserve a window?
    bool canReserveWindow(vector<Node> &path, vector<Node> &reservationTable, int currentWindow, int windowSize, double timeInterval)
    {
        for(int i = 0; i < windowSize; i++)
        {
            int index = path.size() - 1 - i - (currentWindow * windowSize);
            if(isReserved(reservationTable, path[index].row, path[index].col, path[index].time, timeInterval))
            {
                return false;
            }
        }
        return true;
    }

    vector<Node> bfs(int sR, int sC, int gR, int gC, double t, vector<Node> &path, vector<Node> &reservationTable, double timeInterval)
    {
        Node *nodes[GRID_HEIGHT * GRID_WIDTH];
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { nodes[n] = NULL; }

        resetGrid();
        queue<Node*> stack;

        nodes[sR * GRID_WIDTH + sC] = new Node(sR, sC, heuristic(sR, sC, gR, gC), 0, NULL, t);

        stack.push(nodes[sR * GRID_WIDTH + sC]);
        grid[sR][sC] = true;

        while(stack.size() != 0)
        {
            Node current = *stack.front();
            stack.pop();

            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
                    if(abs(i) == abs(j))
                    {
                        continue;
                    }

                    int newRow = current.row + i;
                    int newCol = current.col + j;
                    double newTime = current.time + timeInterval;

                    if(newRow < 0 || newRow > GRID_HEIGHT - 1 || newCol < 0 || newCol > GRID_WIDTH - 1)
                    {
                        // out of bounds
                        continue;
                    }

                    if(nodes[newRow * GRID_WIDTH + newCol] == NULL)
                    {
                        nodes[newRow * GRID_WIDTH + newCol] = new Node(newRow, newCol);
                    }

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

                    else if(grid[newRow][newCol])
                    {
                        // if it is in the closed set, we have already visited here
                        continue;
                    }
                    else if(isReserved(reservationTable, newRow, newCol, newTime, timeInterval))
                    {
                        // if it is reserved
                        continue;
                    }

                    // put on the stack
                    nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                    nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                    stack.push(nodes[newRow * GRID_WIDTH + newCol]);
                    grid[newRow][newCol] = true;
                }
            }
        }

        // free the node array
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

        // return the path, it will have size 0
        return path;
    }

    Node* lowestFScore(Node **openSet, int &index, int &currentSize)
    {
        int lowestFScore = 99999;
        int lowestIndex = -1;

        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++)
        {
            if(openSet[n] != NULL)
            {
                currentSize++;
                if(openSet[n]->fScore < lowestFScore)
                {
                    lowestFScore = openSet[n]->fScore;
                    lowestIndex = n;
                }
            }
        }

        index = lowestIndex;
        return openSet[lowestIndex];
    }

    bool sizeNotZero(Node **openSet)
    {
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++)
        {
            if(openSet[n] != NULL)
            {
                return true;
            }
        }
        return false;
    }

    // Who is aStar? You're aStar! :D
    vector<Node> aStar(int sR, int sC, int gR, int gC, double t, vector<Node> &path, vector<Node> &reservationTable, double timeInterval)
    {
        cout << "\nA*" << endl;
        cout << "Starting row: " << sR << " starting column: " << sC << endl;
        cout << "Goal row: " << gR << " goal column: " << gC << endl;
        cout << "Starting time: " << t << " time interval: " << timeInterval << endl;
        cout << endl;

        // data structures which contain nodes
        resetGrid();
        Node *openSet[GRID_HEIGHT * GRID_WIDTH];
        Node *nodes[GRID_HEIGHT * GRID_WIDTH];
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { nodes[n] = NULL; openSet[n] = NULL; }

        // create starting node in the nodes array
        // 1. construct it
        // 2. set g score to 0
        // 3. set f score to the heuristic to the goal
        nodes[sR * GRID_WIDTH + sC] = new Node(sR, sC, heuristic(sR, sC, gR, gC), 0, NULL, t);

        // add node to open set data structures
        openSet[sR * GRID_WIDTH + sC] = nodes[sR * GRID_WIDTH + sC];

        // do the A*
        while(sizeNotZero(openSet))
        {
            cout << "\nIterate the open set" << endl;
                                              
            int index = 0, currentSize = 0;
            Node current = *lowestFScore(openSet, index, currentSize);
            openSet[index] = NULL;
            cout << "Open set size: " << currentSize << endl;
            cout << "Current node\n" << "Row: " << current.row << " Col: " << current.col << endl;

            // add it to the closed set
            grid[current.row][current.col] = true;

            cout << "That is now true in the closed set." << endl;

            // iterate through the neighbors in all 8 directions
            for(int i = -1; i < 2; i++)
            {
                for(int j = -1; j < 2; j++)
                {
                    cout << endl;
                    // coordinates and time of the neighbor
                    int newRow = current.row + i;
                    int newCol = current.col + j;
                    double newTime = current.time + timeInterval;

                    if(i == 0 && j == 0)
                    {
                        cout << "We aren't evaluating being in the same spot" << endl;
                        continue;
                    }
                    else if(abs(j) == abs(i) && !DIAGONALS)
                    {
                        cout << "We aren't evaluating diagonals" << endl;
                        continue;
                    }
                    else if(newRow < 0 || newRow > GRID_HEIGHT - 1 || newCol < 0 || newCol > GRID_WIDTH - 1)
                    {
                        // out of bounds
                        cout << "This would be out of bounds" << endl;
                        continue;
                    }
                    else if(grid[newRow][newCol])
                    {
                        // if it is in the closed set, we have already visited here
                        cout << "We have already visited this node" << endl;
                        continue;
                    }
                    else if(isReserved(reservationTable, newRow, newCol, newTime, timeInterval))
                    {
                        // if it is reserved
                        cout << "It is reserved at the current time in the reservation table." << endl;
                        continue;
                    }

                    cout << "Passed all the checks. Evaluating neighbor, row: " << newRow << " col: " << newCol << " time: " << newTime << endl;

                    if(nodes[newRow * GRID_WIDTH + newCol] == NULL)
                    {
                        cout << "Construct the node object as the pointer to it is null" << endl;
                        nodes[newRow * GRID_WIDTH + newCol] = new Node(newRow, newCol);
                    }

                    if(newRow == gR && newCol == gC)
                    {
                        // found the goal
                        cout << "Found the goal" << endl;
                        nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                        nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                        path = makePath(nodes[newRow * GRID_WIDTH + newCol]);

                        // free the node array
                        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

                        // found the goal
                        return path;
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


                    if(openSet[newRow * GRID_WIDTH + newCol] == NULL)
                    {
                        // we have discovered a new node
                        cout << "Not in the open set, congrats you found a new node!" << endl;
                    }
                    else if(tentativeGScore >= nodes[newRow * GRID_WIDTH + newCol]->gScore)
                    {
                        // not a better path
                        cout << "Not a new node, and the score going this way to it is worse than the original score you found." << endl;
                        continue;
                    }
                    else
                    {
                        cout << "Not a new node, and the score going this way to it is better than the original score you found." << endl;
                        openSet[newRow * GRID_WIDTH + newCol] = NULL;
                    }

                    // best we have so far, record it
                    cout << "Record this into the open set" << endl;
                    nodes[newRow * GRID_WIDTH + newCol]->prev = nodes[(current.row) * GRID_WIDTH + current.col];
                    nodes[newRow * GRID_WIDTH + newCol]->gScore = tentativeGScore;
                    nodes[newRow * GRID_WIDTH + newCol]->fScore = tentativeGScore + heuristic(newRow, newCol, gR, gC);
                    nodes[newRow * GRID_WIDTH + newCol]->time = newTime;
                    openSet[newRow * GRID_WIDTH + newCol] = nodes[newRow * GRID_WIDTH + newCol];
                }

            }
        }

        // free the node array
        for(int n = 0; n < GRID_HEIGHT * GRID_WIDTH; n++) { if(nodes[n] != NULL) free(nodes[n]); }

        // return the path, it will have size 0
        return path;
    }

    void convertToGridCoordinates(float x, float y, int &r, int &c)
    {
        float x2 = (float)WIDTH / 2.0 * -1;
        for(c = 0; c < GRID_WIDTH; c++)
        {
            if(x2 + ((float)WIDTH / (float)GRID_WIDTH * (c + 1)) > x)
            {
                break;
            }
        }

        float y2 = (float)HEIGHT / 2.0;
        for(r = 0; r < GRID_HEIGHT; r++)
        {
            if(y2 - ((float)HEIGHT / (float)GRID_HEIGHT * (r + 1)) < y)
            {
                break;
            }
        }
    }

    void convertToRoboCoordinates(float &x, float &y, int r, int c)
    {
        x = (float)WIDTH / 2.0 * -1;
        x = x + ((float)WIDTH / (float)GRID_WIDTH * c);
        x = x + ((float)WIDTH / (float)GRID_WIDTH / 2.0);

        y = HEIGHT / 2.0;
        y = y - ((float)HEIGHT / (float)GRID_HEIGHT * r);
        y = y - ((float)HEIGHT / (float)GRID_HEIGHT / 2.0);
    }

    void createReservationMessage(vector<Node> path, uint begin, uint size, int uNum, double timeInterval, string &message)
    {
        vector<int> bits;
        ostringstream oss;

        vector<int> uNumBits = intToBits(uNum, 4);
        bits.insert(bits.end(), uNumBits.begin(), uNumBits.end());

        int startTime = (int)(path[begin * size].time * 100);
        vector<int> startTimeBits = intToBits(startTime, 16);
        bits.insert(bits.end(), startTimeBits.begin(), startTimeBits.end());

        int intervalTime = (int)(timeInterval * 100);
        vector<int> intervalTimeBits = intToBits(intervalTime, 16);
        bits.insert(bits.end(), intervalTimeBits.begin(), intervalTimeBits.end());

        for(uint i = begin * size; i < (begin * size) + size && i < path.size(); i++)
        {
            vector<int> rowBits = intToBits(path[path.size() - 1 - i].row, 8);
            bits.insert(bits.end(), rowBits.begin(), rowBits.end());
            
            vector<int> colBits = intToBits(path[path.size() - 1 - i].col, 8);
            bits.insert(bits.end(), colBits.begin(), colBits.end());
        }

        string temp;
        bitsToString(bits, temp);
        oss << temp;
        message = oss.str();
    }

    void processReservationMessage(string &heardMessage, int &uNum, double &startTime, double &intervalTime, vector<Node> &path)
    {
        string strippedMessage;
        for(int i = heardMessage.length() - 2; i > -1 && heardMessage[i] != ' '; i--)
        {
            strippedMessage = heardMessage[i] + strippedMessage;
        }

        vector<int> bits;
        stringToBits(strippedMessage, bits);

        uNum = bitsToInt(bits, 0, 3);

        startTime = ((double)bitsToInt(bits, 4, 19)) / 100.0;
        intervalTime = ((double)bitsToInt(bits, 20, 35)) / 100.0;

        uint bitIndex = 36;
        int segSize = 7;
        int i = 1;
        
        if(strippedMessage.length() == 0)
        {
            return;
        }

        while(bitIndex < bits.size() - segSize)
        {
            int row = bitsToInt(bits, bitIndex, bitIndex + segSize);
            bitIndex += segSize + 1;

            int col = bitsToInt(bits, bitIndex, bitIndex + segSize);
            bitIndex += segSize + 1;

            Node temp = Node(row, col);
            temp.time = startTime + (intervalTime * i++);

            temp.uNum = uNum;

            path.push_back(temp);
        }
    }

    void addToReservationTable(vector<Node> &reservationTable, vector<Node> &toAdd)
    {
        reservationTable.insert(reservationTable.end(), toAdd.begin(), toAdd.end());
    }

    void removePlayersReservations(vector<Node> &reservationTable, int uNum)
    {
        for(uint i = 0; i < reservationTable.size();)
        {
            if(reservationTable[i].uNum == uNum)
            {
                reservationTable[i] = reservationTable.back();
                reservationTable.pop_back();
            }
            else
                ++i;
            }
        }
};

#endif
#include "PathFinding.cc"
#include <iostream>
#include <fstream>

using namespace PathFinding;
using namespace std;

int main()
{
    resetGrid();

    int path[20][30];
    for(int r = 0; r < 20; r++)
    {
    	for(int c = 0; c < 30; c++)
    	{
    		path[r][c] = -1;
    	}
    }

    vector<Node> godHelpUs = aStar(0, 0, 15, 15, 0);
    for(int i = 0; i < godHelpUs.size(); i++)
    {
        path[godHelpUs[i].row][godHelpUs[i].col] = i;
    }

    for(int r = 0; r < 20; r++)
    {
    	for(int c = 0; c < 30; c++)
    	{
            if(path[r][c] == -1)
                cout << "-- ";
            else if(path[r][c] < 10)
 			    cout << "0" << path[r][c] << " ";
            else
                cout << path[r][c] << " ";
    	}
    	cout << endl;
    }

    return 0;
}
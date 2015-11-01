#include "PathFinding.h"

using namespace PathFinding;

int main()
{
    resetGrid();

    vector<Node> godHelpUs = aStar(0, 0, 10, 10, 0);

    return 0;
}
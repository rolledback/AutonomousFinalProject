import java.util.Scanner;
import java.util.ArrayList;

public class FAR
{
    static MapNode[][] map;
    static int numRows, numCols;

    public static void main(String[] args)
    {
        Scanner input = new Scanner(System.in);
        
        numRows = Integer.parseInt(input.nextLine());
        numCols = Integer.parseInt(input.nextLine());
        map = new MapNode[numRows + 2][numCols + 2];

        int currRow = 0;
        while(input.hasNext())
        {
            String line = input.nextLine();
            String[] spots = line.split(",");

            for(int i = 0; i < spots.length; i++)
            {
                map[currRow + 1][i + 1] = new MapNode(spots[i].equals("0"), currRow, i);
            }

            currRow++;
        }

        addBorder();
        addOneWayStreets();
        printMap();

        ensureConnectivity();
    }

    public static void ensureConnectivity()
    {
        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                MapNode curr = map[row][col];

                int tunnelCase = isTunnel(row, col);
                {
                    if(tunnelCase == 1)
                    {
                        curr.addOutgoingEdge(curr.incomingEdges.get(0));
                        curr.incomingEdges.get(0).addIncomingEdge(curr);
                        
                        // curr.addIncomingEdge(curr.outgoingEdges.get(0));
                        // curr.outgoingEdges.get(0).addOutgoingEdge(curr);
                    }
                    else if(tunnelCase == 2)
                    {
                        curr.addIncomingEdge(curr.outgoingEdges.get(0));
                        curr.outgoingEdges.get(0).addOutgoingEdge(curr);
                    }
                    else if(tunnelCase == 3)
                    {
                        curr.addOutgoingEdge(curr.incomingEdges.get(0));
                        curr.incomingEdges.get(0).addIncomingEdge(curr);
                    }
                    
                    if(tunnelCase > 0)
                    {
                        System.out.println(row + " " + col + " " + tunnelCase);
                        printMap();
                    }
                }
            }
        }
    }

    public static void addOneWayStreets()
    {
        MapNode curr = null;
        MapNode toLeft = null, toRight = null;
        MapNode above = null, below = null;

        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                curr = map[row][col];

                if(curr.canWalk == false)
                {
                    continue;
                }

                if(col != 1)
                {
                    toLeft = map[row][col - 1];
                }
                if(col != numCols)
                {
                    toRight = map[row][col + 1];                    
                }

                if(row != 1)
                {
                    above = map[row - 1][col];
                }
                if(row != numRows)
                {
                    below = map[row + 1][col];
                }

                if(row % 2 == 0)
                {
                    if(toLeft != null && toLeft.canWalk)
                    {
                        toLeft.addOutgoingEdge(curr);
                        curr.addIncomingEdge(toLeft);
                    }
                }
                else
                {
                    if(toRight != null && toRight.canWalk)
                    {
                        curr.addIncomingEdge(toRight);
                        toRight.addOutgoingEdge(curr);
                    }
                }

                if(col % 2 == 0)
                {
                    if(above != null && above.canWalk)
                    {
                        above.addOutgoingEdge(curr);
                        curr.addIncomingEdge(above);
                    }
                }
                else
                {
                    if(below != null && below.canWalk)
                    {
                        curr.addIncomingEdge(below);
                        below.addOutgoingEdge(curr);
                    }
                }

                curr = null;
                toLeft = null;
                toRight = null;
                above = null;
                below = null;
            }
        }
    }

    public static void addBorder() {
        for(int i = 0; i < numRows + 2; i++)
        {
            if(i == 0 || i == numRows + 2 - 1)
            {
                for(int j = 0; j < numCols + 2; j++)
                {
                     map[i][j] = new MapNode(false, -1, -1);
                }
            }
            else
            {
                map[i][0] = new MapNode(false, -1, -1);
                map[i][numCols + 2 - 1] = new MapNode(false, -1, -1);
            }
        }
    }

    public static int isTunnel(int x, int y)
    {
        MapNode curr = map[x][y];
        if(curr.outgoingEdges.size() == 1 && curr.incomingEdges.size() == 1)
        {
            return 1;
        }
        if(curr.outgoingEdges.size() == 1 && curr.incomingEdges.size() == 0)
        {
            return 2;
        }
        if(curr.outgoingEdges.size() == 0 && curr.incomingEdges.size() == 1)
        {
            return 3;
        }
        else
        {
            return 0;
        }
    }

    public static boolean isCorner(int x, int y)
    {
        for(int i = -1; i < 2; i += 2)
        {
            for(int j = -1; j < 2; j++)
            {
                if(map[x + i][y].canWalk == false && map[x][y + j].canWalk == false)
                {
                    return true;
                }
            }
        }
        return false;
    }

    public static boolean isSink(int x, int y)
    {
        if(isCorner(x, y))
        {
            return map[x][y].outgoingEdges.size() == 0 && map[x][y].incomingEdges.size() == 2;
        }
        return false;
    }

    public static boolean isSource(int x, int y)
    {
        if(isCorner(x, y))
        {
            return map[x][y].outgoingEdges.size() == 2 && map[x][y].incomingEdges.size() == 0;
        }
        return false;
    }

    public static void printMap()
    {
        for(int row = 0; row < numRows + 2; row++)
        {
            for(int col = 0; col < numCols + 2; col++)
            {
                System.out.print(map[row][col]);
            }
            System.out.println();
        }
    }
}

class MapNode
{
    public boolean canWalk;
    public ArrayList<MapNode> outgoingEdges;
    public ArrayList<MapNode> incomingEdges;
    public int r, c;

    public MapNode(boolean cW, int r, int c)
    {
        canWalk = cW;
        outgoingEdges = new ArrayList<MapNode>();
        incomingEdges = new ArrayList<MapNode>();
        this.r = r;
        this.c = c;
    }

    public void addOutgoingEdge(MapNode n)
    {
        outgoingEdges.add(n);
    }

    public void addIncomingEdge(MapNode n)
    {
        incomingEdges.add(n);
    }

    public String toString()
    {
        String ret = "------";

        if(canWalk)
        {
            ret = "T,";
        }
        else
        {
            return ret;
        }

        ret += incomingEdges.size() + "," + outgoingEdges.size() + " ";
        return ret;
    }
    
}
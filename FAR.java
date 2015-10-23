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
        ensureConnectivity();
        printMap(); 
    }

    public static void ensureConnectivity()
    {
        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                if(isTunnel(row, col))
                {
                    fixTunnel(row, col);
                }
                if(isSink(row, col) || isSource(row, col))
                {
                    fixSinkOrSource(row, col);
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
                        toLeft.changeEdge(DIRECTION.RIGHT, EDGE_TYPE.OUT);
                        curr.changeEdge(DIRECTION.LEFT, EDGE_TYPE.IN);
                    }
                }
                else
                {
                    if(toRight != null && toRight.canWalk)
                    {
                        curr.changeEdge(DIRECTION.RIGHT, EDGE_TYPE.IN);
                        toRight.changeEdge(DIRECTION.LEFT, EDGE_TYPE.OUT);
                    }
                }

                if(col % 2 == 0)
                {
                    if(above != null && above.canWalk)
                    {
                        above.changeEdge(DIRECTION.DOWN, EDGE_TYPE.OUT);
                        curr.changeEdge(DIRECTION.UP, EDGE_TYPE.IN);
                    }
                }
                else
                {
                    if(below != null && below.canWalk)
                    {
                        below.changeEdge(DIRECTION.UP, EDGE_TYPE.OUT);
                        curr.changeEdge(DIRECTION.DOWN, EDGE_TYPE.IN);
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

    public static boolean isTunnel(int row, int col)
    {
        MapNode curr = map[row][col];
        if(curr.outgoingEdges == 1 && curr.incomingEdges == 1)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    public static void fixTunnel(int row, int col)
    {
        MapNode curr = map[row][col];
        MapNode other;
        int numChanges = 0;

        if(curr.edges[DIRECTION.UP.getValue()] == EDGE_TYPE.IN || curr.edges[DIRECTION.UP.getValue()] == EDGE_TYPE.OUT)
        {
            curr.edges[DIRECTION.UP.getValue()] = EDGE_TYPE.BOTH;
            map[row - 1][col].edges[DIRECTION.DOWN.getValue()] = EDGE_TYPE.BOTH;
            numChanges++;
        }

        if(curr.edges[DIRECTION.DOWN.getValue()] == EDGE_TYPE.IN || curr.edges[DIRECTION.DOWN.getValue()] == EDGE_TYPE.OUT)
        {
            curr.edges[DIRECTION.DOWN.getValue()] = EDGE_TYPE.BOTH;
            map[row + 1][col].edges[DIRECTION.UP.getValue()] = EDGE_TYPE.BOTH;
            numChanges++;
        }

        if(curr.edges[DIRECTION.LEFT.getValue()] == EDGE_TYPE.IN || curr.edges[DIRECTION.LEFT.getValue()] == EDGE_TYPE.OUT)
        {
            curr.edges[DIRECTION.LEFT.getValue()] = EDGE_TYPE.BOTH;
            map[row][col - 1].edges[DIRECTION.RIGHT.getValue()] = EDGE_TYPE.BOTH;
            numChanges++;
        }

        if(curr.edges[DIRECTION.RIGHT.getValue()] == EDGE_TYPE.IN || curr.edges[DIRECTION.RIGHT.getValue()] == EDGE_TYPE.OUT)
        {
            curr.edges[DIRECTION.RIGHT.getValue()] = EDGE_TYPE.BOTH;
            map[row][col + 1].edges[DIRECTION.LEFT.getValue()] = EDGE_TYPE.BOTH;
            numChanges++;
        }

        if(numChanges != 2)
        {
            System.out.println("FUCK");
            System.exit(0);
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
            return map[x][y].outgoingEdges == 0 && map[x][y].incomingEdges == 2;
        }
        return false;
    }

    public static boolean isSource(int x, int y)
    {
        if(isCorner(x, y))
        {
            return map[x][y].outgoingEdges == 2 && map[x][y].incomingEdges == 0;
        }
        return false;
    }

    public static boolean fixSinkOrSource(int row, int col)
    {
        for(int dR = -1; dR < 2; dR++)
        {
            for(int dC = -1; dC < 2; dC++)
            {
                if(dR == 0 && dC == 0)
                {
                    continue;
                }

                if(map[row + dR][col + dC].canWalk == false)
                {
                    continue;
                }

                if(isSink(row + dR, col + dC) || isSource(row + dR, col + dC))
                {
                    continue;
                }
                else
                {
                    if(validMove(row, col, row + dR, col + dC))
                    {

                        return true;
                    }
                }
            }
        }

        return false;
    }

    public static boolean validMove(int row1, int col1, int row2, int col2)
    {
        if(!map[row1][col1].canWalk || !map[row2][col2].canWalk)
        {
            return false;
        }

        if(col1 > col2)
        {
            if(!map[row1][col1 - 1].canWalk || !map[row2][col2 + 1].canWalk)
            {
                return false;
            }
        }
        else if(col1 < col2)
        {
            if(!map[row1][col1 + 1].canWalk || !map[row2][col2 - 1].canWalk)
            {
                return false;
            }
        }

        return true;
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

enum DIRECTION
{
    UP(0),
    DOWN(1),
    LEFT(2),
    RIGHT(3),
    UP_LEFT(4),
    UP_RIGHT(5),
    DOWN_LEFT(6),
    DOWN_RIGHT(7);

    private final int value;
    public static final int numDirections = 8;
    private DIRECTION(int value)
    {
        this.value = value;
    }

    public int getValue()
    {
        return value;
    }
}

enum EDGE_TYPE
{
    NONE(0, "N"),
    IN(1, "I"),
    OUT(2, "O"),
    BOTH(3, "B");

    private final int value;
    private final String sValue;
    private EDGE_TYPE(int value, String sValue)
    {
        this.value = value;
        this.sValue = sValue;
    }

    public int getValue()
    {
        return value;
    }

    public String getsValue()
    {
        return sValue;
    }
}

class MapNode
{
    public boolean canWalk;
    public EDGE_TYPE[] edges;
    public int r, c, incomingEdges, outgoingEdges;

    public MapNode(boolean cW, int r, int c)
    {
        canWalk = cW;

        edges = new EDGE_TYPE[DIRECTION.numDirections];
        changeAllEdges(EDGE_TYPE.NONE);

        this.r = r;
        this.c = c;
    }

    public void changeEdge(DIRECTION which, EDGE_TYPE type)
    {
        edges[which.getValue()] = type;
        countEdges();
    }

    public void changeAllEdges(EDGE_TYPE type)
    {
        for(int i = 0; i < DIRECTION.numDirections; i++)
        {
            edges[i] = type;
        }
        countEdges();
    }

    public void changeEdgeTo(int oRow, int oCol, EDGE_TYPE type)
    {
        if(r == oRow)
        {
            if(c < oCol)
            {
                edges[DIRECTION.RIGHT.getValue()] = type;
            }
            else if(c > oCol)
            {
                edges[DIRECTION.LEFT.getValue()] = type;
            }
        }
        if(c == oCol)
        {
            if(r < oRow)
            {
                edges[DIRECTION.DOWN.getValue()] = type;
            }
            else if(r > oRow)
            {
                edges[DIRECTION.UP.getValue()] = type;
            }
        }

        if(r > oRow)
        {
            if(c < oCol)
            {
                edges[DIRECTION.UP_RIGHT.getValue()] = type;
            }
            else if(c > oCol)
            {
                edges[DIRECTION.UP_LEFT.getValue()] = type;
            }
        }
        if(r < oRow)
        {
            if(c < oCol)
            {
                edges[DIRECTION.DOWN_RIGHT.getValue()] = type;
            }
            else if(c > oCol)
            {
                edges[DIRECTION.DOWN_LEFT.getValue()] = type;
            }
        }
    }

    private void countEdges()
    {
        incomingEdges = 0;
        outgoingEdges = 0;

        for(int i = 0; i < DIRECTION.numDirections; i++)
        {
            if(edges[i] == EDGE_TYPE.BOTH)
            {
                incomingEdges++;
                outgoingEdges++;
            }
            else if(edges[i] == EDGE_TYPE.IN)
            {
                incomingEdges++;
            }
            else if(edges[i] == EDGE_TYPE.OUT)
            {
                outgoingEdges++;
            }
        }
    }

    public String toString()
    {
        String ret = "--------- ";

        if(canWalk)
        {
            ret = "T,";
        }
        else
        {
            return ret;
        }

        for(int i = 0; i < 4; i++)
        {
            ret += edges[i].getsValue();
            if(i != 3)
            {
                ret += ",";
            }
        }
        ret += " ";

        return ret;
    }    
}
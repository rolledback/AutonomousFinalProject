import java.util.Scanner;
import java.util.ArrayList;

public class FAR
{
    static MapNode[][] map;
    static int numRows, numCols;
    static int noWalk;

    public static void main(String[] args)
    {
        Scanner input = new Scanner(System.in);
        
        numRows = Integer.parseInt(input.nextLine());
        numCols = Integer.parseInt(input.nextLine());
        map = new MapNode[numRows + 2][numCols + 2];
        noWalk = 0;

        int currRow = 0;
        while(input.hasNext())
        {
            String line = input.nextLine();
            String[] spots = line.split(",");

            for(int i = 0; i < spots.length; i++)
            {
                map[currRow + 1][i + 1] = new MapNode(spots[i].equals("0"), currRow + 1, i + 1);
                if(!spots[i].equals("0"))
                {
                    noWalk++;
                }
            }

            currRow++;
        }

        addBorder();
        addOneWayStreets();
        ensureConnectivity();
        printMap();
        verifyMap();
    }

    public static boolean verifyMap()
    {
        System.out.println("Coorect Answer ==> " + (numRows * numCols - noWalk));
        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                System.out.print(row + " " + col + " ==> ");
                if(map[row][col].canWalk)
                {
                    boolean[][] seen = new boolean[numRows + 2][numCols + 2];
                    seen[row][col] = true;
                    System.out.println(DFS(row, col, seen));

                    for(int r = 1; r <= numRows; r++)
                    {
                        for(int c = 1; c <= numCols; c++)
                        {
                            if(seen[r][c] == false && map[r][c].canWalk)
                            {
                                System.out.println(r + " " + c);
                            }
                        }
                    }
                }
                else
                {
                    System.out.println("Can't walk here");
                }
            }
        }

        return true;
    }

    public static int DFS(int row, int col, boolean[][] seen)
    {
        if(!map[row][col].canWalk)
        {
            return 0;
        }
        else
        {
            int count = 1;
            for(int i = 0; i < DIRECTION.numDirections; i++)
            {
                if(map[row][col].edges[i] == EDGE_TYPE.OUT || map[row][col].edges[i] == EDGE_TYPE.BOTH)
                {
                    int[] vector = DIRECTION.directionToVector[i];
                    if(!seen[row + vector[0]][col + vector[1]])
                    {
                        seen[row + vector[0]][col + vector[1]] = true;
                        count += DFS(row + vector[0], col + vector[1], seen);
                    }
                }
            }

            return count;
        }
    }

    public static void ensureConnectivity()
    {
        ArrayList<Object[]> tunnels  = new ArrayList<Object[]>();
        ArrayList<Object[]> sinksAndSources  = new ArrayList<Object[]>();

        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                if(isTunnel(row, col))
                {
                    tunnels.add(tunnelFix(row, col));
                }
                else if(isSink(row, col) || isSource(row, col))
                {
                    sinksAndSources.add(sinkOrSourceFix(row, col));
                }
            }
        }

        for(int i = 0; i < tunnels.size(); i++)
        {
            applyTunnelFix(tunnels.get(i));
        }
        System.out.println();
        for(int i = 0; i < sinksAndSources.size(); i++)
        {
            applySinkOrSourceFix(sinksAndSources.get(i));
        }

        for(int row = 1; row <= numRows; row++)
        {
            for(int col = 1; col <= numCols; col++)
            {
                checkAndFixIslands(row, col);
            }
        }
    }

    public static void checkAndFixIslands(int row, int col)
    {
        MapNode curr = map[row][col];
        if(curr.incomingEdges + curr.outgoingEdges == 1)
        {
            for(int i = DIRECTION.minCardinal; i < DIRECTION.maxCardinal; i++)
            {
                if(curr.edges[i] != EDGE_TYPE.NONE)
                {
                    curr.changeEdge(DIRECTION.directionFromInt(i), EDGE_TYPE.BOTH);

                    int[] vector = DIRECTION.directionToVector[i];
                    map[row + vector[0]][col + vector[1]].changeEdgeTo(row, col, EDGE_TYPE.BOTH);
                }
            }
        }
    }

    public static Object[] tunnelFix(int row, int col)
    {
        MapNode curr = map[row][col];
        Object[] tuple = new Object[4];
        tuple[0] = new Integer(row);
        tuple[1] = new Integer(col);

        int t = 2;
        for(int i = DIRECTION.minCardinal; i < DIRECTION.maxCardinal; i++)
        {
            if(curr.edges[i] == EDGE_TYPE.OUT || curr.edges[i] == EDGE_TYPE.IN)
            {
                tuple[t] = DIRECTION.directionFromInt(i);
                t++;
            }
        }

        if(t != 4)
        {
            System.out.println("Error creating tunnel fix for: " + row + " " + col);
            return null;
        }

        return tuple;
    }

    public static void applyTunnelFix(Object[] fix)
    {
        for(int i = 0; i < 4; i++)
        {
            System.out.print(fix[i] + " ");
        }
        System.out.println();

        int row = (Integer)fix[0];
        int col = (Integer)fix[1];
        MapNode curr = map[row][col];

        // first edge
        DIRECTION directionOne = (DIRECTION)fix[2];
        int[] vectorOne = DIRECTION.directionToVector[directionOne.getValue()];
        curr.edges[directionOne.getValue()] = EDGE_TYPE.BOTH;
        map[row + vectorOne[0]][col + vectorOne[1]].changeEdgeTo(row, col, EDGE_TYPE.BOTH);

        // second edge
        DIRECTION directionTwo = (DIRECTION)fix[3];
        int[] vectorTwo = DIRECTION.directionToVector[directionTwo.getValue()];
        curr.edges[directionTwo.getValue()] = EDGE_TYPE.BOTH;
        map[row + vectorTwo[0]][col + vectorTwo[1]].changeEdgeTo(row, col, EDGE_TYPE.BOTH);
    }


    public static Object[] sinkOrSourceFix(int row, int col)
    {
        MapNode curr = map[row][col];
        Object[] tuple = new Object[4];
        tuple[0] = new Integer(row);
        tuple[1] = new Integer(col);

        for(int dR = -1; dR < 2; dR += 2)
        {
            for(int dC = -1; dC < 2; dC += 2)
            {
                if(map[row + dR][col + dC].canWalk == false)
                {
                    continue;
                }
                else if(isSink(row + dR, col + dC) || isSource(row + dR, col + dC))
                {
                    continue;
                }
                else if(checkDiagonalMove(row, col, row + dR, col + dC))
                {
                    tuple[2] = DIRECTION.directionFromVector(dR, dC);
                    tuple[3] = isSource(row, col) ? EDGE_TYPE.IN : EDGE_TYPE.OUT;
                    return tuple;
                }
            }
        }

        for(int dR = -1; dR < 2; dR++)
        {
            for(int dC = -1; dC < 2; dC++)
            {
                if(dR == dC)
                {
                    continue;
                }
                else if(map[row + dR][col + dC].canWalk == false)
                {
                    continue;
                }
                else if(isSink(row + dR, col + dC) || isSource(row + dR, col + dC))
                {
                    continue;
                }
                else
                {
                    tuple[2] = DIRECTION.directionFromVector(dR, dC);
                    tuple[3] = isSource(row, col) ? EDGE_TYPE.IN : EDGE_TYPE.OUT;
                    return tuple;
                }
            }
        }

        return null;
    }

    public static void applySinkOrSourceFix(Object[] fix)
    {
        for(int i = 0; i < 4; i++)
        {
            System.out.print(fix[i] + " ");
        }
        System.out.println();

        int row = (Integer)fix[0];
        int col = (Integer)fix[1];
        MapNode curr = map[row][col];

        DIRECTION dir = (DIRECTION)fix[2];
        EDGE_TYPE type = (EDGE_TYPE)fix[3];
        curr.changeEdge(dir, type);

        int[] vector = DIRECTION.directionToVector[dir.getValue()];
        map[row + vector[0]][col + vector[1]].changeEdgeTo(row, col, (type == EDGE_TYPE.IN) ? EDGE_TYPE.OUT : EDGE_TYPE.IN);
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

    public static boolean checkDiagonalMove(int row1, int col1, int row2, int col2)
    {
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

    public static int[][] directionToVector = {
        {-1, 0},
        {1, 0},
        {0, -1},
        {0, 1},
        {-1, -1},
        {-1, 1},
        {1, -1},
        {1, 1}
    };

    private final int value;
    public static final int numDirections = 8;

    public static final int minCardinal = 0;
    public static final int maxCardinal = 4;

    private DIRECTION(int value)
    {
        this.value = value;
    }

    public static DIRECTION directionFromInt(int d)
    {
        switch(d)
        {
            case 0:
                return UP;
            case 1:
                return DOWN;
            case 2:
                return LEFT;
            case 3:
                return RIGHT;
            case 4:
                return UP_LEFT;
            case 5:
                return UP_RIGHT;
            case 6:
                return DOWN_LEFT;
            case 7:
                return DOWN_RIGHT;
        }
        return null;
    }

    public static DIRECTION directionFromVector(int x, int y)
    {
        for(int i = 0; i < directionToVector.length; i++)
        {
            if(x == directionToVector[i][0] && y == directionToVector[i][1])
            {
                return directionFromInt(i);
            }
        }
        return null;
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

        countEdges();
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
        String ret = "----------------- ";

        if(canWalk)
        {
            ret = "T,";
        }
        else
        {
            return ret;
        }

        for(int i = 0; i < DIRECTION.numDirections; i++)
        {
            ret += edges[i].getsValue();
            if(i != DIRECTION.numDirections - 1)
            {
                ret += ",";
            }
        }
        ret += " ";

        return ret;
    }    
}
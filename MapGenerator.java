import java.util.Random;
import java.util.Arrays;

public class MapGenerator
{
    public static void main(String args[])
    {
        int width = Integer.parseInt(args[0]);
        int height = Integer.parseInt(args[1]);
        double p = Double.parseDouble(args[2]);

        boolean[][] map = generateMap(width, height, p);

        while(true)
        {
            if(isConnected(map))
            {
                break;
            }
            else
            {
                // p += .01;
                map = generateMap(width, height, p);
            }
        }

        System.out.println(height);
        System.out.println(width);
        printMap(map);
    }

    public static void printMap(boolean[][] map)
    {
        for(int r = 0; r < map.length; r++)
        {
            for(int c = 0; c < map[r].length; c++)
            {
                System.out.print((map[r][c] ? "0" : "1"));
                if(c != map[r].length - 1)
                {
                    System.out.print(",");
                }
            }
            System.out.println();
        }
    }

    public static boolean isConnected(boolean[][] map)
    {
        int numOpenSpots = 0;
        int firstR = -1;
        int firstC = -1;
        for(int r = 0; r < map.length; r++)
        {
            for(int c = 0; c < map[r].length; c++)
            {
                if(map[r][c])
                {
                    if(firstR == -1 && firstC == -1)
                    {
                        firstR = r;
                        firstC = c;
                    }
                    numOpenSpots++;
                }
            }
        }

        if(firstR == -1 || firstC == -1)
        {
            return false;
        }

        boolean[][] seen = new boolean[map.length][map[0].length];
        seen[firstR][firstC] = true;
        int found = DFS(map, firstR, firstC, seen);

        return found == numOpenSpots;
    }

    public static int DFS(boolean[][] map, int r, int c, boolean seen[][])
    {
        int count = 1;
        for(int dR = -1; dR < 2; dR += 1)
        {
            for(int dC = -1; dC < 2; dC += 1)
            {
                if(Math.abs(dR) == Math.abs(dC))
                {
                    continue;
                }
                if(r + dR > -1 && r + dR < map.length && c + dC > -1 && c + dC < map[r].length)
                {
                    if(map[r + dR][c + dC] && !seen[r + dR][c + dC])
                    {
                        seen[r + dR][c + dC] = true;
                        count += DFS(map, r + dR, c + dC, seen);
                    }
                }
            }
        }
        return count;
    }

    public static boolean[][] generateMap(int width, int height, double p)
    {
        boolean[][] map = new boolean[height][width];
        Random rand = new Random();

        for(int r = 0; r < map.length; r++)
        {
            for(int c = 0; c < map[r].length; c++)
            {
                if(rand.nextDouble() < p)
                {
                    map[r][c] = true;
                }
            }
        }
    
        return map;
    }
}

#include "../include/priorityqueue.h"
#include "../include/node.h"
#include <vector>
#include <limits>
#include <iostream>

std::vector<node> dijkstra(std::vector<node> nodes, 
    std::vector<std::vector<int>> costmatrix, 
    std::vector<std::vector<bool>> boolmatrix,
    node goalnode, node startnode) 
{
    if (goalnode.n == startnode.n)
    {
        return std::vector<node>({goalnode});
    }

    binHeap pq = binHeap();

    int len = nodes.size();
    int goal = 0;
    int start = 0;

    std::vector<node> path;
    std::vector<bool> visited(len);
    std::vector<int> prev(len);
    std::vector<int> dist(len);

    //Initialise variables and set diagonal of adjacency matrix to false
    for (int i = 0; i < len; i++)
    {
        visited[i] = false;
        dist[i] = std::numeric_limits<int>::max();

        boolmatrix[i][i] = false;

        if (nodes[i].n == startnode.n)
        {
            start = i;
            dist[i] = 0;
        }
        else if (nodes[i].n == goalnode.n)
        {
            goal = i;
        }
    }

    pq.add(start, 0);

    while (! pq.isEmpty())
    {
        int current = pq.pop();

        for (int i = 0; i < len; i++)
        {
            if (boolmatrix[current][i])
            {
                int edgecost = costmatrix[current][i];
                int totalcost = dist[current] + edgecost;

                if (totalcost < dist[i])
                {
                    dist[i] = totalcost;
                    prev[i] = current;
                }

                if (!visited[i])
                {
                    pq.add(i, edgecost);
                }
            }
        }

        visited[current] = true;
    }

    int i = goal;
    path.push_back(goalnode);

    while (i != start)
    {
        path.insert(path.begin(), nodes[prev[i]]);
        i = prev[i];
    }

    return path;
}

/*
int main() 
{
    std::vector<node> nodes1 = { node(6), node(7), node(2), node(3), node(4), node(5) };
    std::vector<std::vector<bool>> boolmatrix1 = { {false, true,  false, false, false, false},
                                                  {true,  false, true,  false, false, false},
                                                  {false, true,  false, true,  false, false},
                                                  {false, false, true,  false, true,  false},
                                                  {false, false, false, true,  false, true },
                                                  {false, false, false, false, true,  false} };
    std::vector<std::vector<int>> costmatrix1(6, std::vector<int>(6, 1));
    node goalnode1 = node(5);
    node startnode1 = node(6);

    std::vector<node> path1 = dijkstra(nodes1, costmatrix1, boolmatrix1, goalnode1, startnode1);

    for (int i = 0; i < 6; i++)
    {
        std::cout << path1[i].n;
    }
}
*/
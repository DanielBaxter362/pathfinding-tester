#ifndef ALGORITHMS
#define ALGORITHMS

#include <vector>

std::vector<node> dijkstra(std::vector<node> nodes,
    std::vector<std::vector<int>> costmatrix,
    std::vector<std::vector<bool>> boolmatrix,
    node goalnode, node startnode);

int main()

#endif
#include "../include/node.h"
#include <cmath>

node::node(int n) : n(n) {}

int node::h(int goalx, int goaly)
{
	return 0;
}

hnode::hnode(int n, int x, int y) : node(n), x(x), y(y) {}

int hnode::h(int goalx, int goaly)
{
    return std::abs(goalx - this->x) + std::abs(goaly - this->y);
}

//int main() {}
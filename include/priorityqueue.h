#ifndef PRIORITYQUEUE
#define PRIORITYQUEUE

#include <vector>

struct queuePair 
{
    int cost, n;

    queuePair(int n, int cost);
};

class binHeap
{
private:
    int size = 0;
    std::vector<queuePair> queue;

public:
    void add(int n, int cost);

    int pop();

    bool isEmpty();
};

#endif
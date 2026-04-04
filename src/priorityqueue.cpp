#include "../include/priorityqueue.h"
#include "../include/node.h"
#include <vector>

queuePair::queuePair(int n, int cost)
{
    this->n = n;
    this->cost = cost;
}

void binHeap::add(int n, int cost)
{
    queuePair item = queuePair(n, cost);
    this->queue.push_back(item);

    if (this->queue.size() == 0)
    {
        return;
    }

    int currentIndex = queue.size() - 1;
    while (currentIndex > 0)
    {
        int parentIndex = (currentIndex - 1) / 2;
        queuePair& parent = this->queue[parentIndex];
        if (parent.cost > item.cost)
        {
            this->queue[currentIndex] = parent;
            this->queue[parentIndex] = item;
            currentIndex = parentIndex;
        }
        else {
            break;
        }
    }

    this->size++;
}

int binHeap::pop()
{
    int size = this->queue.size();

    if (size == 0)
    {
        //throw std::runtime_error("Tried to pop empty queue");
        return 0;
    }

    //swap root with last item
    queuePair root = this->queue[0];
    this->queue[0] = this->queue[size - 1];
    this->queue[size - 1] = root;
    this->queue.pop_back();

    //move new root to correct position
    int i = 0;
    while (true)
    {
        int small = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;

        if (l < this->queue.size() && this->queue[l].cost < this->queue[small].cost) {
            small = l;
        }
        if (r < this->queue.size() && this->queue[r].cost < this->queue[small].cost) {
            small = r;
        }
        if (small == i) {
            break;
        }

        //swap
        queuePair temp = this->queue[small];
        this->queue[small] = this->queue[i];
        this->queue[i] = temp;

        i = small;
    }

    this->size--;

    return root.n;
}

bool binHeap::isEmpty()
{
    return (this->size == 0);
}

//int main() {}
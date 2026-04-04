#ifndef NODE
#define NODE

struct node
{
    int n;

    node(int n);

    int h(int goalx, int goaly);
};

struct hnode : node
{
    int x, y;

    hnode(int n, int x, int y);

    int h(int goalx, int goaly);
};

#endif

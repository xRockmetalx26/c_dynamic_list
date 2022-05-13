#ifndef NODE_H
#define NODE_H

#define NODE_SIZE sizeof (struct _Node)

typedef struct _Node {
    void *data;
    struct _Node *next;
} *Node;

#endif // NODE_H

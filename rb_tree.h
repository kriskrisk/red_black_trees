/* Red Black Tree interface */

#ifndef RED_BLACK_TREES_RB_TREE_H
#define RED_BLACK_TREES_RB_TREE_H

#include <stdbool.h>

typedef struct node_t node_t;
typedef struct map_t map_t;
typedef struct memory_block memory_block;

struct node_t {
    bool red;               // True if red
    node_t *left;
    node_t *right;
    node_t *parent;
    memory_block *memory;   // Info about memory block taken by this node_t
};

struct map_t {
    node_t *root;
};

struct memory_block {
    void *vaddr;
    unsigned int size;
    unsigned int flags;
    void *object;
};

int mymap_init(map_t *map);
void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void *object);
void mymap_munmap(map_t *map, void *vaddr);

#endif //RED_BLACK_TREES_RB_TREE_H

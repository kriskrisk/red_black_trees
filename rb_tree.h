/** @file
   Red Black Tree interface.
   @author Krzysztof Olejnik <krzysztofolejnik48@gmail.com>
   @date 2018-04-08
*/

#ifndef RED_BLACK_TREES_RB_TREE_H
#define RED_BLACK_TREES_RB_TREE_H

#include <stdbool.h>

typedef struct node Node;
typedef struct tree Tree;
typedef struct memory_block Memory_block;

struct node {
    bool red;               // True if red
    Node *left;
    Node *right;
    Memory_block *memory;   // Info about memory block taken by this node
};

struct tree {
    Node *root;
};

struct memory_block {
    void *vaddr;
    unsigned int size;
    unsigned int flags;
};

#endif //RED_BLACK_TREES_RB_TREE_H
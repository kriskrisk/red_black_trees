/** @file
   Implementation of red black trees
   @author Krzysztof Olejnik <krzysztofolejnik48@gmail.com>
   @date 2018-04-08
*/

#include <stdlib.h>
#include <assert.h>
#include "rb_tree.h"


int mymap_init(map_t *map) {
    map->root = NULL;
    return 0;
}

Node *createNode(Memory_block *memory) {
    Node *new_node = malloc(sizeof(Node));
    new_node->red = true;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->memory = memory;
    new_node->parent = NULL;

    return new_node;
}

// I assume that node has gradparent
Node *uncle(Node *node) {
    if (node->parent->parent->right == node->parent) {
        return node->parent->parent->left;
    }

    return node->parent->parent->right;
}

Node *grandparent(Node *node) {
    return node->parent->parent;
}

bool isLeftChild(Node *node) {
    return node == node->parent->left;
}

void rotate_left(Node *node, map_t *map) {
    if (node->parent == NULL) {
        map->root = node->right;
        node->right->parent = NULL;
    } else {
        if (isLeftChild(node)) {
            node->parent->left = node->right;
        } else {
            node->parent->right = node->right;
        }
    }

    node->parent = node->right;
    Node *temp = node->right->left;
    node->right->left = node;
    node->right = temp;
}

void rotate_right(Node *node, map_t *map) {
    if (node->parent == NULL) {
        map->root = node->left;
        node->left->parent = NULL;
    } else {
        if (isLeftChild(node)) {
            node->parent->left = node->left;
        } else {
            node->parent->right = node->left;
        }
    }

    node->parent = node->left;
    Node *temp = node->left->right;
    node->left->right = node;
    node->left = temp;
}

void insert_case1(Node *node, map_t *map);

void insert_case3(Node *node, map_t *map) {
    if (isLeftChild(node) && isLeftChild(node->parent)) {
        rotate_right(grandparent(node), map);
        node->parent->red = false;
        node->parent->right->red = true;
    } else if (!isLeftChild(node) && isLeftChild(node->parent)) {
        rotate_left(node->parent, map);
        rotate_right(node->parent, map);
        node->parent->red = false;
        node->parent->right->red = true;
    } else if (!isLeftChild(node) && !isLeftChild(node->parent)) {
        rotate_left(grandparent(node), map);
        node->parent->red = false;
        node->parent->left->red = true;
    } else {
        rotate_right(node->parent, map);
        rotate_left(node->parent, map);
        node->parent->red = false;
        node->parent->left->red = true;
    }

}

void insert_case2(Node *node, map_t *map) {
    if (node->parent->red) {
        if (uncle(node) != NULL && uncle(node)->red) {
            node->parent->red = false;
            uncle(node)->red = false;
            grandparent(node)->red = true;

            insert_case1(grandparent(node), map);
        } else {
            insert_case3(node, map);         // Red parent and black uncle
        }
    }
}

void insert_case1(Node *node, map_t *map) {
    if (node->parent == NULL)
        node->red = false;
    else
        insert_case2(node, map);
}

// BST insertion for now
void *insert(map_t *map, Memory_block *memory_to_allocate) {
    Node *new_node = createNode(memory_to_allocate);
    Node *curr_considered = map->root;

    if (curr_considered == NULL) {
        map->root = new_node;         // Any vaddr is OK
        map->root->red = false;       // Root is always black
    } else {
        // Always try to put as much to the right as possible
        while (true) {
            void *first_free_block_addr = curr_considered->memory->vaddr + curr_considered->memory->size + 1;
            if (first_free_block_addr > new_node->memory->vaddr) {
                new_node->memory->vaddr = first_free_block_addr;
            }

            if (curr_considered->right == NULL) {
                curr_considered->right = new_node;
                new_node->parent = curr_considered;
                break;
            } else {
                curr_considered = curr_considered->right;
            }
        }

        new_node->parent = curr_considered;
        // Rotations and switching colors

        insert_case1(new_node, map);
    }

    return new_node->memory->vaddr;
}

void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void *o) {
    Memory_block *memory_to_allocate = (Memory_block *) malloc(sizeof(Memory_block));
    memory_to_allocate->vaddr = vaddr;
    memory_to_allocate->size = size;
    memory_to_allocate->flags = flags;
    memory_to_allocate->o = o;

    return insert(map, memory_to_allocate);
}

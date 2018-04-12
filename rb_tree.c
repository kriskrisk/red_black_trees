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

static node_t *createNode(memory_block *memory) {
    node_t *new_node = malloc(sizeof(node_t));
    new_node->red = true;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->memory = memory;
    new_node->parent = NULL;

    return new_node;
}

static bool isLeftChild(node_t *node) {
    return node == node->parent->left;
}

// I assume that node_t has gradparent
static node_t *uncle(node_t *node) {
    if (isLeftChild(node->parent)) {
        return node->parent->parent->right;
    }

    return node->parent->parent->left;
}

static node_t *grandparent(node_t *node) {
    return node->parent->parent;
}

static void rotate_left(node_t *node, map_t *map) {
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
    node_t *temp = node->right->left;
    node->right->left = node;
    node->right = temp;
}

static void rotate_right(node_t *node, map_t *map) {
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
    node_t *temp = node->left->right;
    node->left->right = node;
    node->left = temp;
}

static void insert_case1(node_t *node, map_t *map);

static void insert_case3(node_t *node, map_t *map) {
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

static void insert_case2(node_t *node, map_t *map) {
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

static void insert_case1(node_t *node, map_t *map) {
    if (node->parent == NULL)
        node->red = false;
    else
        insert_case2(node, map);
}

static void *insert(map_t *map, memory_block *memory_to_allocate) {
    node_t *new_node = createNode(memory_to_allocate);
    node_t *curr_considered = map->root;

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

void *mymap_mmap(map_t *map, void *vaddr, unsigned int size, unsigned int flags, void *object) {
    memory_block *memory_to_allocate = (memory_block *) malloc(sizeof(memory_block));
    memory_to_allocate->vaddr = vaddr;
    memory_to_allocate->size = size;
    memory_to_allocate->flags = flags;
    memory_to_allocate->object = object;

    return insert(map, memory_to_allocate);
}

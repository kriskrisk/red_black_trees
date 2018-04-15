/* Implementation of red black trees */

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "rb_tree.h"


int mymap_init(map_t *map) {
    map->root = NULL;
    return 0;
}

static void print_subtree(node_t *node) {
    if (node->left != NULL) {
        print_subtree(node->left);
    }

    printf("[vaddr: %p, size: %u, flags: %u object: %p] ",
           node->memory->vaddr, node->memory->size, node->memory->flags, node->memory->object);

    if (node->right != NULL) {
        print_subtree(node->right);
    }
}

int mymap_dump(map_t *map) {
    if (map->root == NULL) {
        printf("Empty map!");
    } else {
        printf("Currently alocated blocks:\n");
        print_subtree(map->root);
    }

    printf("\n\n");

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

static bool is_left_child(node_t *node) {
    return node == node->parent->left;
}

static node_t *grandparent(node_t *node) {
    return node->parent->parent;
}

// I assume that node_t has a gradparent
static node_t *uncle(node_t *node) {
    if (is_left_child(node->parent)) {
        return grandparent(node)->right;
    }

    return grandparent(node)->left;
}

static void rotate_left(node_t *node, map_t *map) {
    if (node->parent == NULL) {
        map->root = node->right;
        node->right->parent = NULL;
    } else {
        if (is_left_child(node)) {
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
        if (is_left_child(node)) {
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

// Case 2: when parent is red and uncle is black
static void insert_case2(node_t *node, map_t *map) {
    if (is_left_child(node) && is_left_child(node->parent)) {
        rotate_right(grandparent(node), map);
        node->parent->red = false;
        node->parent->right->red = true;
    } else if (!is_left_child(node) && is_left_child(node->parent)) {
        rotate_left(node->parent, map);
        rotate_right(node->parent, map);
        node->parent->red = false;
        node->parent->right->red = true;
    } else if (!is_left_child(node) && !is_left_child(node->parent)) {
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

static void insert_case1(node_t *node, map_t *map) {
    if (node->parent == NULL) {
        node->red = false;
    } else {
        if (node->parent->red) {
            if (uncle(node) != NULL && uncle(node)->red) {
                node->parent->red = false;
                uncle(node)->red = false;
                grandparent(node)->red = true;

                insert_case1(grandparent(node), map);
            } else {
                insert_case2(node, map);         // Red parent and black uncle
            }
        }
    }
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

static void replace(map_t *map, node_t *old, node_t *new) {
    if (old->parent == NULL) {
        map->root = new;
    } else {
        if (is_left_child(old))
            old->parent->left = new;
        else
            old->parent->right = new;
    }

    if (new != NULL) {
        new->parent = old->parent;
    }
}

static bool is_leaf(node_t *node) {
    return node->left == NULL && node->right == NULL;
}

// Finds a node with minimal addresses on the right to put in place of deleted node
static node_t *min_on_right(node_t *node) {
    node_t *max = node->right;

    while (!is_leaf(max)) {
        if (max->left != NULL) {
            max = max->left;
        } else {
            max = max->right;
        }
    }

    return max;
}

static void *block_start(node_t *node) {
    return node->memory->vaddr;
}

static void *block_end(node_t *node) {
    return block_start(node) + node->memory->size;
}

static node_t *find_node(map_t *map, void *vaddr) {
    node_t *node = map->root;

    while (node != NULL) {
        if (block_start(node) <= vaddr && vaddr <= block_end(node)) {
            return node;
        } else if (vaddr <= block_start(node)) {
            node = node->left;
        } else {
            node = node->right;
        }
    }

    return node;
}

// Returns color of a node, NULL is balck
static bool is_red(node_t *node) {
    if (node == NULL) {
        return false;
    }

    return node->red;
}

// I assume that node and it's parent aren't NULL
static node_t *sibling(node_t *node) {
    if (is_left_child(node)) {
        return node->parent->right;
    }

    return node->parent->left;
}

static void delete(map_t *map, node_t *node) {
    if (node->parent == NULL)
        return;
    else {
        if (is_red(sibling(node)) == true) {
            node->parent->red = true;
            sibling(node)->red = false;
            if (is_left_child(node)) {
                rotate_left(node->parent, map);
            } else {
                rotate_right(node->parent, map);
            }
        }

        if (is_red(node->parent) == false && is_red(sibling(node)) == false &&
            is_red(sibling(node)->left) == false && is_red(sibling(node)->right) == false) {
            sibling(node)->red = true;
            delete(map, node->parent);
        } else if (is_red(node->parent) == true && is_red(sibling(node)) == false &&
                   is_red(sibling(node)->left) == false && is_red(sibling(node)->right) == false) {
            sibling(node)->red = true;
            node->parent->red = false;
        } else {
            if (is_left_child(node) && is_red(sibling(node)) == false &&
                is_red(sibling(node)->left) == true && is_red(sibling(node)->right) == false) {
                sibling(node)->red = true;
                sibling(node)->left->red = false;
                rotate_right(sibling(node), map);
            } else if (!is_left_child(node) && is_red(sibling(node)) == false &&
                       is_red(sibling(node)->left) == false && is_red(sibling(node)->right) == true) {
                sibling(node)->red = true;
                sibling(node)->right->red = false;
                rotate_left(sibling(node), map);
            }

            sibling(node)->red = is_red(node->parent);
            node->parent->red = false;

            if (is_left_child(node)) {
                sibling(node)->right->red = false;
                rotate_left(node->parent, map);
            } else {
                sibling(node)->left->red = false;
                rotate_right(node->parent, map);
            }
        }
    }
}

void mymap_munmap(map_t *map, void *vaddr) {
    node_t *child;
    node_t *to_delete = find_node(map, vaddr);

    // This memory wasn't alocated yet
    if (to_delete == NULL) {
        return;
    }

    if (to_delete->left != NULL && to_delete->right != NULL) {
        node_t *replacement = min_on_right(to_delete);
        free(to_delete->memory);
        to_delete->memory = replacement->memory;
        to_delete = replacement;
    }

    // to_delete has at most one child
    child = to_delete->right == NULL ? to_delete->left : to_delete->right;
    if (to_delete->red == false) {
        if (child == NULL) {
            to_delete->red = false;
        } else {
            to_delete->red = child->red;
        }
        delete(map, to_delete);
    }

    replace(map, to_delete, child);

    if (to_delete->parent == NULL && child != NULL) {
        child->red = false;
    }

    free(to_delete->memory);
    free(to_delete);
}

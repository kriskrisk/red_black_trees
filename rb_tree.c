/** @file
   Implementation of red black trees
   @author Krzysztof Olejnik <krzysztofolejnik48@gmail.com>
   @date 2018-04-08
*/

#include <stdlib.h>
#include <assert.h>
#include "rb_tree.h"

Node *createNode(Memory_block *memory) {
    Node *new_node = malloc(sizeof(Node));
    new_node->red = true;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->memory = memory;
    new_node->parent = NULL;

    return new_node;
}

bool isLess(Memory_block *block_to_allocate, Memory_block *block_in_memory) {
    if (block_to_allocate->vaddr + block_to_allocate->size < block_in_memory->vaddr) {
        return true;
    }

    return false;
}

//BST insertion for now
void insert(Tree *tree, Memory_block *memory_to_allocate) {
    Node *new_node = createNode(memory_to_allocate);
    Node *curr_considered = tree->root;
    Node *curr_upper_limit = NULL;

    if (curr_considered == NULL) {
        curr_considered = new_node;         // Any vaddr is ok
        curr_considered->red = false;       // Root is always black
    } else {
        while (1) {
            if (isLess(memory_to_allocate, curr_considered->memory)) {
                if (curr_considered->left == NULL) {
                    curr_considered->left = new_node;
                    break;
                } else {
                    curr_upper_limit = curr_considered;
                    curr_considered = curr_considered->left;
                }
            } else {
                if (curr_considered->right == NULL) {
                    if (curr_upper_limit != NULL) {
                        if (isLess(memory_to_allocate, curr_upper_limit->memory)) {
                            curr_considered->right = new_node;
                            break;
                        } else {
                            curr_considered = curr_upper_limit->right;
                        }
                    } else {
                        curr_considered->right = new_node;
                        break;
                    }
                } else {
                    curr_considered = curr_considered->right;
                }
            }
        }

        new_node->parent = curr_considered;
        // Rotations and switching colors
    }
}


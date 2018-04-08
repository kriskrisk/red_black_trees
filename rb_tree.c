/** @file
   Implementation of red black trees
   @author Krzysztof Olejnik <krzysztofolejnik48@gmail.com>
   @date 2018-04-08
*/

#include <stdlib.h>
#include "rb_tree.h"

Node* createNode(bool red, Memory_block *memory) {
    Node *new_node = malloc(sizeof(Node));
    new_node->red = red;
    new_node->left = NULL;
    new_node->right = NULL;
    new_node->memory = memory;

    return new_node;
}

bool isLess(Memory_block *block_to_allocate, Memory_block *block_in_memory) {
    if (block_to_allocate->vaddr + block_to_allocate->size < block_in_memory->vaddr) {
        return true;
    }

    return false;
}



// Assumption: node is not NULL
Node* insert(Node *node, Memory_block *memory_to_allocate) {
    if (isLess(memory_to_allocate, node->memory)) {
        // wstawiamy po lewej
    } else {
        //wstawiamy po prawej
    }
}

Node* insertToTree(Tree *tree, Memory_block *memory_to_allocate) {
    if (tree->root == NULL) {
        tree->root = createNode(false, memory_to_allocate);
    } else {
        insert(tree->root, Memory_block *memory_to_allocate);
    }
}

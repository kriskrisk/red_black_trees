#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"

int main() {
    map_t *map = malloc(sizeof(map_t));
    mymap_init(map);

    void *ptr1;
    int object1 = 354;
    mymap_mmap(map, ptr1, 3, 5, &object1);

    void *ptr2;
    int object2 = 354;
    mymap_mmap(map, ptr2, 3, 5, &object2);

    void *ptr3;
    int object3 = 354;
    mymap_mmap(map, ptr2 + 1, 3, 5, &object3);

    return 0;
}
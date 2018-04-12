#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"

int main() {
    map_t *map = malloc(sizeof(map_t));
    mymap_init(map);

    int a;
    int b;

    void *ptr1 = &a;
    int object1 = 354;
    mymap_mmap(map, ptr1, 3, 5, &object1);

    void *ptr2 = &b;
    int object2 = 354;
    mymap_mmap(map, ptr2, 3, 5, &object2);

    int object3 = 354;
    mymap_mmap(map, ptr2 + 1, 3, 5, &object3);

    mymap_munmap(map, ptr1);
    mymap_munmap(map, ptr2);
    mymap_munmap(map, ptr2 + 4);

    return 0;
}
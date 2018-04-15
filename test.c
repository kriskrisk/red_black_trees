#include <stdio.h>
#include <stdlib.h>
#include "rb_tree.h"

int main() {
    map_t *map = malloc(sizeof(map_t));
    mymap_init(map);

    printf("Just a short program to check is everything works:\n\n");

    int ptr1;
    int ptr2;

    int object1 = 12;
    int object2 = 34;
    int object3 = 56;
    int object4 = 78;

    void *ptr3 = mymap_mmap(map, &ptr1, 1, 2, &object1);
    void *ptr4 = mymap_mmap(map, &ptr2, 3, 4, &object2);

    mymap_dump(map);

    void *ptr5 = mymap_mmap(map, &ptr2 + 1, 5, 6, &object3);
    void *ptr6 = mymap_mmap(map, &ptr1, 7, 8, &object4);

    mymap_dump(map);

    mymap_munmap(map, ptr3);
    mymap_munmap(map, ptr4 + 1);
    mymap_munmap(map, ptr5);
    mymap_munmap(map, ptr6 + 10);

    mymap_dump(map);

    mymap_munmap(map, ptr6);

    mymap_dump(map);

    free(map);

    return 0;
}
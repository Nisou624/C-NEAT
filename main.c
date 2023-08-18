#include <stdio.h>
#include <stdlib.h>
#include "map.h"

int main(){

    printf("Helloooo\n");
    element gg;
    Node* fn = (Node*)malloc(sizeof(Node));
    fn->id = 1;
    fn->type = INPUT_NODE;
    gg.node = fn;
    Map* myMap = createMap(5);
    put(myMap, 1, gg);
    element result = get(myMap, 1);
    printf("the first node of your map has the id of %ld and it's type is: %d", result.node->id, result.node->type);
    destroyMap(myMap);
    return 0;
}
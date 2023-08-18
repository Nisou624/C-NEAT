#include "map.h"
#include <stdlib.h>

#define SIZE_CAPACITY 16

static size_t hash(int key, size_t capacity) {
    return (size_t)key % capacity;
}

Map* createMap(size_t initialSize){
    Map* map = (Map*)malloc(sizeof(Map));
    map->size = initialSize;
    map->data = (Pair**)calloc(initialSize, sizeof(Pair*));
    return map;
}

void put(Map* map, size_t key, element value){
    size_t index = hash(key, map->size);
    Pair* element = (Pair*)malloc(sizeof(Pair));
    element->key = key;
    element->gene = value;
    map->data[index] = element;
}

element get(Map* map, size_t key){
    size_t index = hash(key, map->size);
    Pair* element = map->data[index];
    if(element && element->key == key){
        return element->gene;
    }
}

void destroyMap(Map* map){
    for(size_t i = 0; i< map->size; ++i){
        if(map->data[i]){
            free(map->data[i]);
        }
    }
    free(map->data);
    free(map);
}
#include "map.h"
#include <stdlib.h>
#include <stdbool.h>

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
    if(key > map->size) resizeMap(map, key);
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

bool contains(Map* map, size_t key){
    size_t index = hash(key, map->size);
    Pair* element = map->data[index];
    return (element && element->key == key) ? true : false;
}

void copyElement(element source, element* target){
    if(source.connection){
        target->connection = (Connection*)malloc(sizeof(Connection));
        target->connection->enabled = source.connection->enabled;
        target->connection->inNode = source.connection->inNode;
        target->connection->innovation = source.connection->innovation;
        target->connection->outNode = source.connection->outNode;
        target->connection->weight = source.connection->weight;
    }else{
        target->node = (Node*)malloc(sizeof(Node));
        target->node->id = source.node->id;
        target->node->type = source.node->type;
    }
}

void resizeMap(Map* map, size_t newSize){
    Pair** newData = (Pair**)calloc(newSize, sizeof(Pair*));
   for (size_t i = 0; i < map->size; i++) {
        Pair* entry = map->data[i];
        if (entry) {
            size_t newIndex = hash(entry->key, newSize);
            newData[newIndex] = entry;
        }
    }

    free(map->data);
    map->data = newData;
    map->size = newSize;
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

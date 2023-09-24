#include "map.h"
#include <stdlib.h>
#include <stdbool.h>

static size_t hash(int key, size_t capacity) {
    return (size_t)key % capacity;
}

Map* createMap(size_t initialSize){
    Map* map = (Map*)malloc(sizeof(Map));
    map->size = initialSize;
    map->data = (Pair**)calloc(initialSize, sizeof(Pair*));
    return map;
}

void resizeMap(Map* map, size_t newSize){
    Pair** newData = (Pair**)calloc(newSize, sizeof(Pair*));
    for (size_t i = 1; i < map->size; i++) {
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

void put(Map* map, size_t key, element value){
    if(key > map->size) resizeMap(map, key + 5);
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
    }else if(source.node){
        target->node = (Node*)malloc(sizeof(Node));
        target->node->id = source.node->id;
        target->node->type = source.node->type;
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


arrayList* createList(size_t initialSize){
    arrayList* list = (arrayList*)malloc(sizeof(arrayList));
    list->capacity = initialSize;
    list->size = 0;
    list->data = (int**)calloc(initialSize, sizeof(int*));
    if (list->data == NULL) {
        perror("Failed to allocate memory for ArrayList data");
        exit(EXIT_FAILURE);
    }
    return list;
}

void add(arrayList* list, int* element){
    if(list->size + 1 >= list->capacity) resizeList(list, list->capacity + 5);
    list->data[list->size++] = element;
}

int getArray(arrayList* list, size_t index){
    if(index < list->size) return *(list->data[index]) ;
}

void resizeList(arrayList *list, size_t newSize){
    int** newData = (int**)calloc(newSize, sizeof(int*));
    for (size_t i = 1; i < list->capacity; i++) {
        int* entry = list->data[i];
        newData[i] = entry;
    }

    free(list->data);
    list->data = newData;
    list->capacity = newSize;
}


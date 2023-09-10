#ifndef MAP_H
#define MAP_H
#include <stddef.h>
#include <stdbool.h>

typedef enum {
    INPUT_NODE,
    OUTPUT_NODE,
    HIDDEN_NODE
} NODE_TYPE;

typedef struct 
{
    size_t id;
    float score;
    NODE_TYPE type;
}Node;

typedef struct 
{
    size_t inNode;
    size_t outNode;
    float weight;
    size_t enabled;
    size_t innovation; 
}Connection;

typedef union 
{
    Node* node;
    Connection* connection;
}element;

typedef struct 
{
    size_t key;
    element gene ;
}Pair;

typedef struct
{
    Pair** data;
    size_t size;
} Map;


Map* createMap(size_t initialSize);
void put(Map* map, size_t key, element value);
element get(Map* map, size_t key);
bool contains(Map* map, size_t key);
void copyElement(element source, element* target);
void resizeMap(Map* map, size_t newSize);
void destroyMap(Map* map);



#endif
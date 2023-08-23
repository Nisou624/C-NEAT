#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
//#include "map.h"
//#include "genome.h"

//--------------------------MAP_LIB---------------------------------------------
typedef enum {
    INPUT_NODE,
    OUTPUT_NODE,
    HIDDEN_NODE
} NODE_TYPE;

typedef struct 
{
    size_t id;
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

void destroyMap(Map* map){
    for(size_t i = 0; i< map->size; ++i){
        if(map->data[i]){
            free(map->data[i]);
        }
    }
    free(map->data);
    free(map);
}

//-------------------------------------------GEN LIB ----------------------------------------------------

typedef struct
{
    size_t nodes;
    size_t innov;
    float fitness;
    Map* NodeGene;
    Map* ConnectionGene;
}Genome;

static size_t GlobalInnovationNumber = 1;

//Initialize the genome with 0 nodes and 1 innovation && HashMaps for nodes and connection Genes
void initGenome(Genome* newGenome, size_t inNodes, size_t outNodes){
    newGenome->nodes = 0;
    newGenome->innov = 1;
    Map* nodesGene = createMap((inNodes + outNodes + 1)*2);
    Map* connectionGene = createMap((inNodes + outNodes) *2);
    newGenome->NodeGene = nodesGene; 
    newGenome->ConnectionGene = connectionGene;
}
//function for initialising a Genome with random parameters
Genome* initRGenome(size_t inNodes, size_t outNodes){

    srand(time(NULL));

    //Initializing the genome with default parameters
    Genome* newGenome = (Genome*)malloc(sizeof(Genome));
    initGenome(newGenome, inNodes, outNodes);
    

    //Adding one hidden node per default
    element tmp;
    Node* hNode = (Node*)malloc(sizeof(Node));
    hNode->id = inNodes + outNodes + 1;
    hNode->type = HIDDEN_NODE;
    tmp.node = hNode;
    put(newGenome->NodeGene, hNode->id, tmp);
    newGenome->nodes++;


    /*
        Loop for the Input Nodes:
            Adding the node && a connection from it to the hidden node 
    */
    for(int i = 0; i<inNodes; i++){
        element node;
        Node* nNode = (Node*)malloc(sizeof(Node));
        nNode->id = i + 1;
        nNode->type = INPUT_NODE;
        node.node = nNode;
        put(newGenome->NodeGene, nNode->id, node);
        newGenome->nodes++;

        element con;
        Connection* newCon = (Connection*)malloc(sizeof(Connection));
        newCon->enabled = 1;
        newCon->inNode = nNode->id;
        newCon->outNode = hNode->id;
        newCon->weight = (float)rand() /RAND_MAX;
        newCon->innovation = GlobalInnovationNumber;
        con.connection = newCon;
        put(newGenome->ConnectionGene, newCon->innovation, con);
        incrInnov();
    }
    
    /*
        Loop for the Output Nodes:
            Adding the node to the NodesGene && a connection from it to the hidden node
    */

    for(int i = 0; i < outNodes; i++){
        element node;
        Node* nNode = (Node*)malloc(sizeof(Node));
        nNode->id = i + inNodes + 1;
        nNode->type = OUTPUT_NODE;
        node.node = nNode;
        put(newGenome->NodeGene, nNode->id, node);
        newGenome->nodes++;

        element con;
        Connection* newCon = (Connection*)malloc(sizeof(Connection));
        newCon->enabled = 1;
        newCon->inNode = hNode->id;
        newCon->outNode = nNode->id;
        newCon->weight = (float)rand() /RAND_MAX;
        newCon->innovation = GlobalInnovationNumber;
        con.connection = newCon;
        put(newGenome->ConnectionGene, newCon->innovation, con);
        if(i != outNodes -1) incrInnov();
    }
    return newGenome;
};

//function to add a Node to the genome provided as a parameter (used for mutations)
void addNodeGene(Genome* genome, Node* newNode){
    element nElement;
    nElement.node = newNode;
    put(genome->NodeGene, newNode->id, nElement);
    genome->nodes++;
};

//function to add a Connection to the Genome provided as parameter (used for mutations)
void addConnectionGene(Genome* genome, Connection* newCon){
    element nElement;
    nElement.connection = newCon;
    put(genome->ConnectionGene, newCon->innovation, nElement);
}

//function for the "Add Node Mutation" from the paper
void addNodeMutation(Genome* genome, size_t r){
    //getting the old connection and disabling it
    Connection* c = get(genome->ConnectionGene, r).connection;
    c->enabled = 0;

    //creating the connection from the inNode to the new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->type = HIDDEN_NODE;
    newNode->id = genome->nodes +1;
    Node* n1 = get(genome->NodeGene, c->inNode).node;
    Node* n2 = get(genome->NodeGene, c->outNode).node;
    Connection* inToNew = (Connection*)malloc(sizeof(Connection));
    inToNew->enabled = 1;
    inToNew->inNode = c->inNode;
    inToNew->outNode = newNode->id;
    inToNew->innovation = GlobalInnovationNumber + 1;
    inToNew->weight = 1.0f;
    addConnectionGene(genome, inToNew);
    incrInnov();

    //creating the connection from the new Node to the outNode
    Connection* newToOut = (Connection*)malloc(sizeof(Connection));
    inToNew->enabled = 1;
    inToNew->inNode = newNode->id;
    inToNew->outNode = c->outNode;
    inToNew->innovation = GlobalInnovationNumber + 1;
    inToNew->weight = c->weight;
    addConnectionGene(genome, newToOut);
    incrInnov();

};

//function for the "Add Connection Mutation" from the paper
void addConnectionMutation(Genome* genome, size_t r1, size_t r2){
    srand(time(NULL));
    
    //assuming that the connection exists
    size_t conExists = 1;

    /*
        getting the nodes we want to connect and checking if a connection between them already exists
        if not we set the variable to false 
    */
    Node* n1 = get(genome->NodeGene, r1).node;
    Node* n2 = get(genome->NodeGene, r2).node;
    if(r1 != r2 && n1->type != n2->type){
        for(size_t i = 1; i <= genome->innov; i++){
            Connection* con = get(genome->ConnectionGene, i).connection;
            if((con->inNode == n1->id && con->outNode == n2->id) || (con->inNode == n2->id && con->outNode == n1->id)){
                conExists = 0;
            }
        }
        //if the connection doesn't exists we create it
        if(conExists){
            element newCon;
            newCon.connection = (Connection*)malloc(sizeof(Connection));
            newCon.connection->enabled = 1;
            if(n1->type != OUTPUT_NODE){
                if(n2->type != INPUT_NODE){
                  newCon.connection->inNode = n1;
                  newCon.connection->outNode = n2;  
                }
            }else{
                newCon.connection->inNode = n2;
                newCon.connection->outNode = n1; 
            }
            newCon.connection->weight = (float)rand() /RAND_MAX;
            newCon.connection->innovation = GlobalInnovationNumber + 1;
            incrInnov();
            put(genome->ConnectionGene, newCon.connection->innovation, newCon);
        }
    }
}

/*
    function that evaluates the performance of the genome and gives it a fitness (score)
        for now we assign a determined fitness for testing
*/
void evaluate(Genome* genome, size_t fitness){
    genome->fitness = fitness;
}

//mating two parent genomes
Genome* crossover(Genome* parent1, Genome* parent2){
    bool randbool = rand() & 1;
    Genome* child = (Genome*)malloc(sizeof(Genome));
    Genome* fp = NULL;
    element Nelement;
    element Celement;
    size_t childNodes = parent1->nodes >= parent2->nodes ? parent1->nodes : parent2->nodes;
    size_t childConnections = parent1->ConnectionGene->size >= parent2->ConnectionGene->size ? parent1->ConnectionGene->size : parent2->ConnectionGene->size;
    child->ConnectionGene = createMap(childConnections);
    child->NodeGene = createMap(childNodes);
    initGenome(child, childNodes - 1, 1);
    bool eqFitness = parent1->fitness == parent2->fitness ? true : false;
    if(!eqFitness) fp = parent1->fitness > parent2->fitness ? parent1 : parent2;
    for (size_t i = 0; i < childNodes; i++)
    {
        Nelement = get(child->NodeGene,i + 1);
        copyElement(get(fp ? fp->NodeGene : parent1->NodeGene, i + 1), &Nelement);
        put(child->NodeGene, i + 1, Nelement);
        child->nodes++;
    }
    
    for (size_t i = 1; i <= childConnections; i++)
    {
        randbool = rand() & 1;
        if(contains(parent1->ConnectionGene, i) && contains(parent2->ConnectionGene, i)){
            Celement = get(child->ConnectionGene, i);
            copyElement(get(randbool ? parent1->ConnectionGene : parent2->ConnectionGene, i), &Celement );
            put(child->ConnectionGene, i, Celement);
        }else{
            if(fp){
                Celement = get(child->ConnectionGene, i);
                copyElement(get(fp->ConnectionGene, i), &Celement );
                put(child->ConnectionGene, i, Celement);
            }else{
                Celement = get(child->ConnectionGene, i);
                copyElement(get(randbool ? parent1->ConnectionGene : parent2->ConnectionGene, i), &Celement );
                if(!Celement.connection->enabled) Celement.connection->enabled = randbool;
                put(child->ConnectionGene, i, Celement);
            }
        }
    }
    return child;   
}

//function to print the nodes and connection Genes of a genome (for test purposes and future implementation of visualization)
void printGenome(Genome* newGene){
    element nodes;
    char nodeType[20];
    for (size_t i = 0; i < newGene->nodes; i++)
    {
        nodes = get(newGene->NodeGene, i+1);
        if(!nodes.node){
            continue;
        }
        switch (nodes.node->type)
        {
        case INPUT_NODE:
            strcpy(nodeType, "Input node\n");
            break;
        case OUTPUT_NODE:
            strcpy(nodeType, "Output node\n");
            break;
        case HIDDEN_NODE:
            strcpy(nodeType, "Hidden node\n");
            break;
        
        default:
            strcpy(nodeType, "y'a une erreur bg\n");
            break;
        }
        printf("The node n°%zu is from the type %s\n", nodes.node->id, nodeType);
    }
    printf("the total number of nodes is %zu\n", newGene->nodes);

    printf("The number of connections is:%zu\n", newGene->innov);
    element cons;
    printf("Connections{\n");
    char enabled[10];
    for (size_t i = 0; i < newGene->ConnectionGene->size; i++)
    {
        cons.connection = get(newGene->ConnectionGene, i+1).connection;
        if (!cons.connection) {
            continue;
        }
        switch (cons.connection->enabled)
        {
        case 1:
            strcpy(enabled, "Enabled");
            break;
        case 0:
            strcpy(enabled, "Disabled");
            break;
        default:
            strcpy(enabled, "error");
            break;
        }
        printf("\t%zu ---> %zu\n", cons.connection->inNode, cons.connection->outNode);
        printf("\t%s\n", enabled);
        printf("\tInnovation n°%zu\n", cons.connection->innovation);
        printf("\tThe weight of the connection is %.2f\n", cons.connection->weight);
    }
    printf("}\n");
}

Genome* copyGenome(Genome* source){
    element node;
    element connection;
    Genome* newGenome = (Genome*)malloc(sizeof(Genome));
    newGenome->ConnectionGene = createMap(source->ConnectionGene->size);    
    newGenome->NodeGene = createMap(source->NodeGene->size);
    for (size_t i = 0; i < source->nodes; i++)
    {
        node.node = (Node*)malloc(sizeof(Node));
        copyElement(get(source->NodeGene, i+1), &node);
        put(newGenome->NodeGene, i+1, node);
    }
    for (size_t i = 1; i <= source->ConnectionGene->size; i++)
    {
        connection.connection = (Connection*)malloc(sizeof(Connection));
        copyElement(get(source->ConnectionGene, i), &connection);
        put(newGenome->ConnectionGene, i, connection);
    }
    
    return newGenome;  
}

//self explanatory
void destroyGenome(Genome* genome){
    destroyMap(genome->ConnectionGene);
    destroyMap(genome->NodeGene);
    free(genome);
}

//function to increament the Innovation Number of the Gene
void incrInnov(){
    GlobalInnovationNumber++;
}

//-------------------------------------------TEST CODE---------------------------------------------------
int main(){
    srand(time(NULL));

    Genome* parent1 = (Genome*)malloc(sizeof(Genome));
    initGenome(parent1, 3, 1);
    //------------------------------------------------NODE 1
    Node* nn = (Node*)malloc(sizeof(Node));
    nn->id = 1;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 2
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 2;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 3
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 3;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 4
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 4;
    nn->type = OUTPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 5
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 5;
    nn->type = HIDDEN_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------CONNEXION 1
    Connection* cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 1;
    cnx->outNode = 4;
    cnx->innovation = 1;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
    //------------------------------------------------CONNEXION 2
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = false;
    cnx->inNode = 2;
    cnx->outNode = 4;
    cnx->innovation = 2;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
    //------------------------------------------------CONNEXION 3
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 3;
    cnx->outNode = 4;
    cnx->innovation = 3;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
    //------------------------------------------------CONNEXION 4
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 2;
    cnx->outNode = 5;
    cnx->innovation = 4;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
    //------------------------------------------------CONNEXION 5
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 5;
    cnx->outNode = 4;
    cnx->innovation = 5;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
/////////////////////////////////////////////////////////////////
    Genome* parent2 = (Genome*)malloc(sizeof(Genome));
    parent1->nodes = 5;
    parent2 = copyGenome(parent1);
    //------------------------------------------------CONNEXION 8
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 1;
    cnx->outNode = 5;
    cnx->innovation = 8;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent1, cnx);
///////////////////////////////////////////////////////
    
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 6;
    nn->type = HIDDEN_NODE;
    addNodeGene(parent2, nn);
    parent2->nodes = 6;
    element altered = get(parent2->ConnectionGene, 5);
    altered.connection->enabled = false;
    put(parent2->ConnectionGene, 5, altered);
    //------------------------------------------------CONNEXION 6
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 5;
    cnx->outNode = 6;
    cnx->innovation = 6;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent2, cnx);
    //------------------------------------------------CONNEXION 7
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 6;
    cnx->outNode = 4;
    cnx->innovation = 7;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent2, cnx);
    //------------------------------------------------CONNEXION 9
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 3;
    cnx->outNode = 5;
    cnx->innovation = 9;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent2, cnx);
    //------------------------------------------------CONNEXION 10
    cnx = (Connection*)malloc(sizeof(Connection));
    cnx->enabled = true;
    cnx->inNode = 1;
    cnx->outNode = 6;
    cnx->innovation = 10;
    cnx->weight = (float)rand() /RAND_MAX;
    addConnectionGene(parent2, cnx);
    parent1->fitness = 7.5;
    parent2->fitness = 8.0;
    Genome* child = crossover(parent1, parent2);
    printf("-----------------------------------PARENT 1---------------------------------------------\n");
    printGenome(parent1);
    printf("-----------------------------------PARENT 2---------------------------------------------\n");
    printGenome(parent2);
    printf("-----------------------------------CHILD---------------------------------------------\n");
    printGenome(child);
    destroyGenome(parent1);
    destroyGenome(parent2);
    destroyGenome(child);
    return 0;
}
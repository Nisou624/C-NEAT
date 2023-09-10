#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <assert.h>
#include <stdbool.h>
//#include "map.h"
//#include "genome.h"


//////////////////////////////////////////////////////////MAP//////////////////////////////////////////////////////////
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

//////////////////////////////////////////////////////////GENOME//////////////////////////////////////////////////////////

typedef struct
{
    size_t nodes;
    size_t input;
    size_t output;
    size_t innov;
    float fitness;
    Map* NodeGene;
    Map* ConnectionGene;
}Genome;

#define WEIGHT_MUTATION 0.8
#define NODE_MUTATION 0.03
#define LINK_MUTATION 0.05

size_t GlobalInnovationNumber = 1;
size_t GlobalNodeInnovationNumber = 1;

//function to increament the Innovation Number of the Connections Gene
void incrConsInnov(){
    GlobalInnovationNumber++;
}

//function to increament the Innovation Number of the Nodes Gene
void incrNodesInnov(){
    GlobalNodeInnovationNumber++;
}

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
        incrConsInnov();
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
        if(i != outNodes -1) incrConsInnov();
    }
    return newGenome;
};

//function to add a Node to the genome provided as a parameter (used for mutations)
void addNodeGene(Genome* genome, Node* newNode){
    element nElement;
    nElement.node = newNode;
    put(genome->NodeGene, newNode->id, nElement);
    genome->nodes = GlobalNodeInnovationNumber;
    incrNodesInnov();
};

//function to add a Connection to the Genome provided as parameter (used for mutations)
void addConnectionGene(Genome* genome, Connection* newCon){
    element nElement;
    nElement.connection = newCon;
    put(genome->ConnectionGene, newCon->innovation, nElement);
    incrConsInnov();
}

//function for the "Add Node Mutation" from the paper
void addNodeMutation(Genome* genome, size_t r){
    //getting the old connection and disabling it
    Connection* c = get(genome->ConnectionGene, r).connection;
    if(!c) return;
    c->enabled = 0;

    //creating the connection from the inNode to the new node
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->type = HIDDEN_NODE;
    newNode->id = GlobalNodeInnovationNumber;
    newNode->score = 0;
    addNodeGene(genome, newNode);
    Node* n1 = get(genome->NodeGene, c->inNode).node;
    Node* n2 = get(genome->NodeGene, c->outNode).node;
    Connection* inToNew = (Connection*)malloc(sizeof(Connection));
    inToNew->enabled = 1;
    inToNew->inNode = c->inNode;
    inToNew->outNode = newNode->id;
    inToNew->innovation = GlobalInnovationNumber + 1;
    inToNew->weight = 1.0f;
    addConnectionGene(genome, inToNew);

    //creating the connection from the new Node to the outNode
    Connection* newToOut = (Connection*)malloc(sizeof(Connection));
    newToOut->enabled = 1;
    newToOut->inNode = newNode->id;
    newToOut->outNode = c->outNode;
    newToOut->innovation = GlobalInnovationNumber + 1;
    newToOut->weight = c->weight;
    addConnectionGene(genome, newToOut);

};

//function for the "Add Connection Mutation" from the paper
void addConnectionMutation(Genome* genome, size_t r1, size_t r2){
    srand(rand());
    
    //assuming that the connection exists
    size_t conExists = 1;

    /*
        getting the nodes we want to connect and checking if a connection between them already exists
        if not we set the variable to false 
    */
    Node* n1 = get(genome->NodeGene, r1).node;
    Node* n2 = get(genome->NodeGene, r2).node;
    //see a better solution for this
    if(!n1 || !n2) return;
    if(r1 != r2 && n1->type != n2->type){
        for(size_t i = 1; i <= GlobalInnovationNumber; i++){
            Connection* con = get(genome->ConnectionGene, i).connection;
            if(!con) continue;
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
                  newCon.connection->inNode = n1->id;
                  newCon.connection->outNode = n2->id;  
                }
            }else{
                newCon.connection->inNode = n2->id;
                newCon.connection->outNode = n1->id; 
            }
            newCon.connection->weight = (float)rand() /RAND_MAX;
            newCon.connection->innovation = GlobalInnovationNumber + 1;
            incrConsInnov();
            put(genome->ConnectionGene, newCon.connection->innovation, newCon);
        }
    }
}


//function to assing a new weight to a connection (used for mutations)
void mutateWeight(Genome* genome, size_t conId){
    srand(rand());
    float newWeight = (float) rand() / RAND_MAX;
    Connection* con = get(genome->ConnectionGene, conId).connection;
    if(!con) return;
    printf("previous weight %.2f \n", con->weight);
    con->weight = newWeight;
    printf("new weight %.2f\n", newWeight);
}

//function to shift the weight of a connection (used for mutations)
void shiftWeight(Genome* genome, size_t conId){
    srand(rand());
    float shift = (float) (rand() / RAND_MAX * 2 )- 1;
    Connection* con = get(genome->ConnectionGene, conId).connection;
    if(!con) return;
    printf("previous weight %.2f \n", con->weight);
    con->weight += shift;
    printf("shifted by: %.2f\n", shift);
    printf("new weight %.2f \n", con->weight);
}

//function for the weight Mutation from the paper
void weightMutation(Genome* genome){
    srand(rand());
    size_t conId = rand() % GlobalInnovationNumber;
    if((float)rand() / RAND_MAX <= 0.1){
        mutateWeight(genome, conId);
    }else{
        shiftWeight(genome, conId);
    }
}

//function that wraps all the mutations 
void Mutate(Genome* genome){
    srand(rand());
    if(rand() / RAND_MAX <= WEIGHT_MUTATION){
        printf("weight mutation \n");
        weightMutation(genome);
    }
    if(rand() / RAND_MAX <= NODE_MUTATION){
        printf("node mutation \n");
        addNodeMutation(genome, (rand() % GlobalNodeInnovationNumber) + 1);
    }
    if(rand() / RAND_MAX <= LINK_MUTATION){
        printf("connection mutation \n");
        addConnectionMutation(genome, (rand() % GlobalInnovationNumber) + 1, (rand() % GlobalInnovationNumber) + 1);
    }
}

/*
    function that evaluates the performance of the genome and gives it a fitness (score)
        for now we assign a determined fitness for testing
*/
void evaluate(Genome* genome, size_t fitness){
    genome->fitness = fitness;
}

//activation function (sigmoid)
float sigmoidf(float s){
    return 1.0f / (1 + expf(- s));
}

/*
    should have a map of Node_id weight pair or just weight (decide that later)
        feeding the weight to the input nodes of the genome
        looping through the connectionGene and calculating the score of each node
        returning the final score of the output node(s) 
*/
void feedForward(float* inputs, size_t length, Genome* genome){

    assert(length == genome->input);

    size_t hiddenIds[genome->nodes];
    size_t remainingConnections[GlobalInnovationNumber];
    size_t cc = 0;
    Connection* con;

    float score = 0;
    for (size_t i = 0; i < length; i++)
    {
        Node* node = get(genome->NodeGene, i + 1 ).node;
        node->score = sigmoidf(inputs[i]);
    }
    
    for (size_t i = 1; i <= genome->ConnectionGene->size; i++)
    {
        con = get(genome->ConnectionGene, i).connection;
        if(!con) continue;
        if(!(con->enabled)) continue;
        if(get(genome->NodeGene, con->inNode).node->type == HIDDEN_NODE){
            remainingConnections[cc] = con->innovation;
            cc++;
        }
        get(genome->NodeGene, con->outNode).node->score += get(genome->NodeGene, con->inNode).node->score * con->weight;
    }

    for (size_t i = 0; i < cc; i++)
    {
        con = get(genome->ConnectionGene, remainingConnections[i]).connection;
        if(!con) continue;
        if(!(con->enabled)) continue;
        get(genome->NodeGene, con->outNode).node->score += get(genome->NodeGene, con->inNode).node->score * con->weight;
    }

    for (size_t i = genome->input; i < genome->input + genome->output; i++)
    {
        get(genome->NodeGene, i + 1).node->score = sigmoidf(get(genome->NodeGene, i + 1).node->score);
    }
    
}

//mating two parent genomes
Genome* crossover(Genome* parent1, Genome* parent2){
    bool randbool = rand() & 1;
    Genome* child = (Genome*)malloc(sizeof(Genome));
    Genome* fp = NULL;
    Genome* chosenParent;
    element Nelement, Celement, Pelement;
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
        chosenParent = randbool ? parent1 : parent2;
        if(contains(parent1->ConnectionGene, i) && contains(parent2->ConnectionGene, i)){
            Celement = get(child->ConnectionGene, i);
            Pelement = get(chosenParent->ConnectionGene, i);
            if(Pelement.connection && (contains(child->NodeGene, Pelement.connection->inNode) && contains(child->NodeGene, Pelement.connection->outNode))){
                copyElement(Pelement, &Celement);
                put(child->ConnectionGene, i, Celement);
            }
        }else{
            if(fp && fp->ConnectionGene->size >= i){
                Celement = get(child->ConnectionGene, i);
                Pelement = get(fp->ConnectionGene, i);
                if(Pelement.connection && (contains(child->NodeGene, Pelement.connection->inNode) && contains(child->NodeGene, Pelement.connection->outNode))){
                    copyElement(Pelement, &Celement);
                    put(child->ConnectionGene, i, Celement);
                }
            }else{
                Celement = get(child->ConnectionGene, i);
                if(chosenParent->ConnectionGene->size >= i){
                    Pelement = get(chosenParent->ConnectionGene, i);
                    if(Pelement.connection && (contains(child->NodeGene, Pelement.connection->inNode) && contains(child->NodeGene, Pelement.connection->outNode))){
                        copyElement(Pelement, &Celement );
                        if(!Celement.connection->enabled) Celement.connection->enabled = randbool;
                        put(child->ConnectionGene, i, Celement);
                    }
                }

            }
        }
    }
    return child;   
}

//function to speciate a genome
bool speciate(Genome* mascote, Genome* candidate, float c1, float c2, float c3){
    bool result;
    float formula;
    size_t nodesNumber = mascote->nodes >= candidate->nodes ? mascote->nodes : candidate->nodes;
    size_t connectionsNumber = mascote->ConnectionGene->size >= candidate->ConnectionGene->size ? mascote->ConnectionGene->size : candidate->ConnectionGene->size;
    size_t matchingGenes = 0, excessGenes = 0, disjointGenes = 0, averageWeights = 0;


    for (size_t i = 0; i < nodesNumber; i++)
    {
        if(contains(mascote->NodeGene, i) && contains(candidate->NodeGene, i)){
            matchingGenes++;
        }else if ((contains(mascote->NodeGene, i) && i > candidate->nodes) || (contains(candidate->NodeGene, i) && i > mascote->nodes) )
        {
            excessGenes++;
        }else{
            disjointGenes++;
        }
    }
    

    for (size_t i = 1; i <= connectionsNumber; i++)
    {
        if(contains(mascote->ConnectionGene, i) && contains(candidate->ConnectionGene, i)){
            matchingGenes++;
            averageWeights += abs(get(mascote->ConnectionGene, i).connection->weight - get(candidate->ConnectionGene, i).connection->weight);
        }else if ((contains(mascote->ConnectionGene, i) && i > candidate->ConnectionGene->size) || (contains(candidate->ConnectionGene, i) && i > mascote->ConnectionGene->size) )
        {
            excessGenes++;
        }else{
            disjointGenes++;
        }
    }

    averageWeights /= matchingGenes;
    formula = (c1 * excessGenes) + (c2 * disjointGenes) + (c3 * averageWeights);

    return result = formula >= 0.5 ? true : false;
    
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

    //printf("The number of connections is:%zu\n", newGene->innov);
    element cons;
    printf("Connections{\n");
    char enabled[10];
    for (size_t i = 0; i < newGene->ConnectionGene->size; i++)
    {
        cons.connection = get(newGene->ConnectionGene, i+1).connection;
        if (!(cons.connection)) {
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

//function to visualize the Genome
void viz(Genome* newGene, char* fileName){
    char color[20];
    char weight[20];
    char dotFileName[256]; // You may want to adjust the buffer size as needed
    snprintf(dotFileName, sizeof(dotFileName), "%s.dot", fileName);
    
    FILE* dot = fopen(dotFileName, "w+");
    element cons;
    element nodes;
    if(!dot) return; 
    fprintf(dot, "digraph Genome {\n");
    for (size_t i = 0; i < newGene->nodes; i++)
    {
        nodes = get(newGene->NodeGene, i+1);
        if(!nodes.node){
            continue;
        }
        switch (nodes.node->type)
        {
        case INPUT_NODE:
            strcpy(color, "gold");
            break;
        case OUTPUT_NODE:
            strcpy(color, "darkviolet");
            break;
        case HIDDEN_NODE:
            strcpy(color, "blue");
            break;
        
        default:
            strcpy(color, "aqua");
            break;
        }
        fprintf(dot, "\t%zu [color=\"%s\"];\n", nodes.node->id, color);
    }
    fprintf(dot, "\n");
    
    for (size_t i = 0; i < newGene->ConnectionGene->size; i++)
    {
        cons.connection = get(newGene->ConnectionGene, i+1).connection;
        if (!(cons.connection) || !cons.connection->enabled) {
            continue;
        }
        sprintf(weight, "%.2f", cons.connection->weight);
        fprintf(dot, "\t%zu -> %zu [color=\"%s\", label=\"%s\"];\n", cons.connection->inNode, cons.connection->outNode, cons.connection->weight >= 0.5 ? strcpy(color, "green"): strcpy(color, "red"), weight);
    }
    fprintf(dot, "}\n");
    fclose(dot);
}

/*
    Utility function
        a function that create a copy of a genome
*/
Genome* copyGenome(Genome* source){
    element node;
    element connection;
    Genome* newGenome = (Genome*)malloc(sizeof(Genome));
    newGenome->ConnectionGene = createMap(source->ConnectionGene->size);    
    newGenome->NodeGene = createMap(source->NodeGene->size);
    for (size_t i = 0; i < source->nodes; i++)
    {
        copyElement(get(source->NodeGene, i+1), &node);
        put(newGenome->NodeGene, i+1, node);
    }
    for (size_t i = 1; i <= source->ConnectionGene->size; i++)
    {
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

//////////////////////////////////////////////////////////MAIN//////////////////////////////////////////////////////////

int main(){
    srand(time(NULL));

    Genome* parent1 = (Genome*)malloc(sizeof(Genome));
    initGenome(parent1, 3, 1);
    //------------------------------------------------NODE 1
    Node* nn = (Node*)malloc(sizeof(Node));
    nn->id = 1;
    nn->score = 0;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 2
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 2;
    nn->score = 0;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 3
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 3;
    nn->score = 0;
    nn->type = INPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 4
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 4;
    nn->score = 0;
    nn->type = OUTPUT_NODE;
    addNodeGene(parent1, nn);
    //------------------------------------------------NODE 5
    nn = (Node*)malloc(sizeof(Node));
    nn->id = 5;
    nn->score = 0;
    nn->type = HIDDEN_NODE;
    addNodeGene(parent1, nn);

    Genome* test = (Genome*)malloc(sizeof(Genome));
    test = copyGenome(parent1);
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
    //printGenome(parent1);
    viz(parent1, "parent1_before_mutation");
    Mutate(parent1);
    viz(parent1, "parent1_after_mutation");
    return 0;
}


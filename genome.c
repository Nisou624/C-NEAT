#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <stdbool.h>
#include <assert.h>
#include "genome.h"
#include "map.h"

#define THRESHOLD 3.0
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
        newCon->weight = ((float)rand() / RAND_MAX) * 2 - 1;
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
        newCon->weight = ((float)rand() / RAND_MAX) * 2 - 1;
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
    srand(time(NULL));
    
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
            newCon.connection->weight = ((float)rand() / RAND_MAX) * 2 - 1;
            newCon.connection->innovation = GlobalInnovationNumber + 1;
            incrConsInnov();
            put(genome->ConnectionGene, newCon.connection->innovation, newCon);
        }
    }
}


//function to assing a new weight to a connection (used for mutations)
void mutateWeight(Genome* genome, size_t conId){
    srand(time(NULL));
    float newWeight = ((float)rand() / RAND_MAX);
    Connection* con = get(genome->ConnectionGene, conId).connection;
    if(!con) return;
    printf("previous weight %.2f \n", con->weight);
    con->weight = newWeight;
    printf("new weight %.2f\n", newWeight);
}

//function to shift the weight of a connection (used for mutations)
void shiftWeight(Genome* genome, size_t conId){
    srand(time(NULL));
    float shift = ((float)rand() / RAND_MAX) * 2 - 1;
    Connection* con = get(genome->ConnectionGene, conId).connection;
    if(!con) return;
    printf("previous weight %.2f \n", con->weight);
    con->weight += shift;
    printf("shifted by: %.2f\n", shift);
    printf("new weight %.2f \n", con->weight);
}

//function for the weight Mutation from the paper
void weightMutation(Genome* genome){
    srand(time(NULL));
    size_t conId = rand() % GlobalInnovationNumber;
    if(((float)rand() / RAND_MAX) <= 0.1){
        mutateWeight(genome, conId);
    }else{
        shiftWeight(genome, conId);
    }
}

/*
    function that wraps all the mutations 
    @param genome the Genome we want to mutate
*/
void Mutate(Genome* genome){
    srand(time(NULL));
    if(((float)rand() / RAND_MAX) <= WEIGHT_MUTATION){
        printf("weight mutation \n");
        weightMutation(genome);
    }
    if(((float)rand() / RAND_MAX) <= NODE_MUTATION){
        printf("node mutation \n");
        addNodeMutation(genome, (rand() % GlobalNodeInnovationNumber) + 1);
    }
    if(((float)rand() / RAND_MAX) <= LINK_MUTATION){
        printf("connection mutation \n");
        addConnectionMutation(genome, (rand() % GlobalInnovationNumber) + 1, (rand() % GlobalInnovationNumber) + 1);
    }
}

/*
    function that evaluates the performance of the genome and gives it a fitness (score)
        for now we assign a determined fitness for testing
    @param genome the Genome that we want to assign a fitness
    @param fitness the fitness that we want to assign to the Genome
*/
void evaluate(Genome* genome, size_t fitness){
    genome->fitness = fitness;
}

/*
    activation function (sigmoid)
    @param s the value of the node
*/
float sigmoidf(float s){
    return 1.0f / (1 + expf(- s));
}

/*
    should have a map of Node_id weight pair or just weight (decide that later)
        feeding the weight to the input nodes of the genome
        looping through the connectionGene and calculating the score of each node
        returning the final score of the output node(s)
    @param inputs an array of the value of the input nodes
    @param length the number of inputs nodes (also the length of the inputs)
    @param genome the working Genome
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

/*
    mating two parent genomes
    @param parent1 the first parent (fitter parent)
    @param parent2 the second parent (less fitter)
    @returns a child Genome
*/
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

//function to calculate the distance between two genome (used for speciation)
bool distance(Genome* mascote, Genome* candidate, float c1, float c2, float c3){
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

    return formula <= THRESHOLD ? true : false;
    
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


void shareScore(arrayList* specie){
    float score = 0.0f;
    for (size_t i = 0; i < specie->size; i++) {
        Genome* genome = getArray(specie, i);
        if (genome != NULL) {
            score += genome->fitness;
        }
    }
    specie->score = score;   
}

void annihilate(arrayList* specie){
    size_t index = round(0.15 * specie->size);
    for (size_t i = index - 1 ; i < specie->size; i++)
    {
        specie->data[i] = NULL;
    }
    
}

population* initPopulation(size_t inNodes, size_t outNodes){
    population* neat = (population*)malloc(sizeof(population));
    neat->conInnovation = 0;
    neat->nodeInnovation = 0;
    for (size_t i = 0; i < 100; i++)
    {
        neat->population[i] = initRGenome(inNodes, outNodes);
    }
    return neat;
}


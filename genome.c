#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "genome.h"
#include "map.h"

//function for initialising a Genome
Genome* initGenome(size_t inNodes, size_t outNodes){

    srand(time(NULL));

    //Initialize the genome with 0 nodes and 1 innovation && HashMaps for nodes and connection Genes
    Genome* newGenome = (Genome*)malloc(sizeof(Genome));
    newGenome->nodes = 0;
    newGenome->innov = 1;
    Map* nodesGene = createMap((inNodes + outNodes + 1)*2);
    Map* connectionGene = createMap((inNodes + outNodes) *2);

    //Adding one hidden node per default
    element tmp;
    Node* hNode = (Node*)malloc(sizeof(Node));
    hNode->id = inNodes + outNodes + 1;
    hNode->type = HIDDEN_NODE;
    tmp.node = hNode;
    put(nodesGene, hNode->id, tmp);
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
        put(nodesGene, nNode->id, node);
        newGenome->nodes++;

        element con;
        Connection* newCon = (Connection*)malloc(sizeof(Connection));
        newCon->enabled = 1;
        newCon->inNode = nNode->id;
        newCon->outNode = hNode->id;
        newCon->weight = (float)rand() /RAND_MAX;
        newCon->innovation = newGenome->innov;
        con.connection = newCon;
        put(connectionGene, newCon->innovation, con);
        newGenome->innov++;
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
        put(nodesGene, nNode->id, node);
        newGenome->nodes++;

        element con;
        Connection* newCon = (Connection*)malloc(sizeof(Connection));
        newCon->enabled = 1;
        newCon->inNode = hNode->id;
        newCon->outNode = nNode->id;
        newCon->weight = (float)rand() /RAND_MAX;
        newCon->innovation = newGenome->innov;
        con.connection = newCon;
        put(connectionGene, newCon->innovation, con);
        if(i != outNodes -1) newGenome->innov++;
    }

    newGenome->ConnectionGene = connectionGene;
    newGenome->NodeGene = nodesGene;
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
    inToNew->innovation = genome->innov + 1;
    inToNew->weight = 1.0f;
    addConnectionGene(genome, inToNew);
    genome->innov++;

    //creating the connection from the new Node to the outNode
    Connection* newToOut = (Connection*)malloc(sizeof(Connection));
    inToNew->enabled = 1;
    inToNew->inNode = newNode->id;
    inToNew->outNode = c->outNode;
    inToNew->innovation = genome->innov + 1;
    inToNew->weight = c->weight;
    addConnectionGene(genome, newToOut);
    genome->innov++;

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
            newCon.connection->innovation = genome->innov + 1;
            genome->innov++;
            put(genome->ConnectionGene, newCon.connection->innovation, newCon);
        }
    }
}

//function to print the nodes and connection Genes of a genome (for test purposes and future implementation of visualization)
void printGenome(Genome* newGene){
    element nodes;
    char nodeType[20];
    for (size_t i = 0; i < newGene->nodes; i++)
    {
        nodes = get(newGene->NodeGene, i+1);
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
    for (size_t i = 0; i < newGene->innov; i++)
    {
        cons.connection = get(newGene->ConnectionGene, i+1).connection;
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

//self explanatory
void destroyGenome(Genome* genome){
    destroyMap(genome->ConnectionGene);
    destroyMap(genome->NodeGene);
    free(genome);
}


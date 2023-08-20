#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "map.h"
#include "genome.h"

//--------------------------MAP_LIB---------------------------------------------

//-------------------------------------------GEN LIB ----------------------------------------------------

//-------------------------------------------TEST CODE---------------------------------------------------
int main(){

    Genome* newGene = initGenome(3, 1);
    printGenome(newGene);
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->id = newGene->nodes + 1;
    newNode->type = HIDDEN_NODE;
    addNodeGene(newGene, newNode);
    printf("------------------------- New Node Added TEST -------------------------\n");
    printGenome(newGene);
    destroyGenome(newGene);
    return 0;
}
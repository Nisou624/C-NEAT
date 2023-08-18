#ifndef GENOME_H
#define GENOME_H
#include <stddef.h>
#include "map.h"

typedef struct
{
    size_t innov = 0;
    Map* NodeGene;
    Map* ConnectionGene;
}Genome;

void initGenome(size_t inNodes, size_t outNodes){
    //
}



#endif
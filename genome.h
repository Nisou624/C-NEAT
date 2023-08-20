#ifndef GENOME_H
#define GENOME_H
#include <stddef.h>
#include "map.h"

typedef struct
{
    size_t nodes;
    size_t innov;
    Map* NodeGene;
    Map* ConnectionGene;
}Genome;

Genome* initGenome(size_t inNodes, size_t outNodes);
void addNodeGene(Genome* genome, Node* newNode);
void addConnectionGene(Genome* genome, Connection* newCon);
Genome* crossover(Genome* parent1, Genome* parent2);
void addNodeMutation(Genome* genome, size_t r);
void addConnectionMutation(Genome* genome, size_t r1, size_t r2);
void PrintGenome(Genome* newGene);
void destroyGenome(Genome* genome);

#endif
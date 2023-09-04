#ifndef GENOME_H
#define GENOME_H
#include <stddef.h>
#include "map.h"

extern GlobalInnovationNumber;

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


void initGenome(Genome* newGenome, size_t inNodes, size_t outNodes);
Genome* initRGenome(size_t inNodes, size_t outNodes);
void addNodeGene(Genome* genome, Node* newNode);
void addConnectionGene(Genome* genome, Connection* newCon);
Genome* crossover(Genome* parent1, Genome* parent2);
void evaluate(Genome* genome, size_t fitness);
void addNodeMutation(Genome* genome, size_t r);
void addConnectionMutation(Genome* genome, size_t r1, size_t r2);
void PrintGenome(Genome* newGene);
Genome* copyGenome(Genome* source);
void destroyGenome(Genome* genome);
void incrInnov();

#endif
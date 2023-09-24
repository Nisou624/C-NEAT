#ifndef GENOME_H
#define GENOME_H
#include <stddef.h>
#include "map.h"

extern size_t GlobalInnovationNumber;
extern size_t GlobalNodeInnovationNumber;

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


typedef struct
{
    Genome* population[100];
    size_t conInnovation;
    size_t nodeInnovation;
} population;



void incrConsInnov();
void incrNodesInnov();
void initGenome(Genome* newGenome, size_t inNodes, size_t outNodes);
Genome* initRGenome(size_t inNodes, size_t outNodes);
void addNodeGene(Genome* genome, Node* newNode);
void addConnectionGene(Genome* genome, Connection* newCon);
void addNodeMutation(Genome* genome, size_t r);
void addConnectionMutation(Genome* genome, size_t r1, size_t r2);
void mutateWeight(Genome* genome, size_t conId);
void shiftWeight(Genome* genome, size_t conId);
void weightMutation(Genome* genome);
void Mutate(Genome* genome);
void evaluate(Genome* genome, size_t fitness);
float sigmoidf(float s);
void feedForward(float* inputs, size_t length, Genome* genome);
Genome* crossover(Genome* parent1, Genome* parent2);
bool distance(Genome* mascote, Genome* candidate, float c1, float c2, float c3);
void printGenome(Genome* newGene);
void viz(Genome* newGene, char* fileName);
Genome* copyGenome(Genome* source);
void destroyGenome(Genome* genome);

#endif
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char const *argv[])
{
    srand(time(NULL));
    float randomUno, randomDos, randomTres;
    randomUno = ((float)rand() / RAND_MAX) * 2 - 1;
    printf("random 1 %.2f\n", randomUno);
    randomDos = ((float)rand() / RAND_MAX) * 2 - 1;
    printf("random 2 %.2f\n", randomDos);
    randomTres = ((float)rand() / RAND_MAX) * 2 - 1;
    printf("random 3 %.2f\n", randomTres);

    return 0;
}

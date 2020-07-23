#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

double ran_expo(double lambda){
    double u;
    u = (rand() % 5 + 1) / (5 + 1.0);
    printf("---%f \n", u);
    return -log(1 - u) / 0.05;
}

int main(void)
{
    int i;
//    srand((unsigned)time(NULL));
//    for (i=0; i<20; i++)
//        printf("%f\n", ran_expo(0.05));

    int random_number = rand() % 100 + 1;
    printf("%d\n", random_number);
    return 0;
}
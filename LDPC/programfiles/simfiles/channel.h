#include <stdio.h>
#include <stdlib.h>


void awgnvector(channels *, int);

void bernoullivector(channels *, int);

void fixederrorsvector(double *noise, int n, double p);

void initialize_channel(channels *, simulations *, graphs *, int param, int loop);

void allocatechannelmemory(graphs *, channels *);



#define Belief 1
#define GalA 2


#define MAXMESSAGE 20
#define EXPMAXMESSAGE 485165000
#define EXPNEGMAXMESSAGE 0.00000000206115
#define MAXNEIGHBORS 100

double sign(double x);

void checkmessagemap(graphs *, int iteration, int decodertype);

void variablemessagemap(graphs *, int iteration, int decodertype);

void initializegraph(graphs *, simulations *, double *);

void makedecisions(graphs * graph, double *decision, int iteration, int which);

void decoding(graphs *, channels *, simulations *, errorcounts * errorcount);

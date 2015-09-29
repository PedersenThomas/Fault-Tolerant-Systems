void uppertriangulation(graphs * graph, int *gap, int verbose);
int findsmallestdegreevnode(graphs * graph, int currentvnode);
void switchvnode(graphs * graph, int nodea, int nodeb);
void switchcnode(graphs * graph, int nodea, int nodeb);
void removedoubleoverlaps(graphs * graph, int verbose);
void printhmatrixtall(graphs * graph);
void printhmatrix(graphs * graph);
void plotshapeofhmatrix(graphs * graph);
void printmatrix(int *matrix, int gap, int vnodenum);
void graphmultiply(graphs * graph, int rowstart, int rowend, int colstart, int colend, int *in,
                   int *out);
void multiplybytminusone(graphs * graph, int gap, int *in, int *out);
void cleararray(int *array, int length);
void determineiphi(graphs * graph, encoders * encoder, char *graphout, int verbose);
void inversematrix(int *matrix, int *inversematrix, int dimension);
void encode(graphs * graph, int *iphi, int *codeword, int gap, int verbose);
void constructencoder(graphs *, channels *, encoders *, simulations *, int loop);
void encoding(graphs * graph, encoders * encoder, channels * channel);

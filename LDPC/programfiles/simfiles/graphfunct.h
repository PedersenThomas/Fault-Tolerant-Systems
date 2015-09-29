/* headfile containing function headers for graph.c */

void printgraph(graphs *, int);
void switchedges(graphs *, int, int, int, int);
void constructirregulargraph(graphs *, simulations *);
void constructprotograph(graphs *, simulations *);
void constructgraph(simulations * simulation, graphs * graph, randomizes * randomize,
                    encoders * encoder, reruns * rerun, int loop, int l);
void readprotograph(simulations *);
void readgraph(graphs * graph, encoders * encoder, char *graphfile, int verbose);
void savegraph(graphs * graph, encoders * encoder, char *graphfile, int verbose);
void freegraph(graphs * graph);

int readparamfile(int argc, char **argv, simulations * simulation, graphs * graph,
                  encoders * encoder, channels * channel, errorcounts * errorcount,
                  randomizes * randomize, reruns * rerun);

int sim_readparamfile(FILE * fpinput, simulations * simulation, graphs * graph,
                      encoders * encoder, channels * channel, errorcounts * errorcount,
                      randomizes * randomize, reruns * rerun);

void sanitycheck(simulations * simulation, graphs * graph, encoders * encoder, channels * channel,
                 errorcounts * errorcount, randomizes * randomize);

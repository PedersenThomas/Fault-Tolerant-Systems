#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/time.h>
#include <assert.h>
#include "sim.h"
#include "graphstruct.h"
#include "simulation.h"
#include "graphfunct.h"
#include "sub.h"
#include "decode.h"
#include "channel.h"
#include "encode.h"
#include "readparam.h"
#include "evaluation.h"

#define STOP_ITER 20
#define MAXLEVEL 20
#define MAXLEVELHALF 10


/* global variables */
unsigned short channelsimrand[3];

unsigned short graphsimrand[3];

int main(int argc, char **argv)
{
   graphs *graph;               // contains variables related to graph
   errorcounts *errorcount;     // contains variables related to performance
   randomizes *randomize;       // contains variables which describe randomness
   encoders *encoder;           // contains variables which help in encoding
   channels *channel;           // contains variables which describe channel
   simulations *simulation;     // contains variables which describe sim parameters
   reruns *rerun;               // contains variables which describe options for reruns of

   // realization

   int i, readparamfile_status;
   int l, loop, flag;
   int channelparameter_num;

   // allocate memory for structures
   graph = (graphs *) allocate(sizeof(graphs));
   (*graph).vnodenum = -1;      // indicate that channel as not been initialized
   errorcount = (errorcounts *) allocate(sizeof(errorcounts));
   randomize = (randomizes *) allocate(sizeof(randomizes));
   encoder = (encoders *) allocate(sizeof(encoders));
   channel = (channels *) allocate(sizeof(channels));
   (*channel).length = -1;      // indicate that channel has not been initialized
   simulation = (simulations *) allocate(sizeof(simulations));
   rerun = (reruns *) allocate(sizeof(reruns));

   // read all parameters from file and store into various structures
   if (readparamfile(argc, argv, simulation, graph, encoder, channel, errorcount, randomize, rerun)
       == 0)
      return 0;
   // perform some very basic checks to make sure input is consistent
   // print warnings for potential errors by let program continue in any case
   sanitycheck(simulation, graph, encoder, channel, errorcount, randomize);

   // this is the format, results are printed to the screen
   print_outputformat(errorcount);

   // indicated that we just before simulating the first point
   // and the first codeword
   l = 0;
   loop = -1;
   // construct a sample graph or read it in so that we
   // know the correct codeword length
   constructgraph(simulation, graph, randomize, encoder, rerun, loop, l);
   constructencoder(graph, channel, encoder, simulation, loop);
   // now where we constructed a sample graph we know the length
   // ... and we can allocate memory for the channel structure
   allocatechannelmemory(graph, channel);
   if ((*simulation).verbose == 1)
   {
      printf("\n\n******************************");
      printf("\n start loops");
      printf("\n******************************\n");
   }

  /************************************************/
   /* simulations for different channel parameter */
  /************************************************/
   // the outer most loop is over the different channel parameters
  for (l = 0; l < (*channel).parameter_num; l++)
  {
  		setchannelseeds(loop, rerun);
       getchannelsimrand(randomize);

     /* clear all counters and open data files */
     initialize_errorcount(argc, argv, l, errorcount, simulation,randomize, channel, graph);
	
     /* for each fixed channel parameter run loop (=realizations) over codewords */
     for (loop = 0; loop < (*simulation).numloops; loop++)
     {
	  	//printf("\n loop %i",loop);
        /* save seeds that had been used for intializing the random generator in this realization 
           (they will be saved in the output data-file) */
        //setchannelseeds(loop, rerun);
        //getchannelsimrand(randomize);

        /* start by constructing the graph and encoder if needed */
        constructgraph(simulation, graph, randomize, encoder, rerun, loop, l);
        constructencoder(graph, channel, encoder, simulation, loop);

        /* produce the channel noise */
        initialize_channel(channel, simulation, graph, l, loop);

        /* modulate the channel noise according to codeword */
        encoding(graph, encoder, channel);

        /* initialize variable-values and message */
        initializegraph(graph, simulation, (*channel).noise);

        /* do iterative decoding */
        decoding(graph, channel, simulation, errorcount);

        /* final count of decoding errors */
        count_error(graph, channel, errorcount, simulation);

        /* stop if accuracy goal is reached before end of end of loops */
        if (print_errors(loop, l, simulation, errorcount, channel, graph, randomize) == 1)
           break;
     }
  }
   printf("\n simulation ends\n");
}

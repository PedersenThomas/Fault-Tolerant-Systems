/* routine to transform graphdescription files by MacKay
  (http://www.inference.phy.cam.ac.uk/mackay/codes/data.html) into format used by
   LTHC at EPFL Lausanne/Switzerland.
 	usage: ./mackay2lth mackayinputfile lthoutputfile
**	The lthoutputfile can now be used for ldpc-performance simulation using the sim-program in the same package (see sample parameter file 'param_mackay_1920.1280.3.303')
*/



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
   int i;
   int verbose = 0;

   // allocate memory for structures
   graph = (graphs *) allocate(sizeof(graphs));

   int node, vnodenum, cnodenum, vdegreesnum, cdegreesnum;
   int socket, tonode, tosocket;
   int dummy;
   encoders *encoder;
   degreelist *vdegreelist, *cdegreelist;
   FILE *fpinput;
   FILE *fpoutput;

   encoder = (encoders *) allocate(sizeof(encoders));

	if(verbose)
		printf("\n inputfile %s, outputfile %s", argv[1], argv[2]);

   if ((fpinput = fopen(argv[1], "r")) == NULL)
   {
      fprintf(stderr, "I can't open the indicated input file.\n");
      return 0;
   }
   if ((fpoutput = fopen(argv[2], "w")) == NULL)
   {
      fprintf(stderr, "I can't open the indicated output file.\n");
      return 0;
   }

   graph = (graphs *) allocate(sizeof(graphs));
   // read in main parameters

   /* read in variable and check nodenum */
   fscanf(fpinput, "%d", &(*graph).vnodenum);
   fscanf(fpinput, "%d", &(*graph).cnodenum);
   if (verbose)
      printf("\n vnodenum %i cnodenum %i", (*graph).vnodenum, (*graph).cnodenum);

   /* read in max variable and check node degree */
   fscanf(fpinput, "%d", &(*graph).vmaxdegree);
   fscanf(fpinput, "%d", &(*graph).cmaxdegree);
   if (verbose)
      printf("\n vmaxdegree %i cmaxdegree %i", (*graph).vmaxdegree, (*graph).cmaxdegree);

   (*graph).rate = 1.0 - ((double)(*graph).cnodenum) / ((double)(*graph).cnodenum);

   // allocate memory for nodes
   (*graph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
   (*graph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);

   // initialize check degree to zero
   for (node = 0; node < (*graph).cnodenum; node++)
      (*((*graph).cnodelist + node)).degree = 0;

   if (verbose)
      printf("\n variable degree:");
   // start by reading in vnode degrees
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      // degree of the node
      fscanf(fpinput, "%d", &vnodedegree);
      if (verbose)
         printf("(%i,%i) ", node, vnodedegree);
      // allocate memory for edges
      vnodeedgelist = (edge *) allocate(sizeof(edge) * vnodedegree);
   }

   if (verbose)
      printf("\n skip check degree:");
   // skip reading in cnode degrees
   for (node = 0; node < (*graph).cnodenum; node++)
   {
      // degree of the node
      fscanf(fpinput, "%d", &dummy);
   }

   /* Read in variable node neighbors */
   if (verbose)
      printf("\n vnodes");
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      if (verbose)
         printf("\n");
      for (socket = 0; socket < (*graph).vmaxdegree; socket++)
      {
         /* if vmaxdegree>=vnodedegree) skip last entryies */
         if (socket >= vnodedegree)
            fscanf(fpinput, "%d", &dummy);
         else
         {
            fscanf(fpinput, "%d", &tonode);
            vnodeedge.dest = tonode - 1;
            // count degree of check nodes
            vnodeedge.socket = (*((*graph).cnodelist + vnodeedge.dest)).degree++;

            if (verbose)
               printf(" %i %i  ", tonode, vnodeedge.socket);
         }
      }
   }


   // allocate memory for check node edges
   for (node = 0; node < (*graph).cnodenum; node++)
   {
      cnodeedgelist = (edge *) allocate(sizeof(edge) * cnodedegree);
		if(verbose)
      	printf("cnode %i degree %i\n", node, cnodedegree);
   }
   // transform view from var nodes to check node edges
	if(verbose)
   	printf("\n transform view from var nodes to check nodes:\n");
   for (node = 0; node < (*graph).vnodenum; node++)
   {
		if(verbose)
      	printf("node %i degree %i\n", node, vnodedegree);
      for (socket = 0; socket < vnodedegree; socket++)
      {
			if(verbose)
         	printf("  dest %i socket %i\n", vnodeedge.dest, vnodeedge.socket);
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).dest = node;
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).socket = socket;
      }
   }
   fclose(fpinput);



   /* find vdegreelist, vdegreelist */

   (*graph).vdegreelist = (degreelist *) allocate(sizeof(degreelist));
   (*graph).cdegreelist = (degreelist *) allocate(sizeof(degreelist));

   vdegreelist = (degreelist *) allocate(sizeof(degreelist));
   cdegreelist = (degreelist *) allocate(sizeof(degreelist));

   for (i = 0; i < MAXDEGREENUM; i++)
   {
      (*vdegreelist).numofnodes[i] = 0;
      (*cdegreelist).numofnodes[i] = 0;
   }

   for (node = 0; node < (*graph).vnodenum; node++)
      (*vdegreelist).numofnodes[vnodedegree]++;

   for (node = 0; node < (*graph).cnodenum; node++)
      (*cdegreelist).numofnodes[cnodedegree]++;

   (*graph).vdegreesnum = (*graph).cdegreesnum = 0;
   for (i = 0; i < MAXDEGREENUM; i++)
   {
      if ((*vdegreelist).numofnodes[i] > 0)
      {
			if(verbose)
         	printf("\nVdegree %i numofnodes %i", i, (*(*graph).vdegreelist).numofnodes[i]);

         (*(*graph).vdegreelist).degree[(*graph).vdegreesnum] = i;
         (*(*graph).vdegreelist).numofnodes[(*graph).vdegreesnum] = (*vdegreelist).numofnodes[i];
         (*graph).vdegreesnum++;
      }

      if ((*cdegreelist).numofnodes[i] > 0)
      {
			if(verbose)
         	printf("\nCdegree %i numofnodes %i", i, (*(*graph).cdegreelist).numofnodes[i]);
         (*(*graph).cdegreelist).degree[(*graph).cdegreesnum] = i;
         (*(*graph).cdegreelist).numofnodes[(*graph).cdegreesnum] = (*cdegreelist).numofnodes[i];

         (*graph).cdegreesnum++;
      }
   }

	if(verbose)
	{
   	printf("\n test Vdegreelist");
  	 	for (i = 0; i < (*graph).vdegreesnum; i++)
      	printf("\n degree %i numofnodes %i",
             (*(*graph).vdegreelist).degree[i], (*(*graph).vdegreelist).numofnodes[i]);

   	printf("\n test Cdegreelist");
   	for (i = 0; i < (*graph).cdegreesnum; i++)
      	printf("\n degree %i numofnodes %i",
             (*(*graph).cdegreelist).degree[i], (*(*graph).cdegreelist).numofnodes[i]);

	}
   savegraph(graph, encoder, argv[2], 0);
   printf("\n simulation ends\n");
}

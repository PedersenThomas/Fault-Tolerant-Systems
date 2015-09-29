#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include "graphstruct.h"
#include "simulation.h"
#include "graphfunct.h"
#include "sub.h"

/********************
 * rounding function *
 * of a given graph  *
 ********************/
double round(double x)
{
   return floor(x + 0.5);
}


/************************************
* print out the relevant quantities *
* of a given graph                  *
* 0 -> print general parameters     *
* 2 -> print connection list        *
* 4 -> print messages               *
************************************/
void printgraph(graphs * graph, int what)
{
   int i, node, socket, tonode, tosocket;

   FILE *fp;

   /* print general parameters */
   printf("\n ************************\n");
   if ((what & 1) != 0)
   {
      printf("\n");
      printf("Graph: rate:=%g\n", (*graph).rate);
      printf("vnodenum:=%6d  cnodenum:=%6d\n", (*graph).vnodenum, (*graph).cnodenum);
      printf("vdegreesnum:=%3d  cdegreesnum:=%3d\n", (*graph).vdegreesnum, (*graph).cdegreesnum);
      printf("vmaxdegree:= %3d  cmaxdegree:= %3d\n", (*graph).vmaxdegree, (*graph).cmaxdegree);
      printf("\n");
      printf("vdegreelist:\n");
      for (i = 0; i < (*graph).vdegreesnum; i++)
         printf("degree:=%3d number of nodes:=%3d\n",
                (*(*graph).vdegreelist).degree[i], (*(*graph).vdegreelist).numofnodes[i]);
      printf("\n");
      printf("cdegreelist:\n");
      for (i = 0; i < (*graph).cdegreesnum; i++)
         printf("degree:=%3d number of nodes:=%3d\n",
                (*(*graph).cdegreelist).degree[i], (*(*graph).cdegreelist).numofnodes[i]);
   }

   /* print connection list */
   if ((what & 2) != 0)
   {
      /* connection list */
      printf("list of neighbors from vnode perspective:\n");
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         printf("%6d: ", node);
         for (socket = 0; socket < vnodedegree; socket++)
            printf("%6d[%2d] ", vnodeedge.dest, vnodeedge.socket);
         printf("\n");
      }

      printf("list of neighbors from the cnode perspective:\n");
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         printf("%6d:", node);
         for (socket = 0; socket < cnodedegree; socket++)
            printf("%6d[%2d] ", cnodeedge.dest, cnodeedge.socket);
         printf("\n");
      }
   }

   /* print messages */
   if ((what & 4) != 0)
   {
      /* print variable messages */
      printf("variable messages:\n");
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         printf("%6d: [%6g] ", node, vnodevalue);
         for (socket = 0; socket < vnodedegree; socket++)
         {
            tonode = vnodeedge.dest;
            tosocket = vnodeedge.socket;
            printf("%6g ", cnodetoedge.message);
         }
         printf("\n");
      }

      /* print check messages */
      printf("check messages:\n");
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         printf("%6d: ", node);
         for (socket = 0; socket < cnodedegree; socket++)
         {
            tonode = cnodeedge.dest;
            tosocket = cnodeedge.socket;
            printf("%6g ", vnodetoedge.message);
         }
         printf("\n");
      }
   }
   printf("\n*******************\n");
}

/*******************************************
 * save a graph in a file                  *
 ******************************************/
void savegraph(graphs * graph, encoders * encoder, char *graphfile, int verbose)
{
   int i, node, vnodenum, cnodenum, vdegreesnum, cdegreesnum;
   int row, col;
   int vmaxdegree, cmaxdegree, degree, socket, tonode, tosocket;
   double rate;
   FILE *fp;

   // open the file
   if ((fp = fopen(graphfile, "w")) == NULL)
   {
      fprintf(stderr, "I can't open the indicated file: %s.\n",graphfile);
      return exit(1);
   }

   // read in main parameters
   fprintf(fp, "/****************************\n");
   fprintf(fp, "*Description File of a Code *\n");
   fprintf(fp, "******************************/\n");

   fprintf(fp, "\n");
   fprintf(fp, "/*vnodednum and cnodenum*/\n");
   fprintf(fp, "%7d %7d\n", (*graph).vnodenum, (*graph).cnodenum);

   fprintf(fp, "\n");
   fprintf(fp, "/* vdegreesnum and cdegreesnum */\n");
   fprintf(fp, "%7d %7d\n", (*graph).vdegreesnum, (*graph).cdegreesnum);

   fprintf(fp, "\n");
   fprintf(fp, "/* vmaxdegree and cmaxdegree */\n");
   fprintf(fp, "%7d %7d\n", (*graph).vmaxdegree, (*graph).cmaxdegree);

   fprintf(fp, "\n");
   fprintf(fp, "/* list of vnode degrees */\n");
   for (i = 0; i < (*graph).vdegreesnum; i++)
      fprintf(fp, "%3d %3d\n", (*(*graph).vdegreelist).degree[i],
              (*(*graph).vdegreelist).numofnodes[i]);

   fprintf(fp, "\n");
   fprintf(fp, "/* list of cnode degrees */\n");
   for (i = 0; i < (*graph).cdegreesnum; i++)
      fprintf(fp, "%3d %3d\n", (*(*graph).cdegreelist).degree[i],
              (*(*graph).cdegreelist).numofnodes[i]);

   fprintf(fp, "\n");
   fprintf(fp, "/*description of vnodes *\n");
   fprintf(fp, "* in the following format *\n");
   fprintf(fp, "* vnodedegree          *\n");
   fprintf(fp, "* tonode tosocket...  */\n");

   for (node = 0; node < (*graph).vnodenum; node++)
   {
      fprintf(fp, "%3d ", vnodedegree);
      for (socket = 0; socket < vnodedegree; socket++)
      {
         if (socket % 3 == 0 && socket > 0)
            fprintf(fp, "\n    ");

         fprintf(fp, "%7d ", vnodeedge.dest);
         fprintf(fp, "%7d  ", vnodeedge.socket);
      }
      fprintf(fp, "\n");
   }

   fprintf(fp, "\n");
   fprintf(fp, "/*description of cnodes *\n");
   fprintf(fp, "* in the following format *\n");
   fprintf(fp, "* cnodedegree          *\n");
   fprintf(fp, "* tonode tosocket...  */\n");

   for (node = 0; node < (*graph).cnodenum; node++)
   {
      fprintf(fp, "%3d ", cnodedegree);

      for (socket = 0; socket < cnodedegree; socket++)
      {
         if (socket % 3 == 0 && socket > 0)
            fprintf(fp, "\n    ");

         fprintf(fp, "%7d ", cnodeedge.dest);
         fprintf(fp, "%7d  ", cnodeedge.socket);
      }
      fprintf(fp, "\n");
   }

   fprintf(fp, "\n\n-1\n\n");

   // if decoder if known write 
   // gap and inverse of phi onto file as well

   fprintf(fp, "/* information for encoder *\n");
   fprintf(fp, "* gap [-1 indicates no info] *\n");
   fprintf(fp, "* inverse of phi row by row */\n");

   if ((*encoder).gap >= 0)
   {
      fprintf(fp, "%d\n", (*encoder).gap);
      for (row = 0; row < (*encoder).gap; row++)
      {
         for (col = 0; col < (*encoder).gap; col++)
         {
            fprintf(fp, "%ld ", (*encoder).iphi[row * (*encoder).gap + col]);
         }
         fprintf(fp, "\n");
      }
      fprintf(fp, "\n");
   }
   fprintf(fp, "-1\n");
   fclose(fp);
}


/**************************************************************************
 * read in a protograph from a file and convert it to the classical graph *
 * discription file                                                       *
 **************************************************************************/
void readprotograph(simulations * simulation)
{
   int node, vnodenum, cnodenum, vdegreesnum, cdegreesnum;

   int socket, tonode, tosocket;

   int row, col, temp;

   int verbose;

   graphs *graph;

   encoders *encoder;

   FILE *fpin;

   encoder = (encoders *) allocate(sizeof(encoders));
   verbose = (*simulation).verbose;

   // open the file containing protograph is at the same time outfile of this routine
   if ((fpin = fopen((*simulation).protofile, "r")) == NULL)
   {
      fprintf(stderr, "I can't open the indicated proto-file: %s.\n",(*simulation).protofile);
      return exit(1);
   }
   graph = (graphs *) allocate(sizeof(graphs));
   // read in main parameters
   skip(fpin, 0);
   skip(fpin, 0);

   fscanf(fpin, "%d", &(*graph).vnodenum);
   fscanf(fpin, "%d", &(*graph).cnodenum);
   (*graph).rate = 1.0 - ((double)(*graph).cnodenum) / ((double)(*graph).cnodenum);

   // allocate memory for nodes
   (*graph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
   (*graph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);

   // initialize check degree to zero
   for (node = 0; node < (*graph).cnodenum; node++)
      (*((*graph).cnodelist + node)).degree = 0;

   // start by reading in vnodes
   skip(fpin, 0);
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      // degree of the node
      fscanf(fpin, "%d", &vnodedegree);

      // allocate memory for edges

      vnodeedgelist = (edge *) allocate(sizeof(edge) * vnodedegree);
      for (socket = 0; socket < vnodedegree; socket++)
      {
         fscanf(fpin, "%d", &tonode);
         fscanf(fpin, "%d", &tosocket);

         vnodeedge.dest = tonode;

         // count degree of check nodes
         (*((*graph).cnodelist + vnodeedge.dest)).degree++;
         vnodeedge.socket = tosocket;
      }
   }

   // allocate memory for check node edges
   for (node = 0; node < (*graph).cnodenum; node++)
      cnodeedgelist = (edge *) allocate(sizeof(edge) * cnodedegree);

   // transform view from var nodes to check node edges
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      for (socket = 0; socket < vnodedegree; socket++)
      {
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).dest = node;
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).socket = socket;
      }
   }
   fclose(fpin);


   // copy graph to (*simulation).
	// protograph
   // 1.  allocate memory for protograph
   (*simulation).protograph = (graphs *) allocate(sizeof(graphs));

   (*(*simulation).protograph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
   (*(*simulation).protograph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);

   for (node = 0; node < (*graph).vnodenum; node++)
   {
      (*((*(*simulation).protograph).vnodelist + node)).edgelist =
         (edge *) allocate(sizeof(edge) * vnodedegree);
   }


   for (node = 0; node < (*graph).cnodenum; node++)
      (*((*(*simulation).protograph).cnodelist + node)).edgelist =
         (edge *) allocate(sizeof(edge) * cnodedegree);

   // 2. copy values
   (*(*simulation).protograph).vnodenum = (*graph).vnodenum;
   (*(*simulation).protograph).cnodenum = (*graph).cnodenum;

   for (node = 0; node < (*graph).vnodenum; node++)
   {
      (*((*(*simulation).protograph).vnodelist + node)).degree = vnodedegree;
      for (socket = 0; socket < vnodedegree; socket++)
      {
         (*((*((*(*simulation).protograph).vnodelist + node)).edgelist + socket)).dest =
            vnodeedge.dest;
         (*((*((*(*simulation).protograph).vnodelist + node)).edgelist + socket)).socket =
            vnodeedge.socket;
      }
   }

   for (node = 0; node < (*graph).cnodenum; node++)
   {
      (*((*(*simulation).protograph).cnodelist + node)).degree = cnodedegree;
      for (socket = 0; socket < cnodedegree; socket++)
      {
         (*((*((*(*simulation).protograph).cnodelist + node)).edgelist + socket)).dest
            = cnodeedge.dest;
         (*((*((*(*simulation).protograph).cnodelist + node)).edgelist + socket)).socket
            = cnodeedge.socket;
      }
   }
}


/*******************************************
 * read in a graph from a file             *
 ******************************************/
void readgraph(graphs * graph, encoders * encoder, char *graphfile, int verbose)
{
  int i, num, node, vnodenum, cnodenum, vdegreesnum, cdegreesnum;
  int vmaxdegree, cmaxdegree, degree, socket, tonode, tosocket;
  int row, col, temp;
  double rate;
  FILE *fp;

  // opening the file containing the graph
  if ((fp = fopen(graphfile, "r")) == NULL)
  {
     fprintf(stderr, "I can't open the indicated graph file: %s.\n",graphfile);
     return exit(1);
  }
  if (verbose)
  {
     printf("\n************************************\n");
     printf(" read %s file", graphfile);
     printf("\n************************************\n");
  }
  // read in main parameters
  skip(fp, 0);
  skip(fp, 0);
  fscanf(fp, "%d", &vnodenum);
  (*graph).vnodenum = vnodenum;
  if (0)
     printf("\n%d ", vnodenum);
  fscanf(fp, "%d", &cnodenum);
  (*graph).cnodenum = cnodenum;
  if (0)
     printf("%d", cnodenum);
  (*graph).rate = 1.0 - ((double)(*graph).cnodenum) / ((double)(*graph).vnodenum);

  skip(fp, 0);
  fscanf(fp, "%d", &vdegreesnum);      // is still zero for protographs
  (*graph).vdegreesnum = vdegreesnum;
  if (0)
     printf("\n%d ", vdegreesnum);
  fscanf(fp, "%d", &cdegreesnum);      // is still zero for protographs
  (*graph).cdegreesnum = cdegreesnum;
  if (0)
     printf("%d", cdegreesnum);

  skip(fp, 0);
  fscanf(fp, "\n%d", &vmaxdegree);
  (*graph).vmaxdegree = vmaxdegree;
  if (0)
     printf("%d", vmaxdegree);
  fscanf(fp, "%d", &cmaxdegree);
  (*graph).cmaxdegree = cmaxdegree;
  if (0)
     printf("%d", cmaxdegree);
  // allocate memory for nodes
  (*graph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
  (*graph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);

  // enter the degreelist
  (*graph).vdegreelist = (degreelist *) allocate(sizeof(degreelist));
  (*graph).cdegreelist = (degreelist *) allocate(sizeof(degreelist));


  // read in vnodelist
  skip(fp, 0);
  for (i = 0; i < (*graph).vdegreesnum; i++)
  {
     fscanf(fp, "%d", &degree);
     fscanf(fp, "%d", &num);
     (*(*graph).vdegreelist).degree[i] = degree;
     (*(*graph).vdegreelist).numofnodes[i] = num;
  }
  // read in cnodelist
  skip(fp, 0);
  for (i = 0; i < (*graph).cdegreesnum; i++)
  {
     fscanf(fp, "%d", &degree);
     fscanf(fp, "%d", &num);
     (*(*graph).cdegreelist).degree[i] = degree;
     (*(*graph).cdegreelist).numofnodes[i] = num;
  }
  // start by reading in vnodes
  skip(fp, 0);
  for (node = 0; node < (*graph).vnodenum; node++)
  {
     // degree
     fscanf(fp, "%d", &degree);
     vnodedegree = degree;
     // allocate memory for edges
     vnodeedgelist = (edge *) allocate(sizeof(edge) * degree);
     for (socket = 0; socket < vnodedegree; socket++)
     {
        fscanf(fp, "%d", &tonode);
        fscanf(fp, "%d", &tosocket);
        vnodeedge.dest = tonode;
        vnodeedge.socket = tosocket;
     }
  }
  // next read in cnodes
  skip(fp, 0);
  for (node = 0; node < (*graph).cnodenum; node++)
  {
     // degree
     fscanf(fp, "%d", &degree);
     cnodedegree = degree;
     // allocate memory for edges
     cnodeedgelist = (edge *) allocate(sizeof(edge) * degree);
     for (socket = 0; socket < cnodedegree; socket++)
     {
        fscanf(fp, "%d", &tonode);
        fscanf(fp, "%d", &tosocket);
        cnodeedge.dest = tonode;
        cnodeedge.socket = tosocket;
     }
  }
  // now read encoder information if available
  skip(fp, 0);
  fscanf(fp, "%d", &(*encoder).gap);
  if ((*encoder).gap >= 0)
  {
     // allocate memory
     (*encoder).iphi = (int *)allocate((*encoder).gap * (*encoder).gap * sizeof(int));

     for (row = 0; row < (*encoder).gap; row++)
        for (col = 0; col < (*encoder).gap; col++)
        {
           fscanf(fp, "%d", &temp);
           (*encoder).iphi[row * (*encoder).gap + col] = temp;
        }
  }
  fclose(fp);
}


/*********************************************
 * simplest constructor of a proto graph      *
 * no check for double edges etc is done      *
 *********************************************/
void constructprotograph(graphs * graph, simulations * simulation)
{
   int node, socket;
   int *perm, *iperm;
   int pvnodenum, pcnodenum;    // protograph node numbers
   int gvnodenum, gcnodenum;    // final graph node numbers
   int degree;
   int maxdegree, vdegree, cdegree;
   int copie, nodecounter, dest;
   double sumv;

   vnode pvnode;
   cnode pcnode;

   degreelist *vdegreelist, *cdegreelist;

   int flag, i, j, k;

   pvnodenum = (*(*simulation).protograph).vnodenum;
   pcnodenum = (*(*simulation).protograph).cnodenum;

   gvnodenum = pvnodenum * (*simulation).length;
   gcnodenum = pcnodenum * (*simulation).length;

   (*graph).vnodenum = gvnodenum;
   (*graph).cnodenum = gcnodenum;

   /* allocate memory for nodes */
   (*graph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
   (*graph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);

   /* compute graph parameters (maxdegrees, degreelist) */
   (*graph).vdegreelist = (degreelist *) allocate(sizeof(degreelist));
   (*graph).cdegreelist = (degreelist *) allocate(sizeof(degreelist));

   vdegreelist = (degreelist *) allocate(sizeof(degreelist));
   cdegreelist = (degreelist *) allocate(sizeof(degreelist));

   for (i = 0; i < MAXDEGREENUM; i++)
   {
      (*vdegreelist).numofnodes[i] = 0;
      (*cdegreelist).numofnodes[i] = 0;
   }

	
   maxdegree = 0;

   for (node = 0; node < pvnodenum; node++)
   {
      degree = (*((*(*simulation).protograph).vnodelist + node)).degree;
      (*vdegreelist).numofnodes[degree]++;

      if (degree > maxdegree)
         maxdegree = degree;
   }
   (*graph).vmaxdegree = maxdegree;
   maxdegree = 0;

   for (node = 0; node < (*(*simulation).protograph).cnodenum; node++)
   {
      degree = (*((*(*simulation).protograph).cnodelist + node)).degree;
      (*cdegreelist).numofnodes[degree]++;

      if (degree > maxdegree)
         maxdegree = degree;
   }
   (*graph).cmaxdegree = maxdegree;
   vdegree = 0;
   cdegree = 0;
   for (i = 0; i < MAXDEGREENUM; i++)
   {
      if ((*vdegreelist).numofnodes[i] > 0)
      {
         (*(*graph).vdegreelist).degree[vdegree] = i;
         (*(*graph).vdegreelist).numofnodes[vdegree] =
            (*vdegreelist).numofnodes[i] * (*simulation).length;
         vdegree++;
      }
      if ((*cdegreelist).numofnodes[i] > 0)
      {
         (*(*graph).cdegreelist).degree[cdegree] = i;
         (*(*graph).cdegreelist).numofnodes[cdegree] =
            (*cdegreelist).numofnodes[i] * (*simulation).length;
         cdegree++;
      }
   }

   (*graph).vdegreesnum = vdegree;
   (*graph).cdegreesnum = cdegree;

   // initialize permutation vectors for random assignment of check nodes 
   perm = (int *)allocate(sizeof(int) * (*simulation).length);
   iperm = (int *)allocate(sizeof(int) * (*simulation).length);

   // initialize vnodeedges of final graph 
   for (nodecounter = 0; nodecounter < pvnodenum; nodecounter++)
   {
      pvnode = (*((*(*simulation).protograph).vnodelist + nodecounter));
      degree = pvnode.degree;

      for (copie = 0; copie < (*simulation).length; copie++)
      {
         node = nodecounter * (*simulation).length + copie;
         vnodeedgelist = (edge *) allocate(sizeof(edge) * degree);
         vnodedegree = degree;
      }
   }

   // initialize cnodeedges of final graph 
   for (nodecounter = 0; nodecounter < pcnodenum; nodecounter++)
   {
      pcnode = (*((*(*simulation).protograph).cnodelist + nodecounter));
      degree = pcnode.degree;
      for (copie = 0; copie < (*simulation).length; copie++)
      {
         node = nodecounter * (*simulation).length + copie;
         cnodeedgelist = (edge *) allocate(sizeof(edge) * degree);
      }
   }

   // set all cnodedegrees to zero 
   for (node = 0; node < (*graph).cnodenum; node++)
      cnodedegree = 0;

   for (node = 0; node < (*simulation).length; node++)
   {
      *(perm + node) = node;
   }

   // copy pvnodes and their edges and assign randomly check nodes of one type */
   for (nodecounter = 0; nodecounter < pvnodenum; nodecounter++)
   {
      pvnode = (*((*(*simulation).protograph).vnodelist + nodecounter));
      for (socket = 0; socket < pvnode.degree; socket++)
      {
         dest = (*(pvnode.edgelist + socket)).dest;
         graphrandompermutation(perm, iperm, (*simulation).length);

         for (copie = 0; copie < (*simulation).length; copie++)
         {
            node = nodecounter * (*simulation).length + copie;
            vnodeedge.dest = dest * (*simulation).length + *(perm + copie);
            vnodeedge.socket = (*(pvnode.edgelist + socket)).socket;
            (*((*graph).cnodelist + vnodeedge.dest)).degree++;
         }
      }
   }

   // tell check nodes in final graph their neighbors 
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      for (socket = 0; socket < vnodedegree; socket++)
      {
         dest = vnodeedge.dest;
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).dest = node;
         (*((*((*graph).cnodelist + vnodeedge.dest)).edgelist + vnodeedge.socket)).socket = socket;
      }
   }

   // now free memory of tempory arrays
   free(vdegreelist);
   free(cdegreelist);
   free(perm);
   free(iperm);

}

/*********************************************
 * simplest constructor of an irregular graph *
 * no check for double edges etc is done      *
 *********************************************/
void constructirregulargraph(graphs * graph, simulations * simulation)
{
   int node, socket, tosocket, tonode;

   int *perm, *iperm;

   int graphcnodenum, degreesnum, conodenum, degree, degreecounter;

   int vedgecount, cedgecount, vedgecountnew, cedgecountnew, edgecounter;

   int *whichnode, *whichsocket;

   int start, degreetwocounter;

   double sumv, sumc, target;

   int *checknodes;

   int *varnode;

   int flag, i, j, k;

   /* variables which are hidden in structure simulation */
   int vnodenum, cnodenum, vdegreesnum, cdegreesnum, typedist, verbose;

   int *vdegreelist, *cdegreelist;

   double *vpercentagelist, *cpercentagelist;

   vnodenum = (*simulation).length;
   cnodenum = (*simulation).cnodenum;
   vdegreelist = (*simulation).vdegreelist;
   vpercentagelist = (*simulation).vpercentagelist;
   vdegreesnum = (*simulation).vdegreesnum;
   cdegreelist = (*simulation).cdegreelist;
   cpercentagelist = (*simulation).cpercentagelist;
   cdegreesnum = (*simulation).cdegreesnum;
   typedist = (*simulation).typedist;
   verbose = (*simulation).verbose;

   /* set number of degrees and max degrees */
   if (verbose > 0)
   {
      printf("\n");
      printf("Construction of (irregular) graph:\n");
      printf("1. Set number of degrees and max degrees.\n");
   }
   assert(vdegreesnum > 0 && cdegreesnum > 0);
   (*graph).vdegreesnum = vdegreesnum;
   (*graph).cdegreesnum = cdegreesnum;
   (*graph).vmaxdegree = *(vdegreelist + vdegreesnum - 1);
   (*graph).cmaxdegree = *(cdegreelist + cdegreesnum - 1);

   switch (typedist)
   {
   case 1:
      /* if edge degree distribution */
      /* take the percentage lists and convert them */
      /* into node perspective */
      if (verbose > 0)
         printf("2. Convert node percentages into node perspective.\n");

      sumv = 0;
      for (degreesnum = 0; degreesnum < vdegreesnum; degreesnum++)
         sumv += *(vpercentagelist + degreesnum) / ((double)*(vdegreelist + degreesnum));

      sumc = 0;
      for (degreesnum = 0; degreesnum < cdegreesnum; degreesnum++)
         sumc += *(cpercentagelist + degreesnum) / ((double)*(cdegreelist + degreesnum));

      /* approximate number of cnodes */
      graphcnodenum = (int)round(vnodenum * sumc / sumv);
      if (verbose)
         printf("graphcnodenum:=%d\n", graphcnodenum);

      /* enter the degreelist */
      (*graph).vdegreelist = (degreelist *) allocate(sizeof(degreelist));
      (*graph).cdegreelist = (degreelist *) allocate(sizeof(degreelist));

      if (verbose > 0)
         printf("3. Number of vnodes: [before adjustments]\n");
      for (degreesnum = vdegreesnum - 1; degreesnum >= 0; degreesnum--)
      {
         (*(*graph).vdegreelist).degree[degreesnum] = *(vdegreelist + degreesnum);
         (*(*graph).vdegreelist).numofnodes[degreesnum] =
            (int)round(vnodenum ** (vpercentagelist + degreesnum) /
                       (sumv ** (vdegreelist + degreesnum)));
         if (verbose > 0)
         {
            printf("degreesnum:=%d\n", degreesnum);
            printf("   vdegree:=%d vdegreenum:=%d\n", (*(*graph).vdegreelist).degree[degreesnum],
                   (*(*graph).vdegreelist).numofnodes[degreesnum]);
         }
      }

      if (verbose > 0)
         printf("4. Number of cnodes: [before adjustments]\n");
      for (degreesnum = cdegreesnum - 1; degreesnum >= 0; degreesnum--)
      {
         (*(*graph).cdegreelist).degree[degreesnum] = *(cdegreelist + degreesnum);
         (*(*graph).cdegreelist).numofnodes[degreesnum] =
            (int)round(graphcnodenum ** (cpercentagelist + degreesnum) /
                       (sumc ** (cdegreelist + degreesnum)));
         if (verbose > 0)
            printf("   cdegree:=%d cdegreenum:=%d\n", (*(*graph).cdegreelist).degree[degreesnum],
                   (*(*graph).cdegreelist).numofnodes[degreesnum]);
      }
      break;

   case 0:

      /* calculate number of check nodes */
      sumv = 0;
      for (degreesnum = 0; degreesnum < vdegreesnum; degreesnum++)
         sumv += *(vpercentagelist + degreesnum) * ((double)*(vdegreelist + degreesnum));

      sumc = 0;
      for (degreesnum = 0; degreesnum < cdegreesnum; degreesnum++)
         sumc += *(cpercentagelist + degreesnum) * ((double)*(cdegreelist + degreesnum));

      if (verbose)
         printf("2. Calculate number of check nodes.\n");

      graphcnodenum = (int)round((sumv / sumc) * vnodenum);

      if (verbose)
         printf("graphcnodenum=%d\n", graphcnodenum);

      /* enter the degreelist */
      (*graph).vdegreelist = (degreelist *) allocate(sizeof(degreelist));
      (*graph).cdegreelist = (degreelist *) allocate(sizeof(degreelist));

      if (verbose > 0)
         printf("3. Number of vnodes: [before adjustments]\n");
      for (degreesnum = vdegreesnum - 1; degreesnum >= 0; degreesnum--)
      {
         (*(*graph).vdegreelist).degree[degreesnum] = *(vdegreelist + degreesnum);
         (*(*graph).vdegreelist).numofnodes[degreesnum] =
            (int)round(vnodenum ** (vpercentagelist + degreesnum));
         if (verbose > 0)
         {
            printf("degreesnum:=%d\n", degreesnum);
            printf("   vdegree:=%d vdegreenum:=%d\n", (*(*graph).vdegreelist).degree[degreesnum],
                   (*(*graph).vdegreelist).numofnodes[degreesnum]);
         }
      }

      if (verbose > 0)
         printf("4. Number of cnodes: [before adjustments]\n");
      for (degreesnum = cdegreesnum - 1; degreesnum >= 0; degreesnum--)
      {
         (*(*graph).cdegreelist).degree[degreesnum] = *(cdegreelist + degreesnum);
         (*(*graph).cdegreelist).numofnodes[degreesnum] =
            (int)round(graphcnodenum ** (cpercentagelist + degreesnum));
         if (verbose > 0)
            printf("   cdegree:=%d cdegreenum:=%d\n", (*(*graph).cdegreelist).degree[degreesnum],
                   (*(*graph).cdegreelist).numofnodes[degreesnum]);
      }
      break;

   default:
      printf("\nyou should never end here--graph\n");
      exit(1);
      break;
   }

   /* make sure that the edgecounts match up */
   if (verbose > 0)
      printf("5. See if edgecounts match up.\n");
   vedgecount = 0;
   for (degreesnum = 0; degreesnum < vdegreesnum; degreesnum++)
      vedgecount +=
         (*(*graph).vdegreelist).numofnodes[degreesnum] *
         (*(*graph).vdegreelist).degree[degreesnum];
   cedgecount = 0;
   for (degreesnum = 0; degreesnum < cdegreesnum; degreesnum++)
      cedgecount +=
         (*(*graph).cdegreelist).numofnodes[degreesnum] *
         (*(*graph).cdegreelist).degree[degreesnum];
   if (verbose > 0)
   {
      printf("   Preliminary counts:\n");
      printf("   vedgecount:=%d cedgecount:=%d\n", vedgecount, cedgecount);
   }

   /* make sure that cedgecount >= vedgecount */
   if (cedgecount < vedgecount && verbose > 0)
      printf("   Need to increase cedgecount.\n");
   while (cedgecount < vedgecount)
   {
      /* remove one cnode of smallest degree */
      degree = 0;
      while ((*(*graph).cdegreelist).numofnodes[degree] <= 0)
         degree++;

      if (cdegreesnum == 1)     /* if only 1 right degree, add another one */
      {
         cdegreesnum++;
         (*graph).cdegreesnum = cdegreesnum;
         (*(*graph).cdegreelist).degree[1] = (*(cdegreelist)) + 1;
         (*(*graph).cdegreelist).numofnodes[1] = 0;
      }


      (*(*graph).cdegreelist).numofnodes[degree]--;
      (*(*graph).cdegreelist).numofnodes[degree + 1]++;
      cedgecount +=
         ((*(*graph).cdegreelist).degree[degree + 1] - (*(*graph).cdegreelist).degree[degree]);
   }
   assert(cedgecount >= vedgecount);

   /* make adjustments if necessary */
   if (vedgecount != cedgecount)
   {
      if (verbose > 0)
         printf("   Adjustments of edgecounts.\n");

      if ((vedgecount - cedgecount) % 2 != 0)
      {
         /* we need degree 3 nodes for the adjustment */
         assert((*(*graph).vdegreelist).degree[1] == 3);

         /* add a degree three node */
         assert((*(*graph).vdegreelist).degree[1] == 3);
         (*(*graph).vdegreelist).numofnodes[1]++;
         vedgecount += (*(*graph).vdegreelist).degree[1];
      }

      /* make up the rest with degree 2 nodes */
      assert((*(*graph).vdegreelist).degree[0] == 2);
      (*(*graph).vdegreelist).numofnodes[0] += (cedgecount - vedgecount) / 2;
      vedgecount = cedgecount;
   }

   /* count them again to be sure */
   if (verbose > 0)
      printf("   Check if edgecounts match after correction.\n");
   vedgecountnew = 0;
   for (degreesnum = 0; degreesnum < vdegreesnum; degreesnum++)
      vedgecountnew +=
         (*(*graph).vdegreelist).numofnodes[degreesnum] *
         (*(*graph).vdegreelist).degree[degreesnum];
   assert(vedgecountnew == vedgecount);

   cedgecountnew = 0;
   for (degreesnum = 0; degreesnum < cdegreesnum; degreesnum++)
      cedgecountnew +=
         (*(*graph).cdegreelist).numofnodes[degreesnum] *
         (*(*graph).cdegreelist).degree[degreesnum];
   assert(cedgecountnew == cedgecount);

   /* determine the true vnodenum and graphcnodenum */
   vnodenum = 0;
   for (degreesnum = 0; degreesnum < vdegreesnum; degreesnum++)
      vnodenum += (*(*graph).vdegreelist).numofnodes[degreesnum];
   graphcnodenum = 0;
   for (degreesnum = 0; degreesnum < cdegreesnum; degreesnum++)
      graphcnodenum += (*(*graph).cdegreelist).numofnodes[degreesnum];

   /* enter them into graph structure */
   (*graph).vnodenum = vnodenum;
   (*graph).cnodenum = graphcnodenum;

   if (verbose > 0)
   {
      printf("6. True node numbers:\n");
      printf("   vnodenum:=%d cnodenum:=%d\n", vnodenum, graphcnodenum);
   }
   /* now the true rate is determined */
   (*graph).rate = 1.0 - ((double)(*graph).cnodenum) / ((double)(*graph).vnodenum);

   /* allocate memory for nodes */
   (*graph).vnodelist = (vnode *) allocate(sizeof(vnode) * (*graph).vnodenum);
   (*graph).cnodelist = (cnode *) allocate(sizeof(cnode) * (*graph).cnodenum);


   /* set the degree of each node */
   degree = 0;
   while ((*(*graph).vdegreelist).numofnodes[degree] <= 0)
      degree++;
   degreecounter = (*(*graph).vdegreelist).numofnodes[degree];
   assert(degreecounter > 0);

   for (node = 0; node < (*graph).vnodenum; node++)
   {
      vnodedegree = (*(*graph).vdegreelist).degree[degree];
      degreecounter--;
      assert(degreecounter >= 0);

      if ((degreecounter == 0) && (degree < vdegreesnum - 1))
      {
         degree++;
         while (((*(*graph).vdegreelist).numofnodes[degree] <= 0) && (degree < vdegreesnum))
            degree++;
         assert(degree <= vdegreesnum);

         degreecounter = (*(*graph).vdegreelist).numofnodes[degree];
         assert(degreecounter >= 0);
      }
   }
   degree = 0;
   while ((*(*graph).cdegreelist).numofnodes[degree] <= 0)
      degree++;
   degreecounter = (*(*graph).cdegreelist).numofnodes[degree];
   assert(degreecounter > 0);

   for (node = 0; node < (*graph).cnodenum; node++)
   {
      cnodedegree = (*(*graph).cdegreelist).degree[degree];
      degreecounter--;
      assert(degreecounter >= 0);

      if ((degreecounter == 0) && (degree < cdegreesnum - 1))
      {
         degree++;
         while (((*(*graph).cdegreelist).numofnodes[degree] <= 0) && (degree < cdegreesnum))
            degree++;
         assert(degree <= cdegreesnum);

         degreecounter = (*(*graph).cdegreelist).numofnodes[degree];
         assert(degreecounter >= 0);
      }
   }

   /* count the edges again to make sure no mistakes were made */
   vedgecountnew = 0;
   for (node = 0; node < (*graph).vnodenum; node++)
      vedgecountnew += vnodedegree;
   assert(vedgecountnew == vedgecount);

   cedgecountnew = 0;
   for (node = 0; node < (*graph).cnodenum; node++)
      cedgecountnew += cnodedegree;
   assert(cedgecountnew == cedgecount);

   /* now allocate memory for edges */
   for (node = 0; node < (*graph).vnodenum; node++)
      vnodeedgelist = (edge *) allocate(sizeof(edge) * vnodedegree);
   for (node = 0; node < (*graph).cnodenum; node++)
      cnodeedgelist = (edge *) allocate(sizeof(edge) * cnodedegree);
   // generate random permutation which determines graph 
   perm = (int *)allocate(sizeof(int) * vedgecount);
   iperm = (int *)allocate(sizeof(int) * vedgecount);
   graphrandompermutation(perm, iperm, vedgecount);

   /* allocate temporary memory */
   whichnode = (int *)allocate(sizeof(int) * vedgecount);
   whichsocket = (int *)allocate(sizeof(int) * vedgecount);

   /* fill in whichnode and whichsocket */
   edgecounter = 0;
   for (node = 0; node < (*graph).cnodenum; node++)
   {
      for (socket = 0; socket < cnodedegree; socket++)
      {
         *(whichnode + edgecounter) = node;
         *(whichsocket + edgecounter) = socket;
         edgecounter++;
      }
   }
   edgecounter = 0;
   for (node = 0; node < (*graph).vnodenum; node++)
   {
      for (socket = 0; socket < vnodedegree; socket++)
      {
         tonode = *(whichnode + *(perm + edgecounter));
         tosocket = *(whichsocket + *(perm + edgecounter));

         vnodeedge.dest = tonode;
         vnodeedge.socket = tosocket;
         cnodetoedge.dest = node;
         cnodetoedge.socket = socket;

         edgecounter++;
      }
   }

   /* free temporary memory */
   free(perm);
   free(iperm);
   free(whichnode);
   free(whichsocket);
}


/* construct graph using degree distribution or given graphfile */
void constructgraph(simulations * simulation, graphs * graph,
                    randomizes * randomize, encoders * encoder, reruns * rerun, int loop, int l)
{
  // if graph is given in file and graphstructure has not been 
  // created yet then read it in 
  if ((*simulation).newgraph == 0)
  {
     if ((*graph).vnodenum == -1)
     {
        readgraph(graph, encoder, (*simulation).graphfile, (*simulation).verbose);
     }
  }

  /* if the graph is not given then construct it */
  if ((*simulation).newgraph == 1)     // only degree distribution is given
  {
     /* if this is before the first loop we have to construct a graph for sure */
     if ((*graph).vnodenum == -1)
     {
        setgraphseeds(loop + 1, rerun);
        getgraphsimrand(randomize);
        // init random generator only in the very beginning
        // initrandom((*randomize).graphseed); replaced by 'graphsimrand'
        switch ((*simulation).ensemble)
        {
        case 0:               // standard ensemble
           constructirregulargraph(graph, simulation);
           // indicates that encoder has not been built yet
           (*encoder).gap = -1;
           // 
           break;
        case 1:               // protograph ensemble
           if ((*graph).vnodenum == -1)
              readprotograph(simulation);
           constructprotograph(graph, simulation);
           // indicates that encoder has not been built yet
           (*encoder).gap = -1;
           // 
           break;
        default:
           printf("\n this enemble has not yet been implemented!\n");
           exit(1);
           break;
        }
        if ((*simulation).constructgraphonce == 1)
           savegraph(graph, encoder, (*simulation).graphfile, (*simulation).verbose);
     }

     /* if we are in the middle of the simulation */
     /* we only have to construct a graph if we want a new one for each codeword */
     if ((loop > 0 && l == 0) || (l > 0))
     {
        if ((*simulation).constructgraphonce == 1)
        {
           // there is nothing to do
        }
        if ((*simulation).constructgraphonce == 0)
        {
           setgraphseeds(loop, rerun);
           getgraphsimrand(randomize);
           freegraph(graph);
           switch ((*simulation).ensemble)
           {
           case 0:
              constructirregulargraph(graph, simulation);
              break;
           case 1:
              constructprotograph(graph, simulation);
              break;
           default:
              printf("\nensemble is not yet implemented\n");
              exit(1);
              break;
           }
        }
     }
  }
}


/***************************
 * free memory for graph   *
 **************************/
void freegraph(graphs * graph)
{
   int node;

   /* now allocate memory for edges */
   for (node = 0; node < (*graph).vnodenum; node++)
      free(vnodeedgelist);
   for (node = 0; node < (*graph).cnodenum; node++)
      free(cnodeedgelist);

   /* allocate memory for nodes */
   free((*graph).vnodelist);
   free((*graph).cnodelist);

   /* enter the degreelist */
   free((*graph).vdegreelist);
   free((*graph).cdegreelist);
}

/*******************
 * switch two edges *
 *******************/
void switchedges(graphs * graph, int nodea, int socketa, int nodeb, int socketb)
{
   int node, socket, tonodea, tosocketa, tonodeb, tosocketb;

   /* find right hand side of edge a */
   node = nodea;
   socket = socketa;
   tonodea = vnodeedge.dest;
   tosocketa = vnodeedge.socket;

   /* find right hand side of edge b */
   node = nodeb;
   socket = socketb;
   tonodeb = vnodeedge.dest;
   tosocketb = vnodeedge.socket;

   /* now switch */
   /* left hand side of edge a */
   node = nodea;
   socket = socketa;
   vnodeedge.dest = tonodeb;
   vnodeedge.socket = tosocketb;

   /* right hand side of edge a */
   node = tonodeb;
   socket = tosocketb;
   cnodeedge.dest = nodea;
   cnodeedge.socket = socketa;

   /* left hand side of edge b */
   node = nodeb;
   socket = socketb;
   vnodeedge.dest = tonodea;
   vnodeedge.socket = tosocketa;

   /* right hand side of edge b */
   node = tonodea;
   socket = tosocketa;
   cnodeedge.dest = nodeb;
   cnodeedge.socket = socketb;
}

/********************
 * switch two vnodes *
 ********************/
void switchvnodes(graphs * graph, int nodea, int nodeb)
{
}

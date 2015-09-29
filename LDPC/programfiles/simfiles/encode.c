#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "graphstruct.h"
#include "simulation.h"
#include "graphfunct.h"
#include "encode.h"

/* global variables */
extern unsigned short channelsimrand[3];

/*********************************
* perform upper triangualation   *
*********************************/
void uppertriangulation(graphs * graph, int *gap, int verbose)
{
   int vnodenum, cnodenum;

   int currentvnode, currentleftcnode, currentrightcnode;

   int node, socket, newvnode, newcnode, tonode, tosocket;

   if (verbose)
      printf("upper triangulation ...\n");
   /* initialize gap */
   *gap = 0;

   /* we need to make sure that graph does not */
   /* contain double edges */
   if (verbose > 0)
      printhmatrixtall(graph);
   removedoubleoverlaps(graph, 0);


   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   currentvnode = 0;
   currentleftcnode = 0;
   currentrightcnode = (cnodenum - 1);

   /* use the "value" field temporarily */
   /* to indicate the residual degree */
   for (node = 0; node < vnodenum; node++)
      vnodevalue = (double)vnodedegree;

   while (currentleftcnode <= currentrightcnode)
   {
      if (verbose > 0)
      {
         printgraph(graph, 7);
      }

      /* find next vnode for diagonal extension */
      newvnode = findsmallestdegreevnode(graph, currentvnode);
      if (verbose)
         printf("newvnode:=%d\n", newvnode);

      /* switch positions of newvnode and currenvnode */
      if (verbose)
         printf("switch position of newvnode\n");
      switchvnode(graph, currentvnode, newvnode);
      if (verbose)
         printgraph(graph, 7);

      /* now take action depending on the */
      /* residual degree of this vnode */
      node = currentvnode;
      switch ((int)vnodevalue)
      {
      case 1:
         if (verbose)
         {
            printf("we are in the case of a degree one node\n");
            printf("this node has residual degree %g\n", vnodevalue);
         }

         /* find a cnode between currentleftcnode */
         /* and currentrightcnode */
         newcnode = -1;
         for (socket = 0; socket < vnodedegree; socket++)
         {
            if ((vnodeedge.dest >= currentleftcnode) && (vnodeedge.dest <= currentrightcnode))
            {
               newcnode = vnodeedge.dest;
               break;
            }
         }
         assert(newcnode >= 0);
         if (verbose)
            printf("newcnode:=%d\n", newcnode);

         /* decrease residual degrees for all */
         /* variable nodes connected to this cnode */
         if (verbose)
            printf("decrease degrees of all connected variable nodes\n");
         tonode = newcnode;
         for (tosocket = 0; tosocket < ctonodedegree; tosocket++)
            (*((*graph).vnodelist + (cnodetoedge.dest))).value -= 1;

         /* now switch position of newcnode and update counters */
         if (verbose)
            printf("switch position of newcnode\n");
         switchcnode(graph, currentleftcnode, newcnode);
         currentvnode++;
         currentleftcnode++;
         break;
      default:
         /* make sure residual degree */
         /* is at least two */
         if (vnodevalue <= 1)
            printf("vnodevalue:=%d\n", vnodevalue);
         assert(vnodevalue > 1);

         if (verbose)
         {
            printf("we are in the case of a higher degree node\n");
            printf("this node has residual degree %g\n", vnodevalue);
         }

         /* adjust gap */
         (*gap) += (vnodevalue - 1);

         /* find a cnode between currentleftcnode */
         /* and currentrightcnode */
         newcnode = -1;
         for (socket = 0; socket < vnodedegree; socket++)
         {
            if ((vnodeedge.dest >= currentleftcnode) && (vnodeedge.dest <= currentrightcnode))
            {
               newcnode = vnodeedge.dest;
               break;
            }
         }
         assert(newcnode >= 0);
         if (verbose)
            printf("the chosen cnode is %d\n", newcnode);

         /* decrease residual degrees for all */
         /* variable nodes connected to this cnode */
         if (verbose)
            printf("decrease degrees of all connected variable nodes\n");
         tonode = newcnode;
         for (tosocket = 0; tosocket < ctonodedegree; tosocket++)
            (*((*graph).vnodelist + (cnodetoedge.dest))).value -= 1;

         /* now switch position of newcnode and update counters */
         if (verbose)
            printf("switch position of cnode\n");
         switchcnode(graph, currentleftcnode, newcnode);
         if (verbose)
         {
            printgraph(graph, 7);
         }
         currentvnode++;
         currentleftcnode++;

         /* finally declare all remaining checks which are */
         /* connected to this vnode to be known */
         if (verbose)
            printf("declare all other check nodes to be known\n");

         while (socket < vnodedegree)
         {
            if ((vnodeedge.dest >= currentleftcnode) && (vnodeedge.dest <= currentrightcnode))
            {
               tonode = vnodeedge.dest;
               if (verbose)
                  printf("declare node %d to be known\n", tonode);

               if (verbose)
                  printhmatrixtall(graph);
               /* decrease degree of connected variable nodes */
               if (verbose)
                  printf("decrease degrees\n");
               for (tosocket = 0; tosocket < ctonodedegree; tosocket++)
                  (*((*graph).vnodelist + (cnodetoedge.dest))).value -= 1;

               if (verbose)
                  printf("switch cnodes currentrightcnode %d and tonode %d\n",
                         currentrightcnode, tonode);
               switchcnode(graph, currentrightcnode, tonode);
               if (verbose)
                  printhmatrixtall(graph);
               currentrightcnode--;
            }

            socket++;
         }
         break;
      }
   }

   /* rearrange the matrix to fit paper */
   tonode = cnodenum - *gap - 1;
   for (node = 0; node < (cnodenum - *gap) / 2; node++)
   {
      switchcnode(graph, node, tonode);
      tonode--;
   }
   tonode = vnodenum - 1;
   for (node = 0; node < vnodenum / 2; node++)
   {
      switchvnode(graph, node, tonode);
      tonode--;
   }

   if (verbose)
   {
      printhmatrixtall(graph);
      printhmatrix(graph);
   }
}

/*********************************
* among all remaining vnodes     *
* find one with residual degree  *
* one or otherwise               *
* the one with smallest residual *
* degree.                        * 
*********************************/
int findsmallestdegreevnode(graphs * graph, int currentvnode)
{
   int node, smallestcurrentdegree, bestcurrentnode;

   node = currentvnode;
   smallestcurrentdegree = vnodevalue;
   if (smallestcurrentdegree == 0)
      smallestcurrentdegree = (*graph).cnodenum + 1;
   bestcurrentnode = currentvnode;
   for (node = currentvnode + 1; node < (*graph).vnodenum; node++)
   {
      /* if we find a degree one */
      /* node then stop searching and */
      /* return its index */
      if (vnodevalue == 1)
         return node;

      if (vnodevalue < smallestcurrentdegree && vnodevalue > 0)
      {
         smallestcurrentdegree = vnodevalue;
         bestcurrentnode = node;
      }
   }

   return bestcurrentnode;
}

/*******************************
* switch two vnodes in a graph *
*******************************/
void switchvnode(graphs * graph, int nodea, int nodeb)
{
   int node, socket, tonode, tosocket;

   int tempdegree;

   double tempvalue;

   edge *tempedgelist;

   /* check if there is anything to be done */
   if (nodea == nodeb)
      return;

   /* first update cnodes */
   node = nodea;
   for (socket = 0; socket < vnodedegree; socket++)
   {
      /* find out cnode which this edge is connected to */
      tonode = vnodeedge.dest;
      tosocket = vnodeedge.socket;

      cnodetoedge.dest = nodeb;
   }
   node = nodeb;
   for (socket = 0; socket < vnodedegree; socket++)
   {
      /* find out cnode which this edge is connected to */
      tonode = vnodeedge.dest;
      tosocket = vnodeedge.socket;

      cnodetoedge.dest = nodea;
   }

   /* now exchange the two vnodes */
   node = nodea;
   tonode = nodeb;

   tempdegree = vnodedegree;
   tempvalue = vnodevalue;
   tempedgelist = vnodeedgelist;
   vnodedegree = vtonodedegree;
   vnodevalue = vtonodevalue;
   vnodeedgelist = vtonodeedgelist;
   vtonodedegree = tempdegree;
   vtonodevalue = tempvalue;
   vtonodeedgelist = tempedgelist;
}


/*******************************
* switch two cnodes in a graph *
*******************************/
void switchcnode(graphs * graph, int nodea, int nodeb)
{
   int node, socket, tonode, tosocket;

   int tempdegree;

   edge *tempedgelist;

   /* check if there is anything to be done */
   if (nodea == nodeb)
      return;

   /* first update vnodes */
   node = nodea;
   for (socket = 0; socket < cnodedegree; socket++)
   {
      /* find out vnode which this edge is connected to */
      tonode = cnodeedge.dest;
      tosocket = cnodeedge.socket;

      vnodetoedge.dest = nodeb;
   }
   node = nodeb;
   for (socket = 0; socket < cnodedegree; socket++)
   {
      /* find out vnode which this edge is connected to */
      tonode = cnodeedge.dest;
      tosocket = cnodeedge.socket;

      vnodetoedge.dest = nodea;
   }

   /* now exchange the two cnodes */
   node = nodea;
   tonode = nodeb;

   tempdegree = cnodedegree;
   tempedgelist = cnodeedgelist;
   cnodedegree = ctonodedegree;
   cnodeedgelist = ctonodeedgelist;
   ctonodedegree = tempdegree;
   ctonodeedgelist = tempedgelist;
}


/******************************************
* try to get rid of double overlaps       *
******************************************/
void removedoubleoverlaps(graphs * graph, int verbose)
{
   int i, node, socket, othersocket, cyclelength;

   int *shortcyclelist, shortcyclenum;

   /* allocate memory */
   shortcyclelist = (int *)allocate(sizeof(int) * (*graph).vnodenum * 100);

   shortcyclenum = 1;
   while (shortcyclenum > 0)
   {
      shortcyclenum = 0;

      /* get list of double overlaps */
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         for (socket = 0; socket < vnodedegree; socket++)
         {
            for (othersocket = socket + 1; othersocket < vnodedegree; othersocket++)
            {
               if (vnodeedge.dest ==
                   (*((*((*graph).vnodelist + node)).edgelist + othersocket)).dest)
               {
                  *(shortcyclelist + 5 * shortcyclenum + 0) = node;
                  *(shortcyclelist + 5 * shortcyclenum + 1) = socket;
                  *(shortcyclelist + 5 * shortcyclenum + 3) = vnodeedge.dest;
                  shortcyclenum++;
               }
            }
         }
      }

      if (verbose)
      {
         printf("\n");
         printf("%5d cycles of short weight and length:\n", shortcyclenum);
         printf("****************************\n");
         for (i = 0; i < shortcyclenum; i++)
            printf("%6d - %6d - %6d - %6d - %6d\n",
                   *(shortcyclelist + 5 * i + 0), *(shortcyclelist + 5 * i + 2),
                   *(shortcyclelist + 5 * i + 3), *(shortcyclelist + 5 * i + 4),
                   *(shortcyclelist + 5 * i + 1));
      }

      /* try to remove these connections */
      for (i = 0; i < shortcyclenum; i++)
      {
         node = (int)floor(drand48() * (*graph).vnodenum);
         socket = (int)floor(drand48() * vnodedegree);
         if (verbose)
            printf("nodea:=%d socketa:=%d -- nodeb:=%d socketb:=%d\n",
                   *(shortcyclelist + 5 * i + 0), *(shortcyclelist + 5 * i + 1), node, socket);

         /* now switch the bad edge with the random edge */
         switchedges(graph, *(shortcyclelist + 5 * i + 0), *(shortcyclelist + 5 * i + 1), node,
                     socket);
      }

   }

   /* free memory */
   free(shortcyclelist);
}

/**************************
* NOTE: the point         *
* (0,0) is in the upper   *
* left corner             *
**************************/
void printhmatrixtall(graphs * graph)
{
   int node, socket, tonode;

   int vnodenum, cnodenum;

   int *hcolumn;

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   /* allocate memory for one column */
   /* of the parity check matrix */
   hcolumn = (int *)allocate(cnodenum * sizeof(int));

   printf("H matrix [(0, 0) is on upper left corner]:\n");
   /* go over all vnodes */
   for (node = 0; node < vnodenum; node++)
   {
      /* clear field */
      for (tonode = 0; tonode < cnodenum; tonode++)
         hcolumn[tonode] = 0;

      /* determine connections */
      for (socket = 0; socket < vnodedegree; socket++)
         hcolumn[vnodeedge.dest]++;

      for (tonode = 0; tonode < cnodenum; tonode++)
         printf("%1d", hcolumn[tonode]);
      printf("\n");
   }

   printf("\n");
}

/***************************
* NOTE: the point          *
* (0,0) is in the upper    *
* left corner              *
***************************/
void printhmatrix(graphs * graph)
{
   int node, socket, tonode;

   int vnodenum, cnodenum;

   int *hrow;

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   /* allocate memory for row */
   /* of the parity check matrix */
   hrow = (int *)allocate(vnodenum * sizeof(int));

   printf("H matrix:\n");
   /* go over all cnode */
   for (node = 0; node < cnodenum; node++)
   {
      /* clear field */
      for (tonode = 0; tonode < vnodenum; tonode++)
         hrow[tonode] = 0;

      /* determine connections */
      for (socket = 0; socket < cnodedegree; socket++)
         hrow[cnodeedge.dest]++;

      for (tonode = 0; tonode < vnodenum; tonode++)
         printf("%1d", hrow[tonode]);
      printf("\n");
   }

   printf("\n");
}

/***************************
* NOTE: the point          *
* (0,0) is in the upper    *
* left corner              *
***************************/
void plotshapeofhmatrix(graphs * graph)
{
   int node, socket, tonode;

   int vnodenum, cnodenum;

   int *hrow;

   FILE *fp;

   if ((fp = fopen("hmatrix.pbm", "w")) == NULL)
   {
      fprintf(stderr, "I can't open file 'hmatrix.pbm'.\n");
      return exit(1);
   }

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   fprintf(fp, "P1\n");
   fprintf(fp, "%d %d\n", vnodenum, cnodenum);

   /* write top of frame * /* for (node=0; node<cnodenum+4; node++) fprintf(fp, "1 "); fprintf(fp, 
      "\n"); for (node=0; node<cnodenum+4; node++) fprintf(fp, "1 "); fprintf(fp, "\n"); */

   /* allocate memory for row */
   /* of the parity check matrix */
   hrow = (int *)allocate(vnodenum * sizeof(int));

   /* go over all cnode */
   for (node = 0; node < cnodenum; node++)
   {
      /* clear field */
      for (tonode = 0; tonode < vnodenum; tonode++)
         hrow[tonode] = 0;

      /* determine connections */
      for (socket = 0; socket < cnodedegree; socket++)
         hrow[cnodeedge.dest]++;

      /* fprintf(fp, "1 1 "); */
      for (tonode = 0; tonode < vnodenum; tonode++)
      {
         if (hrow[tonode] > 0)
            fprintf(fp, "1 ");
         else
            fprintf(fp, "0 ");
      }
      /* fprintf(fp, "1 1"); */
      fprintf(fp, "\n");
   }

   /* write bottom of frame */
   /* for (node=0; node<cnodenum+4; node++) fprintf(fp, "1 "); fprintf(fp, "\n"); for (node=0;
      node<cnodenum+4; node++) fprintf(fp, "1 "); */
   fprintf(fp, "\n");

   fclose(fp);
}

/***************************
/* compute inverse of phi  *
***************************/
// void determineiphi(graphs *graph, int gap, char *graphfile, int verbose)
void determineiphi(graphs * graph, encoders * encoder, char *graphfile, int verbose)
{
   int vnodenum, cnodenum;

   int row, col, node, socket;

   int temp, k;

   int *matrix, *extendedphi, *phi, *iphi;

   int gap;

   // copy encoders gap to internal gap for better readibility
   gap = (*encoder).gap;

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   /* allocate memory */
   matrix = (int *)allocate(vnodenum * gap * sizeof(int));
   extendedphi = (int *)allocate(vnodenum * gap * sizeof(int));
   phi = (int *)allocate(gap * gap * sizeof(int));
   iphi = (int *)allocate(gap * gap * sizeof(int));

   /* clear matrix */
   for (row = 0; row < gap; row++)
      for (col = 0; col < vnodenum; col++)
         *(matrix + row * vnodenum + col) = 0;

   /* fill in the elements of the matrix */
   if (verbose)
      printf("copy the last %d rows of H into matrix\n", gap);
   for (node = cnodenum - gap; node < cnodenum; node++)
      for (socket = 0; socket < cnodedegree; socket++)
         *(matrix + (node - cnodenum + gap) * vnodenum + cnodeedge.dest) = 1;

   if (verbose)
   {
      printhmatrix(graph);
      printmatrix(matrix, gap, vnodenum);
   }

   /* perform gaussian elimination on the last gap rows */
   /* of the H matrix to clear E */
   if (verbose)
      printf("do gaussian elimination\n");
   node = cnodenum - gap - 1;
   for (col = vnodenum - 1; col > vnodenum - 1 - cnodenum + gap; col--)
   {
      for (row = 0; row < gap; row++)
      {
         if (*(matrix + row * vnodenum + col) == 1)
         {
            /* add the appropriate row of T */
            for (socket = 0; socket < cnodedegree; socket++)
               *(matrix + row * vnodenum + cnodeedge.dest) ^= 1;
         }

         if (verbose)
         {
            printhmatrix(graph);
            printmatrix(matrix, gap, vnodenum);
         }
      }

      node--;
   }

   /* next we need ensure that the submatrix */
   /* phi is not singular */
   /* first copy matrix into extended phi */
   for (row = 0; row < gap; row++)
      for (col = 0; col < vnodenum; col++)
         *(extendedphi + row * vnodenum + col) = *(matrix + row * vnodenum + col);

   row = -1;
   for (col = vnodenum - cnodenum + gap - 1; col > vnodenum - cnodenum - 1; col--)
   {
      row++;

      /* find a non zero entry */
      node = col;
      while (*(matrix + row * vnodenum + node) != 1)
      {
         node--;

         if (node < 0)
         {
            printf("H matrix is rank deficient!\n");
            exit(0);
         }
      }

      /* switch columns */
      switchvnode(graph, col, node);
      for (k = 0; k < gap; k++)
      {
         temp = *(matrix + k * vnodenum + node);
         *(matrix + k * vnodenum + node) = *(matrix + k * vnodenum + col);
         *(matrix + k * vnodenum + col) = temp;
      }
      for (k = 0; k < gap; k++)
      {
         temp = *(extendedphi + k * vnodenum + node);
         *(extendedphi + k * vnodenum + node) = *(extendedphi + k * vnodenum + col);
         *(extendedphi + k * vnodenum + col) = temp;
      }
      if (verbose)
      {
         printf("after switch\n");
         printmatrix(matrix, gap, vnodenum);
      }

      /* clear this column */
      for (k = row + 1; k < gap; k++)
      {
         if (*(matrix + k * vnodenum + col) == 1)
         {
            for (node = col; node >= 0; node--)
               *(matrix + k * vnodenum + node) ^= *(matrix + row * vnodenum + node);
         }
      }

      if (verbose)
      {
         printf("after addition\n");
         printmatrix(matrix, gap, vnodenum);
      }
   }

   /* we now know that phi is nonsingular */
   for (row = 0; row < gap; row++)
      for (col = vnodenum - cnodenum; col < vnodenum - cnodenum + gap; col++)
         *(phi + row * gap + col - vnodenum + cnodenum) = *(extendedphi + row * vnodenum + col);

   if (verbose)
   {
      printf("phi matrix\n");
      printmatrix(phi, gap, gap);
   }

   /* finally we need to determine the inverse of phi */
   inversematrix(phi, iphi, gap);

   /* store the inverse etc in the file */
   (*encoder).iphi = iphi;

   (*encoder).gap = gap;
   savegraph(graph, encoder, graphfile, 0);

   free(matrix);
   free(extendedphi);
   free(phi);
   free(iphi);
}

/************************
* lu decomposition of a *
* binary matrix         *
************************/
void ludecomposition(int *matrix, int *roworder, int dimension)
{
   int col, row, k;

   int nextrow, temp;

   /* algorithm is from numerical recipes in c book */
   /* we use Crout's algorithm but since we work */
   /* over GF(2) we need only a simple pivoting */

   for (col = 0; col < dimension; col++)
   {
      /* first do recursion on beta */
      /* except for case row=col */
      for (row = 0; row < col; row++)
         for (k = 0; k < row; k++)
            *(matrix + row * dimension + col) ^=
               (*(matrix + row * dimension + k) & *(matrix + k * dimension + col));

      /* now treat the case row=col */
      nextrow = -1;
      for (row = col; row < dimension; row++)
      {
         temp = *(matrix + row * dimension + col);
         for (k = 0; k < col; k++)
            temp ^= (*(matrix + row * dimension + k) & *(matrix + k * dimension + col));
         if (temp == 1)
         {
            nextrow = row;
            *(matrix + nextrow * dimension + col) = 1;
            break;
         }
      }
      if (nextrow < 0)
      {
         printf("Matrix is singular!\n");
         exit(0);
      }

      /* exchange rows and columns and record this */
      if (col != nextrow)
      {
         for (k = 0; k < dimension; k++)
         {
            temp = *(matrix + col * dimension + k);
            *(matrix + col * dimension + k) = *(matrix + nextrow * dimension + k);
            *(matrix + nextrow * dimension + k) = temp;
         }
         temp = *(roworder + col);
         *(roworder + col) = *(roworder + nextrow);
         *(roworder + nextrow) = temp;
      }
      /* now do alpha recursion */
      for (row = col + 1; row < dimension; row++)
         for (k = 0; k < col; k++)
            *(matrix + row * dimension + col) ^=
               (*(matrix + row * dimension + k) & *(matrix + k * dimension + col));
   }
}

/*************************
* solve system of linear *
* equations given a lu   *
* decomposition          *
*************************/
void lusolve(int *matrix, int *rhs, int *roworder, int dimension)
{
   int row, k;

   int *temp;

   /* allocate memory */
   temp = (int *)allocate(dimension * sizeof(int));

   /* first rearrange input */
   for (row = 0; row < dimension; row++)
      temp[row] = rhs[roworder[row]];

   /* forward substitution */
   for (row = 0; row < dimension; row++)
   {
      for (k = 0; k < row; k++)
         temp[row] ^= (temp[k] & *(matrix + row * dimension + k));
   }

   /* backward substitution */
   for (row = dimension - 1; row >= 0; row--)
   {
      for (k = row + 1; k < dimension; k++)
         temp[row] ^= (temp[k] & *(matrix + row * dimension + k));
   }

   for (row = 0; row < dimension; row++)
      rhs[row] = temp[row];

   free(temp);
}

/*************************
* determine inverse of a *
* binary matrix          *
*************************/
void inversematrix(int *matrix, int *inversematrix, int dimension)
{
   int row, col, k;

   int *rhs, *roworder;

   /* allocate memory */
   rhs = (int *)allocate(dimension * sizeof(int));
   roworder = (int *)allocate(dimension * sizeof(int));

   /* determine lu decomposition of matrix */
   for (row = 0; row < dimension; row++)
      roworder[row] = row;
   ludecomposition(matrix, roworder, dimension);

   for (col = 0; col < dimension; col++)
   {
      /* clear field */
      for (k = 0; k < dimension; k++)
         rhs[k] = 0;
      rhs[col] = 1;

      lusolve(matrix, rhs, roworder, dimension);

      for (k = 0; k < dimension; k++)
      {
         *(inversematrix + k * dimension + col) = rhs[k];
      }

   }


   /* free space */
   free(rhs);
   free(roworder);
}

/******************************
/* encode an information word *
******************************/
void encode(graphs * graph, int *iphi, int *codeword, int gap, int verbose)
{
   int node, socket;

   int vnodenum, cnodenum;

   int row, col, temp;

   int *tempa, *tempb;

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   /* allocate and clear temp */
   if (verbose)
      printf("allocate space\n");
   tempa = (int *)allocate(vnodenum * sizeof(int));
   tempb = (int *)allocate(vnodenum * sizeof(int));
   cleararray(tempa, vnodenum);
   cleararray(tempb, vnodenum);

   /* start by determining p1 */
   /* A s^T */
   graphmultiply(graph, 0, cnodenum - gap, 0, vnodenum - cnodenum, codeword, tempa);
   if (verbose)
   {
      printf("A s^T\n");
      printmatrix(tempa, 1, cnodenum - gap);
   }

   /* T^-1 A s^T */
   multiplybytminusone(graph, gap, tempa, tempb);
   if (verbose)
   {
      printf("T^-1 A s^T\n");
      printmatrix(tempb, 1, cnodenum - gap);
   }

   /* E (T^-1 A s^T) */
   cleararray(tempa, gap);
   graphmultiply(graph, cnodenum - gap, cnodenum,
                 vnodenum - cnodenum + gap, vnodenum, tempb, tempa);
   if (verbose)
   {
      printf("E (T^-1 A s^T) \n");
      printmatrix(tempa, 1, gap);
   }

   /* C s^T + E T^-1 A s^T */
   graphmultiply(graph, cnodenum - gap, cnodenum, 0, vnodenum - cnodenum, codeword, tempa);
   if (verbose)
   {
      printf("C s^T + E T^-1 A s^T \n");
      printmatrix(tempa, 1, gap);
   }

   /* phi^-1 (C s^T + E T^-1 A s^T) */
   for (row = 0; row < gap; row++)
      for (col = 0; col < gap; col++)
         codeword[row + vnodenum - cnodenum] ^= (*(iphi + row * gap + col) & tempa[col]);
   if (verbose)
   {
      printf("phi^-1 (C s^T + E T^-1 A s^T) \n");
      printmatrix(codeword, 1, vnodenum);
   }

   /* now determine p2 */
   /* A s^T + B p1^T */
   cleararray(tempa, vnodenum);
   graphmultiply(graph, 0, cnodenum - gap, 0, vnodenum - gap, codeword, tempa);
   if (verbose)
   {
      printf("A s^T + B p1^T \n");
      printmatrix(tempa, 1, cnodenum - gap);
   }

   /* T^-1 (A s^T + B p1^T) */
   multiplybytminusone(graph, gap, tempa, codeword + vnodenum - cnodenum + gap);
   if (verbose)
   {
      printf("T^-1 (A s^T + B p1^T) \n");
      printmatrix(codeword, 1, vnodenum);
   }

   /* make sure that all constraints are fulfilled */
   if (verbose)
      printf("make sure that all constraints are fulfilled \n");
   cleararray(tempa, vnodenum);
   graphmultiply(graph, 0, cnodenum, 0, vnodenum, codeword, tempa);
   for (node = 0; node < vnodenum; node++)
      assert(tempa[node] == 0);

   /* free allocated space */
   free(tempa);
   free(tempb);
}

/**************************
/* multiply a vector with *
* a specified part of H   *
* using the sparseness of *
* the graph               *
**************************/
void graphmultiply(graphs * graph, int rowstart, int rowend,
                   int colstart, int colend, int *in, int *out)
{
   int node, socket;

   for (node = rowstart; node < rowend; node++)
   {
      for (socket = 0; socket < cnodedegree; socket++)
      {
         if ((cnodeedge.dest >= colstart) && (cnodeedge.dest < colend))
            out[node - rowstart] ^= in[cnodeedge.dest - colstart];
      }
   }
}

/********************************
/* perform a multiplication     *
* with T^-1 without explicitly  *
* computing T^-1 using the fact *
* that T is lower triangular    *
* and sparse                    *
********************************/
void multiplybytminusone(graphs * graph, int gap, int *in, int *out)
{
   int node, socket;

   int vnodenum, cnodenum;

   int temp;

   vnodenum = (*graph).vnodenum;
   cnodenum = (*graph).cnodenum;

   for (node = 0; node < (cnodenum - gap); node++)
   {
      temp = 0;
      for (socket = 0; socket < cnodedegree; socket++)
      {
         if ((cnodeedge.dest - vnodenum + cnodenum - gap) >= 0)
            temp ^= out[cnodeedge.dest - vnodenum + cnodenum - gap];
      }

      out[node] = (temp ^ in[node]);
   }
}

/******************
/* clear an array *
******************/
void cleararray(int *array, int length)
{
   int i;

   for (i = 0; i < length; i++)
      array[i] = 0;
}

/******************
/* print a matrix *
******************/
void printmatrix(int *matrix, int gap, int vnodenum)
{
   int row, col;

   for (row = 0; row < gap; row++)
   {
      for (col = 0; col < vnodenum; col++)
         printf("%1d", *(matrix + row * vnodenum + col));
      printf("\n");
   }
   printf("\n");
}

int node;

/***************************************
* this routine constructs the encoder * 
* for every new codeword              *
**************************************/
void constructencoder(graphs * graph, channels * channel, encoders * encoder,
                      simulations * simulation, int loop)
{
   int node;



   if (loop == -1)
   {
      // if encoder shall be used and encoder has not been built 
      // then build one
      if (((*encoder).useencoder == 1) && ((*encoder).gap < 0))
      {
         /* run greedy algorithm to find approximate */
         /* upper triangulation */
         uppertriangulation(graph, &(*encoder).gap, 0);

         /* build encoder */
         determineiphi(graph, encoder, (*simulation).graphfile, 0);

         /* read the modified graph and encoder back */
         readgraph(graph, encoder, (*simulation).graphfile, (*simulation).verbose);
      }
      else
      {
         /* store the inverse etc in the file */
         savegraph(graph, encoder, (*simulation).graphfile, 0);
      }
   }
   else if (loop >= 0)
   {
      if ((*encoder).useencoder == 1)
      {
         {
            /* run greedy algorithm to find approximate */
            /* upper triangulation */
            uppertriangulation(graph, &(*encoder).gap, 0);
            determineiphi(graph, encoder, (*simulation).graphfile, 0);
            readgraph(graph, encoder, (*simulation).graphfile, (*simulation).verbose);

            /* store the inverse etc in the file */
            savegraph(graph, encoder, (*simulation).graphfile, 0);
         }
      }
   }
}

/* if we use encoder then we have */
/* to generate information vector */
/* and encode it */

void encoding(graphs * graph, encoders * encoder, channels * channel)
{
   int n;
   int *binarycodeword;
   int node;

   // if we do not use the encoder use the all-one codeword
   if ((*encoder).useencoder == 0)
   {
      for (node = 0; node < (*graph).vnodenum; node++)
         (*channel).codeword[node] = 1.0;
   }
   else if ((*encoder).useencoder == 1)
   {
      binarycodeword = (int *)allocate(sizeof(int) * (*channel).length);
      /* generate information vector */
      for (n = 0; n < (*channel).length; n++)
         binarycodeword[n] = 0;
      for (n = 0; n < (*graph).vnodenum - (*graph).cnodenum; n++)
         binarycodeword[n] = (int)floor(2.0 * erand48(channelsimrand));

		//printf("\n channelseeds in encoder: %i %i %i\n",
		//channelsimrand[0],channelsimrand[1],channelsimrand[2]);

      /* encode this word */
      encode(graph, (*encoder).iphi, binarycodeword, (*encoder).gap, 0);

      /* modulate this word */
      for (n = 0; n < (*channel).length; n++)
      {
         (*channel).codeword[n] = -2.0 * binarycodeword[n] + 1.0;
      }
      free(binarycodeword);
   }
   /* take noise into account */
   for (n = 0; n < (*channel).length; n++)
   {
      (*channel).noise[n] *= (*channel).codeword[n];
   }

}

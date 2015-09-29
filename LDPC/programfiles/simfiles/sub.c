#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "graphstruct.h"
#include "simulation.h"
#include "sub.h"
#include "sim.h"

/* global variables */
extern unsigned short channelsimrand[3];

extern unsigned short graphsimrand[3];

/********************************
 * Allocate a pointer to void of *
 * size n bytes.                 *    
 ********************************/
void *allocate(int n)
{
   void *p;

   if ((p = malloc(n)) == NULL)
   {
      printf("/***********************************/");
      printf("/*    W   A   R   N   I   N   G    */");
      printf("/***********************************/");
      printf("/*      Not sufficient memory!     */");
      printf("/***********************************/");
      exit(1);
   }

   return p;
}

/******************************************
 * generate Gaussian random variable       *
 * with variance 1                         *
 ******************************************/
double gauss()
{
   static int ready = 0;

   static double noise[2];

   if (ready == 1)
   {
      ready = 0;
      return noise[1];
   }
   else
   {
      nextgausspair(noise);
      ready = 1;
      return noise[0];
   }
}

/*******************************
 * initialize the random number *
 * generator                    *
 *******************************/
void initrandom(int random)
{
   struct timeval tp;

   struct timezone tpz;

   if (random == 0)
   {
      gettimeofday(&tp, &tpz);
      srand48(tp.tv_sec);
   }
   else
      srand48(random);
}

/*************************************
 * generate two independent Gaussian  *
 * random variables with variance 1   *
 *************************************/
void nextgausspair(double *noise)
{
   double u, v;

   double alpha, w;

   do
   {
      u = 2 * erand48(channelsimrand) - 1;
      v = 2 * erand48(channelsimrand) - 1;

      w = u * u + v * v;
   }
   while (w >= 1);

   alpha = sqrt(-2 * log(w) / w);
   *(noise + 0) = alpha * u;
   *(noise + 1) = alpha * v;
}

/**********************************
 * generate random permutation of  *
 * n elements and inverse function *
 **********************************/
void graphrandompermutation(int *perm, int *iperm, int n)
{
   int i, j;

   double test;

   // printf("\n in graphrandompermutation\n");
   // printf("state of rg %d %d %d\n", graphsimrand[0], graphsimrand[1], graphsimrand[3]);
   for (i = 0; i < n; i++)
      *(iperm + i) = i;

   for (i = n - 1; i >= 0; i--)
   {
      test = erand48(graphsimrand);     // was before drand48()
      // printf("\n random nr %f",test);
      j = (int)floor((i + 1) * test);
      *(perm + i) = *(iperm + j);
      *(iperm + j) = *(iperm + i);
   }

   for (i = 0; i < n; i++)
      *(iperm + *(perm + i)) = i;
}

/**********************************
 * generate random permutation of  *
 * n elements and inverse function *
 **********************************/
void randompermutation(int *perm, int *iperm, int n)
{
   int i, j;

   for (i = 0; i < n; i++)
      *(iperm + i) = i;

   for (i = n - 1; i >= 0; i--)
   {
      j = (int)floor((i + 1) * erand48(channelsimrand));

      /* compatible with fast program */
      *(perm + n - 1 - i) = *(iperm + j);

      /* original version */
      /* *(perm+i)=*(iperm+j); */

      *(iperm + j) = *(iperm + i);
   }

   for (i = 0; i < n; i++)
      *(iperm + *(perm + i)) = i;
}

/************************************/
/* Perform permutation according to */
/* `perm'.  */
/* ATTENTION: Does not work inplace */
/************************************/
void permute(double *from, double *to, int *perm, int N)
{
   int i;

   /* compatible with fast version */
   for (i = 0; i < N; i++)
      *(to + perm[i]) = *(from + i);

   /* original version */
   /* for (i=0; i<N; i++) (to+i)=*(from+*(perm+i)); */

}

/************************************/
/* Skip all data of a file until */
/* next C "end of comment" symbol */
/* appears. verbose=1 will result */
/* in all skipped data being */
/* printed.  */
/************************************/
void skip(FILE * fp, int verbose)
{
   int cold, cnew;

   cold = ' ';

   while ((cnew = getc(fp)) != EOF)
   {
      if (verbose)
         printf("%c", cnew);
      if (cnew == '/' && cold == '*')
         break;
      cold = cnew;
   }
   if (cnew == EOF)
      return;
   if ((cnew = getc(fp)) != '\n')
      ungetc(cnew, fp);
}


double binomial(int n, int k)
{
   double result;

   result = lgamma((double)n + 1.0) - lgamma((double)k + 1.0) - lgamma((double)(n - k) + 1.0);
   return (exp(result));
}

// store state of random generators at this moment
// stored values can be used for rerun the same experiment
void getchannelsimrand(randomizes * randomize)
{
   int i;

   for (i = 0; i < 3; i++)
      (*randomize).channelsimrand[i] = channelsimrand[i];
}

void getgraphsimrand(randomizes * randomize)
{
   int i;

   for (i = 0; i < 3; i++)
      (*randomize).graphsimrand[i] = graphsimrand[i];
}

void setgraphseeds(int loop, reruns * rerun)
{
   int i;

   if ((*rerun).rerun == 2)
   {
      for (i = 0; i < 3; i++)
      {
         graphsimrand[i] = *(*((*rerun).seeds + loop) + i);
         // printf("\n %i: graph %d", i, graphsimrand[i]);
      }
   }
}

void setchannelseeds(int loop, reruns * rerun)
{
   int i;

   if ((*rerun).rerun == 2)
   {
      for (i = 0; i < 3; i++)
      {
         channelsimrand[i] = *(*((*rerun).seeds + loop) + 3 + i);
      }
   }
}

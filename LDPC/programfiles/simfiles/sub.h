

/***************************
* definitions for routines *
* in sub.c                 *
***************************/
/* external variables */

extern unsigned short channelsimrand[3];

extern unsigned short graphsimrand[3];

/*********************************************/
/* make sure that memory is allocated safely */
/*********************************************/
void *allocate(int);

/********************************************/
/* produce a gaussian random variable with */
/* with variance 1 */
/********************************************/
double gauss();

/*******************************
 * initialize the random number *
 * generator                    *
 *******************************/
void initrandom(int);

/*************************************
 * generate two independent Gaussian  *
 * random variables with variance 1   *
 *************************************/
void nextgausspair(double *);

/**********************************
 * generate random permutation of  *
 * n elements and inverse function *
 **********************************/
void randompermutation(int *, int *, int);

/**********************************
 * generate random permutation of  *
 * n elements and inverse function *
 **********************************/
void graphrandompermutation(int *perm, int *iperm, int n);

/************************************/
/* Perform permutation according to */
/* `perm'.  */
/* ATTENTION: Does not work inplace */
/************************************/
void permute(double *, double *, int *, int);

/************************************/
/* Skip all data of a file until */
/* next C "end of comment" symbol */
/* appears. verbose=1 will result */
/* in all skipped data being */
/* printed.  */
/************************************/

void skip(FILE *, int);

double binomial(int n, int k);

void getchannelsimrand(randomizes * randomize);

void getgraphsimrand(randomizes * randomize);

void setgraphseeds(int loop, reruns * rerun);

void setchannelseeds(int loop, reruns * rerun);

/* contains structures for graphconstruction and really useful shortcuts */

#define MAXDEGREENUM 100
#define MAXCYCLE 4
#define STANDARDENSEMBLE 0
#define DEGREETWOSEPERATEDENSEMBLE 1
#define DEGREETWOINRINGENSEMBLE 2
#define vnodedegree (*((*graph).vnodelist+node)).degree
#define cnodedegree (*((*graph).cnodelist+node)).degree
#define vtonodedegree (*((*graph).vnodelist+tonode)).degree
#define ctonodedegree (*((*graph).cnodelist+tonode)).degree
#define vnodevalue (*((*graph).vnodelist+node)).value
#define vtonodevalue (*((*graph).vnodelist+tonode)).value
#define cnodevalue (*((*graph).cnodelist+node)).value
#define ctonodevalue (*((*graph).cnodelist+tonode)).value
#define vnodeedgelist (*((*graph).vnodelist+node)).edgelist
#define cnodeedgelist (*((*graph).cnodelist+node)).edgelist
#define vtonodeedgelist (*((*graph).vnodelist+tonode)).edgelist
#define ctonodeedgelist (*((*graph).cnodelist+tonode)).edgelist
#define vnodeedge (*((*((*graph).vnodelist+node)).edgelist+socket))
#define cnodeedge (*((*((*graph).cnodelist+node)).edgelist+socket))
#define vnodetoedge (*((*((*graph).vnodelist+tonode)).edgelist+tosocket))
#define cnodetoedge (*((*((*graph).cnodelist+tonode)).edgelist+tosocket))

/**************************
 * edge: one integer which *
 * denotes the destination *
 * node and a double which *
 * contains the message    *
 **************************/
typedef struct
{
   int dest;                    /* destination node */
   int socket;                  /* socket at the destination */
   double message;              /* incoming message from distination node */
} edge;

/************************
 * define structure of a *
 * variable node         *
 ************************/
typedef struct
{
   int degree;                  /* degree of node */
   double value;                /* received value */
   edge *edgelist;              /* list of connected edges */
} vnode;

/************************
 * define structure of a *
 * check node            *
 ************************/
typedef struct
{
   int degree;                  /* degree of node */
   double value;                /* value of checknode */
   edge *edgelist;              /* list of connected edges */
} cnode;

/************************
 * define structure of a *
 * degreelist            *
 ************************/
typedef struct
{
   int degree[MAXDEGREENUM];    /* particular degree */
   int numofnodes[MAXDEGREENUM];        /* number of nodes with this particular degree */
} degreelist;

/************************
 * define structure of a *
 * graph                 *
 ************************/
typedef struct
{
   int vnodenum;                /* number of vnodes */
   int cnodenum;                /* number of cnodes */
   double rate;                 /* rate of the code */
   int vdegreesnum;             /* number of different vdegrees */
   int cdegreesnum;             /* number of different cdegrees */
   int vmaxdegree;              /* maximum vdegree */
   int cmaxdegree;              /* maximum cdegree */
   degreelist *vdegreelist;     /* list of vdegrees */
   degreelist *cdegreelist;     /* list of cdegrees */
   vnode *vnodelist;            /* list of vnodes */
   cnode *cnodelist;            /* list of cnodes */
} graphs;

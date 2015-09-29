

typedef struct
{
   char *fpinput;               // parameter file
   int verbose;                 // 0 -> silent, 1 -> verbose mode
   int newgraph;                // construct each time a new graph (and encoder)
   int constructgraphonce;      // 0-> construct new graph each time, 1-> construct graph one time 
                                // 
   // 
   // 
   // 
   // 
   // 
   // 
   // randomly
   char graphfile[100];          // file in which graphs are saved 
   // (before start, after constructing 
   // or after the inverse of the encoder)
   char protofile[100];          // file in which underlying protograph is stored
   char inputfile[100];
   graphs *protograph;          // stores protograph for later use
   int ensemble;                // standard ensemble,poisson,protographs
   int length;                  // for standart and poisson ensemble: 
   // given number of variable nodes 
   // (will be approximated by given degree distribution)
   // for protograph ensembles:
   // number of copies of original protograph 
   int cnodenum;                // only used for poisson ensemble
   int vdegreesnum;             // for stand. ens.: number of variable degrees
   int *vdegreelist;            // for stand. ens.: 
   // NOTE: always allow degree two and three 
   // variables; this is needed by the graph 
   // constructor to ensure that the edge counts 
   // match up; 
   double *vpercentagelist;     // for stand. ens.: distribution of variable degrees
   int cdegreesnum;             // for stand. ens.: number of check degrees
   int *cdegreelist;            // for stand. ens.: degrees of check nodes
   double *cpercentagelist;     // for stand. ens.: distribution of check degrees
   int typedist;                // for stand. ens.: 0 -> node distribution
   // 1 -> edge distribution
   int numloopstype;            // if 0, then run for fxd number of loops, if 1 run until accuracy 
                                // 
   // 
   // 
   // 
   // 
   // 
   // 
   // 
   // 
   // has been reached 
   int numloops;                // number of realisations we want to run
   double alpha;                // accuracy goal 
   int numiterations;           // number of iterations in one decoding process
   // int numiter_option; // want to stop before end of numiterations?(see paramfile)
   int decodertype;             // BP,GalA,...
   int printoption;             // 0 -> print to screen, 1 -> print to 'printfile' 
   int printfrequency;          // every how many realisations you want to print results?
   int computevariance;         // set to 1 if you want to compute variance
} simulations;

typedef struct
{
   int useencoder;              // 1 -> use encoder, 0 -> use all-one codeword
   int gap;                     // 
   int *iphi;                   // inverse of encoder matrix
   char *fpinput;               // store graph (inverse of encoder) in here
} encoders;

typedef struct
{
   // BEC,BEC with fixed errorprob,
   // BSC,BSC with fixed errorprob ,AWGN,...
   int type;
   double *parameter;           // array of errorprobability after transmission
   int parameter_num;           // number of channelparameter
   int length;                  // length of codeword (number of variable nodes)
   double *noise;               // noise comming from the channel will be 
   // noise added to the codeword
   double *codeword;            // sent codeword
   double *decisions;           // decisions taken at each round
} channels;

typedef struct
{
   // howstart for codeword-generator: 
   // 0 -> start randomly 
   // 1 -> starting seed is given in paramfile
   // 2 -> starting seed is given in seedinputfile
   int howstartgraph;
   int howstartchannel;

   // print used seeds for a specific run of simulation (is fixed at
   // the beginning of sim (before first parameter and first loop))
   int chooseloop;
   unsigned short channelsimrand[3];
   unsigned short graphsimrand[3];
   // int randominit;
} randomizes;


typedef struct
{
   int initialerrors;           // channel error
   int biterrors;               // number of bit-errors after current loop
   int totalbiterrors;          // number of bit-errors after n loops
   int worderrors;              // number of word-errors after current loop
   int totalworderrors;         // number of word-errors after n loops
   int sizeof_minerror;         // count only errors of size at least 'sizeof_minerror'
   int *errorhistrogram;        // histogram of errorsize
   int biterror_flag;           // flag: 0 -> do not count biterrors
   int worderror_flag;          // flag: 0 -> do not count worderrors
   int errorhistogram_flag;     // flag: 0 -> do not compute histogram of errors
   char printfile[25];          // file where errors/word are printed to
   char printfilebit[25];       // file where errors/bit are printed to
} errorcounts;

typedef struct
{
   int rerun;                   // flag: 2 -> you want to rerun a realization
   int cond[4];                 // biteps_min,biteps_max, messageeps_min, messageeps_max
   unsigned short int **seeds;  // seeds of realiations we want to rerun (6 per realization)
   int action;                  // do this action on reruned realizations
} reruns;

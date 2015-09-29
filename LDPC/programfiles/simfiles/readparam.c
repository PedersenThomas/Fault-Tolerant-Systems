#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <ctype.h>
#include <assert.h>
#include <sys/time.h>
#include "graphstruct.h"
#include "simulation.h"
#include "sub.h"
#include "sim.h"
#include "graphfunct.h"
#include "decode.h"
#include "readparam.h"

/******************************************
* Read in parameters from parameter file  *
******************************************/
int readparamfile(int argc, char **argv, simulations * simulation, graphs * graph,
                  encoders * encoder, channels * channel, errorcounts * errorcount,
                  randomizes * randomize, reruns * rerun)
{

   FILE *fpinput;

   int i, verbose;

   printf("\n");
   printf("read param file ...");

  /*************************************/
   /* read input from file param */
  /*************************************/
   if (argc % 2 == 1)
   {
      if ((fpinput = fopen("param", "r")) == NULL)
      {
         fprintf(stderr, "I can't open the file `param'\n");
         return 0;
      }
		else
			printf(" param \n");
   }
   else
   {
      for (i = 1; i < argc; i += 2)
      {
         if (argv[i][0] != '-')
         {
            if ((fpinput = fopen(argv[i], "r")) == NULL)
            {
               fprintf(stderr, "I can't open the indicated parameter file.\n");
               return 0;
            }
				else 
					printf(" %s",argv[i]);
            break;
         }
      }
   }

   /* start now to read in param file */

   // run in verbose or quiet mode
   skip(fpinput, 0);
   fscanf(fpinput, "%d", &(*simulation).verbose);
   verbose = (*simulation).verbose;


   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).newgraph);
   (*rerun).rerun = (*simulation).newgraph;
   if (verbose)
      printf("\n%d", (*simulation).newgraph);

   if ((*simulation).newgraph < 2)
      return sim_readparamfile(fpinput, simulation, graph, encoder, channel,
                               errorcount, randomize, rerun);
   else if ((*simulation).newgraph >= 2)
   {
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"this option has not been implemented (readparam())\n");
      exit(1);
   }
}


// read params from paramfile
int sim_readparamfile(FILE * fpinput, simulations * simulation, graphs * graph,
                      encoders * encoder, channels * channel, errorcounts * errorcount,
                      randomizes * randomize, reruns * rerun)
{
   int i, verbose;

   int degree;

   double percentage;

   int simulationseed;

   struct timezone tpz;

   struct timeval tp;           /* variables for generation of randomness */
	unsigned long int ult;


   /*******************************************************/
   /* read input from file param for simulation */
   /*******************************************************/

   verbose = (*simulation).verbose;
   switch ((*simulation).newgraph)
   {
   case 0:                     // graph is already known
      // read in name where graph is stored
      fscanf(fpinput, "%s", &(*simulation).graphfile);
		if(isdigit((*simulation).graphfile[0]))
		{
			fprintf(stderr,"ERROR:\n");
			fprintf(stderr,"graphfile name needs to  start with a character\n");
			exit(1);	
		}
      // in this case the next paragraph is not relavant
      skip(fpinput, verbose);
      break;
   case 1:                     // just degree distribution is given
      // determine how to construct graph
      fscanf(fpinput, "%d", &(*randomize).howstartgraph);
      switch ((*randomize).howstartgraph)
      {
		case 0:                  // generate graphseed 'randomly' such that seeds change
                               // quickly, we have 3 different seeds and the
                               // seeds are predictable on different machines
          gettimeofday(&tp, &tpz);
          ult = (tp.tv_sec ^ (tp.tv_usec / 1000));
          graphsimrand[0]=(unsigned short int) (ult & 177777);
          ult >>= sizeof(short);
          graphsimrand[1]=(unsigned short int) (ult&177777);
          graphsimrand[2]=(unsigned short int) ((tp.tv_sec ^ (tp.tv_usec * 100))&177777);
          break;
      case 1:                  // starting graphseed is given
         fscanf(fpinput, "%d", &simulationseed);
         graphsimrand[0] = simulationseed;
         fscanf(fpinput, "%d", &simulationseed);	
         graphsimrand[1] = simulationseed;
         fscanf(fpinput, "%d", &simulationseed);
         graphsimrand[2] = simulationseed;
         break;
		default:
			fprintf(stderr,"ERROR:\n");
			fprintf(stderr,"this parameter (howstartgraph) must be 0 or 1.\n");
			exit(1);
      }
      if (verbose)
      {
         printf("\n%d\n", (*randomize).howstartgraph);
         if ((*randomize).howstartgraph == 1)
            printf("graphseeds: %d %d %d\n", graphsimrand[0], graphsimrand[1], graphsimrand[2]);
      }
      fscanf(fpinput, "%d", &(*simulation).constructgraphonce);
		if((*simulation).constructgraphonce>1)
		{
			fprintf(stderr,"ERROR:\n");
			fprintf(stderr,"this parameter (constructgraphonce) must be 0 or 1.\n");
			exit(1);
		}
      // read in name into which graph should be stored (do this everytime even
      // if new graph everytime, cause we need it in case we use the encoder)
      fscanf(fpinput, "%s", (*simulation).graphfile);
		if(!isalpha((*simulation).graphfile[0]))
		{
			fprintf(stderr,"ERROR:\n");
			fprintf(stderr,"graphfile (%s) name should start with a letter\n",(*simulation).graphfile);
			exit(1);
		}

      // read in ensemble type
      skip(fpinput, verbose);
      fscanf(fpinput, "%d", &(*simulation).ensemble);
      if (verbose)
         printf("\n%i", (*simulation).ensemble);
      // read in length 
      // in case of protographs number of copies
      // is stored in length
      fscanf(fpinput, "%d", &(*simulation).length);
      if (verbose)
         printf("\n%i\n", (*simulation).length);
      // read in parameters depending on ensemble
      if ((*simulation).ensemble == 0)  // irregular ensemble
      {
         fscanf(fpinput, "%d", &(*simulation).typedist);
         // allocate memory for degreelists
         (*simulation).vdegreelist = (int *)allocate(sizeof(int) * MAXDEGREENUM);
         (*simulation).cdegreelist = (int *)allocate(sizeof(int) * MAXDEGREENUM);

         (*simulation).vpercentagelist = (double *)allocate(sizeof(double) * MAXDEGREENUM);
         (*simulation).cpercentagelist = (double *)allocate(sizeof(double) * MAXDEGREENUM);

         fscanf(fpinput, "%d", &degree);
         (*simulation).vdegreelist[0] = degree;
         fscanf(fpinput, "%lf", &percentage);
         (*simulation).vpercentagelist[0] = percentage;

         (*simulation).vdegreesnum = 0;
         degree = 0;
         while (degree != -1)
         {
            fscanf(fpinput, "%d", &degree);
            (*simulation).vdegreelist[(*simulation).vdegreesnum + 1] = degree;
            fscanf(fpinput, "%lf", &percentage);
            (*simulation).vpercentagelist[(*simulation).vdegreesnum + 1] = percentage;
            (*simulation).vdegreesnum++;
         }
         fscanf(fpinput, "%d", &degree);
         (*simulation).cdegreelist[0] = degree;
         fscanf(fpinput, "%lf", &percentage);
         (*simulation).cpercentagelist[0] = percentage;

         (*simulation).cdegreesnum = 0;
         degree = 0;
         while (degree != -1)
         {
            fscanf(fpinput, "%d", &degree);
            (*simulation).cdegreelist[(*simulation).cdegreesnum + 1] = degree;
            fscanf(fpinput, "%lf", &percentage);
            (*simulation).cpercentagelist[(*simulation).cdegreesnum + 1] = percentage;
            (*simulation).cdegreesnum++;
         }

         if (verbose)
         {
            for (i = 0; i < (*simulation).vdegreesnum; i++)
               printf("%2d %g\n", (*simulation).vdegreelist[i], (*simulation).vpercentagelist[i]);
            printf("-1 -1.0\n");
            for (i = 0; i < (*simulation).cdegreesnum; i++)
               printf("%2d %g\n", (*simulation).cdegreelist[i], (*simulation).cpercentagelist[i]);
            printf("-1 -1.0\n");
         }
      }
      else if ((*simulation).ensemble == 1)     // protograph ensemble
      {
         fscanf(fpinput, "%s", &simulation->protofile);
      }
      else
      {
			fprintf(stderr,"ERROR:\n");
         fprintf(stderr,"this ensemble has not yet been implemented\n");
         exit(1);
      }
      break;
   default:
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"we should never end up here (newgraph)\n");
      exit(1);
      break;
   }

   // check if encoder shall be used
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*encoder).useencoder);
	if((*encoder).useencoder>1) {
		fprintf(stderr,"ERROR:\n");
		fprintf(stderr,"the parameter (useencoder) must be 0 or 1.\n");
		exit(1);
	}
   if (verbose)
      printf("\n%d", (*encoder).useencoder);

   // channel type
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*channel).type);
   if (verbose)
      printf("\n%d", (*channel).type);

   // channelparameter
   if ((*channel).type == BEC || (*channel).type == BSC || (*channel).type == AWGNC)
   {
      skip(fpinput, verbose);
      fscanf(fpinput, "%d", &(*channel).parameter_num);
      if (verbose)
         printf("\n%d", (*channel).parameter_num);

      (*channel).parameter = (double *)allocate(sizeof(double) * (*channel).parameter_num);

      skip(fpinput, verbose);
      for (i = 0; i < (*channel).parameter_num; i++)
      {
         fscanf(fpinput, "%lf", &(*channel).parameter[i]);
         if (verbose)
            printf("\n%g", (*channel).parameter[i]);
      }
   }
   else
   {
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"not a valid channel\n");
      exit(1);
   }


   // decoder type 
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).decodertype);
   if (verbose)
      printf("\n%d", (*simulation).decodertype);
	if((*simulation).decodertype>2)
	{
		fprintf(stderr,"ERROR:\n");
		fprintf(stderr,"decoders others than GalA and BeliefProbagation have not been implemented\n");
	}

   // random number generator
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*randomize).howstartchannel);

   switch ((*randomize).howstartchannel)
   {
	 case 0:                     // generate starting seed randomly
      gettimeofday(&tp, &tpz);
          ult = 2*(tp.tv_sec ^ (tp.tv_usec / 1000));
          channelsimrand[0]=(unsigned short int) (ult & 177777);
          ult >>= sizeof(short);
          channelsimrand[1]=(unsigned short int) (ult&177777);
          channelsimrand[2]=(unsigned short int) ((tp.tv_sec ^ (tp.tv_usec * 100))&177777);
        break;
   case 1:                     // starting seed is given
      fscanf(fpinput, "%d", &simulationseed);
      channelsimrand[0] = simulationseed;
      fscanf(fpinput, "%d", &simulationseed);
      channelsimrand[1] = simulationseed;
      fscanf(fpinput, "%d", &simulationseed);
      channelsimrand[2] = simulationseed;
      break;
   }

   if (verbose)
   {
      printf("\n%d\n", (*randomize).howstartchannel);
      if ((*randomize).howstartchannel == 1)
         printf("%d %d %d\n", channelsimrand[0], channelsimrand[1], channelsimrand[2]);
   }

   // number of loops
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).numloopstype);
   if (verbose)
      printf("\n%d", (*simulation).numloopstype);


   if ((*simulation).numloopstype == 0)
   {
      // if fixed number of loops is desired then read in this number
      fscanf(fpinput, "%d", &(*simulation).numloops);
      if (verbose)
         printf("\n%d", (*simulation).numloops);
   }
   else
   {
      // if accuracy goal is desired then read in this goal
      fscanf(fpinput, "%lf", &(*simulation).alpha);
      // in this case set number of loops to a REALLY large number
      (*simulation).numloops = 999999999;
      if (verbose)
      {
         printf("\n%g", (*simulation).alpha);
         printf("\n%d", (*simulation).numloops);
      }
   }


   // number of iterations at the decoder
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).numiterations);
   if (verbose)
      printf("\n%d", (*simulation).numiterations);

   // count only errors larger than...
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*errorcount).sizeof_minerror);
   if (verbose)
      printf("\n%d", (*errorcount).sizeof_minerror);


   // print out every ...
   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).printfrequency);
	if((*simulation).printfrequency > (*simulation).numloops)
	{
		fprintf(stderr,"ERROR:\n");
		fprintf(stderr,"printfrequency must be smaller or equal as number of loops\n");
		exit(1);
	}

   if (verbose)
      printf("\n%d", (*simulation).printfrequency);

   skip(fpinput, verbose);
   fscanf(fpinput, "%d", &(*simulation).printoption);
   if (verbose)
      printf("\n%d", (*simulation).printoption);

   if ((*simulation).printoption == 0)
   {
      (*errorcount).biterror_flag = 1;
      (*errorcount).worderror_flag = 1;
   }

   if ((*simulation).printoption == 1)
   {
      (*errorcount).biterror_flag = 1;
      (*errorcount).worderror_flag = 0;
	

		if((*errorcount).sizeof_minerror>1)
		{
			printf("\n expurgation >1\n");
			printf("\n expurgation parameter :=%i",(*errorcount).sizeof_minerror);
			(*errorcount).sizeof_minerror=1;
		 	fprintf(stderr,"\n\nWARNING:\n");
       	fprintf(stderr,"expurgation parameter has been corrected to 1\n");
         fprintf(stderr,"because you print result into a file.\n");
		}

      if ((*simulation).printfrequency > 1)
      {
			printf("\n printfrequency>1\n");
			(*simulation).printfrequency=1;
         fprintf(stderr,"\n\nWARNING:\n");
         fprintf(stderr,"print frequency has been corrected to 1\n");
         fprintf(stderr,"because you print result into a file.\n");
      }

      fscanf(fpinput, "%s", &(*errorcount).printfile);
		if(!isalpha((*errorcount).printfile[0]))
		{
			fprintf(stderr,"ERROR:\n");
			fprintf(stderr,"printfile name should start with a letter\n");
			exit(1);
		}
      if (verbose)
         printf("\n%s", (*errorcount).printfile);
   }
   return 1;
}


/* perform some very basic consistency checks */
void sanitycheck(simulations * simulation, graphs * graph, encoders * encoder, channels * channel,
                 errorcounts * errorcount, randomizes * randomize)
{
   printf("perform sanity check on consistency of input ...");

   // only certain combinations of channel and decoder type make sence:
   if ((*channel).type == BEC || (*channel).type == AWGNC)
   {
      if ((*simulation).decodertype != Belief)
      {
         fprintf(stderr,"ERROR:\n");
         fprintf(stderr,"with BEC and AWGN channel, only use belief propagation!\n");
         exit(1);
      }
   }

   // if data is printed to a file then it should be printed out for
   // each codeword and no expurgation should be performed
   if ((*simulation).printoption == 1)
   {
      if ((*errorcount).sizeof_minerror > 1)
      {
         printf("\n\nWARNING:\n");
         printf("It is recommended that you set expurgation parameter to 1\n");
         printf("if you print result into a file.\n");
      }

      if ((*simulation).printfrequency != 1)
      {
         printf("\n\nWARNING:\n");
         printf("	It is recommended that you set print frequency to 1\n");
         printf("	if you print result into a file.\n");
      }
   }

   if ((*simulation).numloopstype == 1)
   {
      if ((*simulation).numloops < 100)
      {
         printf("\n\nWARNING:\n");
         printf("Are you sure you only want %d loops?\n", (*simulation).numloops);
      }
   }

   printf("... check finished \n\n");

   // please add here any other helpful check you can think of

}

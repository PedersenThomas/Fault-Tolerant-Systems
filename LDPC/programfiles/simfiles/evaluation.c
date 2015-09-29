#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include <sys/time.h>
#include "graphstruct.h"
#include "simulation.h"
#include "sub.h"
#include "sim.h"
#include "graphfunct.h"
#include "evaluation.h"
#include "decode.h"


// global variables
FILE *fpdataoutput;             // file pointer to file onto which sim data is written

extern unsigned short channelsimrand[3];

extern unsigned short graphsimrand[3];

double max(double x, double y)
{
   if (x > y)
      return x;
   else
      return y;
}

/***********************************************************/
/* this is the format, the output will be printed to screen */
/***********************************************************/
void print_outputformat(errorcounts * errorcount)
{
   printf("\n");
   printf("simulations:\n");
   printf("results are printed in the following format.\n");
   printf("i ... initial number of errors over the whole length\n");
   printf("r ... remaining number of errors over the whole length\n");
   if ((*errorcount).biterror_flag == 1)
   {
      printf("pb ... bit error rate.\n");
   }
   if ((*errorcount).worderror_flag == 1)
   {
      printf("pB ... word error rate over the whole length with confidence interval\n");
   }
}

/***************************************************************************/
/* initializing variables of errorcount-structure and files used for output */
/***************************************************************************/
void initialize_errorcount(int argc, char **argv, int l, errorcounts * errorcount,
                           simulations * simulation, randomizes *randomize, channels * channel, graphs * graph)
{
   char filename[200], command[200];

   int numedges, node, socket;

   // reset all counters
   (*errorcount).totalbiterrors = 0;
   (*errorcount).biterrors = 0;
   (*errorcount).worderrors = 0;

	if (l==0)
   {
		printf("\n\n /******* SEEDS ***********/\n");
		printf("graphseeds:   %d %d %d\n",
				(*randomize).graphsimrand[0],(*randomize).graphsimrand[1],(*randomize).graphsimrand[2]);
      	
		printf("channelseeds: %d %d %d\n",
				(*randomize).channelsimrand[0],(*randomize).channelsimrand[1],(*randomize).channelsimrand[2]);
		printf("/*************************/\n\n");
	}

   // check if the output should go to file 
   if ((*simulation).printoption == 1)
   {
      // close previous file
      if (l > 1)
         fclose(fpdataoutput);

      // open the new file
      sprintf(filename, "data/%s_%6.5lf", (*errorcount).printfile, (*channel).parameter[l]);
      if (argc % 2 != 1)
      {
         // if the name of the parameter file was explicitly given
         sprintf(command, "cp %s data/%s_%6.5lf", argv[1], (*errorcount).printfile,
                 (*channel).parameter[l]);
      }
      else
      {
         // if no name was given use the generic name
         sprintf(command, "cp param data/%s_%6.5lf", (*errorcount).printfile, (*channel).parameter[l]);
      }

      // copy the current param file to the data file
      system(command);

      // now open this data file in append mode to write onto it data
      fpdataoutput = fopen(filename, "a");

      // make sure output is not buffered and instantly written
      setlinebuf(fpdataoutput);

      numedges = 0;
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         for (socket = 0; socket < vnodedegree; socket++)
         {
            numedges += 1;
         }
      }

	
		// store random seeds to rerun same realization
		fprintf(fpdataoutput,"\ngraphseeds: %d %d %d\n",
				(*randomize).graphsimrand[0],(*randomize).graphsimrand[1],(*randomize).graphsimrand[2]);
      	
		fprintf(fpdataoutput,"\nchannelseeds: %d %d %d\n",
				(*randomize).channelsimrand[0],(*randomize).channelsimrand[1],(*randomize).channelsimrand[2]);


      fprintf(fpdataoutput, "\n\n\n/************** LENGTH %d ******************/\n\n",
              (*channel).length);
      fprintf(fpdataoutput, "\n\n\n/************** NUMBER OF EDGES %d ******************/\n\n",
              numedges);
      fprintf(fpdataoutput, "\n\n\n/************** LOOPS %d ******************/\n\n",
              (*simulation).numloops);
   }
}

/*************************************************************/
/* count bit errors. if biterror>0 , increment worderrors */
/*************************************************************/
void count_error(graphs * graph, channels * channel, errorcounts * errorcount,
                 simulations * simulation)
{
   (*errorcount).biterrors = counterrors(channel);
   if ((*errorcount).biterrors >= max((*errorcount).sizeof_minerror, 1))
   {
      (*errorcount).totalbiterrors += (*errorcount).biterrors;
      (*errorcount).worderrors++;
   }
}



/***********************************/
/* various forms of output */
/***********************************/
int print_errors(int loop, int l, simulations * simulation, errorcounts * errorcount,
                 channels * channel, graphs * graph, randomizes * randomize)
{
   int stopflag = 0;

   double confidencealpha;

   int errormessages;

   // general statistics
   if ((loop + 1) % (*simulation).printfrequency == 0)
   {
      // print on screen 
      if ((*simulation).printoption == 0)
      {
         confidencealpha = (*errorcount).worderrors / ((double)(loop + 1));
         confidencealpha =
            2.0 * sqrt(confidencealpha * (1 - confidencealpha) / ((double)(loop + 1)));
         printf
            ("[%g-%6d] i:=%6d r:=%6d pb:=%12.10lf %12.10g <= pB:=%12.10g <= %12.10g\n",
             (*channel).parameter[l], loop + 1, (*errorcount).initialerrors,
             (*errorcount).biterrors,
             ((double)(*errorcount).totalbiterrors) / ((double)((*channel).length) * (loop + 1)),
             max(0, (*errorcount).worderrors / ((double)(loop + 1)) - confidencealpha),
             (*errorcount).worderrors / ((double)(loop + 1)),
             confidencealpha + (*errorcount).worderrors / ((double)(loop + 1)));
      }

//      // print to file //HERE: PRINT SEEDS
//      if ((*simulation).printoption == 1)
//      {
//         fprintf(fpdataoutput, "%d    %d %d %d   %d %d %d    %d  \n", loop + 1,
//                 (*randomize).graphsimrand[0], (*randomize).graphsimrand[1],
//                 (*randomize).graphsimrand[2],
//                 (*randomize).channelsimrand[0], (*randomize).channelsimrand[1],
//                 (*randomize).channelsimrand[2], (*errorcount).biterrors);
//      }

 		// print to file //HERE: DO NOT PRINT SEEDS
      if ((*simulation).printoption == 1)
      {
         fprintf(fpdataoutput, "%d		%d  \n", loop + 1, (*errorcount).biterrors);
      }
   }

   // check if accuracy goal has been set
   if ((*simulation).numloopstype == 1)
   {
      // if the accuracy goal has been reached set stopflag to 1 
      confidencealpha = (*errorcount).worderrors / ((double)(loop + 1));
      confidencealpha = 2.0 * sqrt(confidencealpha * (1 - confidencealpha) / ((double)(loop + 1)));
      // we insist on having seen at least 10 errors since otherwise we might stop much too early
      // if by chance we see a mistake right away
      if (confidencealpha <= (*simulation).alpha * (*errorcount).worderrors / ((double)(loop + 1))
          && (*errorcount).worderrors >= 10)
         stopflag = 1;
   }

   return stopflag;
}


/*********************************************************************/
/* some functions which allow quick access to content of structures */
/*********************************************************************/
void show_simulation_values(simulations * simulation, errorcounts * errorcount)
{
   int i;

   printf("simulation values:\n");
   printf("verbose  ---> %i\n", (*simulation).verbose);
   printf("newgraph ---> %i\n", (*simulation).newgraph);
   printf("construct graph only once? --->  %i \n", (*simulation).constructgraphonce);
   if ((*simulation).newgraph == 0 || (*simulation).constructgraphonce == 1)
      printf("graph given in ---> %s\n", (*simulation).graphfile);

   printf("ensemble ---> %i\n", (*simulation).ensemble);

   if ((*simulation).ensemble == 0)     // Standard ensemble
   {
      printf("number of var nodes ---> %i\n", (*simulation).length);
      printf("type of distribution ---> %i\n", (*simulation).typedist);
      printf("vdegreenum  ---> %i\n", (*simulation).vdegreesnum);

      for (i = 0; i < (*simulation).vdegreesnum; i++)
      {
         printf("    %i  ", (*simulation).vdegreelist[i]);
         printf("    %f \n", (*simulation).vpercentagelist[i]);
      }

      printf("cdegreenum  ---> %i\n", (*simulation).cdegreesnum);

      for (i = 0; i < (*simulation).cdegreesnum; i++)
      {
         printf("    %i  ", (*simulation).cdegreelist[i]);
         printf("    %f \n", (*simulation).cpercentagelist[i]);
      }

   }

   if ((*simulation).ensemble == 1)     // Protograph ensemble
   {
      printf("number of copies ---> %i\n", (*simulation).length);
      printf("protograph input file ---> %s\n", (*simulation).protofile);
   }

   printf("\nnumber of loops ---> %i\n", (*simulation).numloops);
   printf("number of iterations inside decoder ---> %i\n", (*simulation).numiterations);
   // printf("iteration-number option ---> %i\n", (*simulation).numiter_option);
   printf("decodertype ---> %i\n", (*simulation).decodertype);
   if ((*simulation).printoption == 0)
      printf("print results to screen\n");
   if ((*simulation).printoption >= 1)
      printf("print error/word to file ---> %s\n", (*errorcount).printfile);
   if ((*simulation).printoption == 2)
      printf("print error/word to second file ---> %s\n", (*errorcount).printfilebit);
   printf("printfrequency ---> %i\n", (*simulation).printfrequency);
}

void show_encoder_values(encoders * encoder)
{
   printf("\nshow encoder values\n");
   if ((*encoder).useencoder == 1)
   {
      printf("use encoder\n");
      printf("gap %i\n", (*encoder).gap);
   }
   else if ((*encoder).useencoder == 0)
      printf("don't use encoder\n");
}

void show_channel_values(channels * channel, graphs * graph)
{
   int i;

   printf("\nshow channel values\n");
   printf("channeltype %i\n", (*channel).type);
   printf("number of channelparameter %i\n", (*channel).parameter_num);
   printf("channelparameter\n");
   for (i = 0; i < (*channel).parameter_num; i++)
      printf(" %fl, ", (*channel).parameter[i]);

   printf("\n length %i", (*channel).length);
   printf("\n noise \t codeword \t decisions: \t received_values\n");
   for (i = 0; i < (*channel).length; i++)
      printf(" %g \t\t %g \t\t\t %g \t\t\t\t%g\n ",
             (*channel).noise[i], (*channel).codeword[i], (*channel).decisions[i],
             (*((*graph).vnodelist + i)).value);


}

void show_random_values(randomizes * randomize)
{
   printf("\nshow random values \n");
   if ((*randomize).howstartgraph > 0)
      printf("seed triple is given in param file\n");


   if ((*randomize).howstartgraph == 0)
      printf("use time for random graph initialization\n");

   if ((*randomize).howstartchannel == 0)
      printf("start sourceword generation randomly\n");
   if ((*randomize).howstartchannel == 1)
      printf("3 starting seeds are given in paramfile\n");
   if ((*randomize).howstartchannel == 2)
      printf("starting seeds are given in seedinputfile\n");
}

void show_error_values(errorcounts * errorcount)
{
   printf("\nshow error flags and values\n");
   printf("biterror_flag %i\n", (*errorcount).biterror_flag);
   printf("worderror_flag %i\n", (*errorcount).worderror_flag);
   printf("inital error %i\n", (*errorcount).initialerrors);

   if ((*errorcount).biterror_flag)
   {
      printf("biterrors %i\n", (*errorcount).biterrors);
      printf("totalbiterrors %i\n", (*errorcount).totalbiterrors);
   }

   if ((*errorcount).worderror_flag)
   {
      printf("worderror %i\n", (*errorcount).worderrors);
      printf("total worderrors %i\n", (*errorcount).totalworderrors);
   }
}


/*********************************
* count current number of errors *
*********************************/
int counterrors(channels * channel)
{
   int node, nerr = 0;

   for (node = 0; node < (*channel).length; node++)
   {
      if (*((*channel).decisions + node) != *((*channel).codeword + node))
         nerr++;
   }
   return nerr;
}

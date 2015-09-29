#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "graphstruct.h"
#include "simulation.h"
#include "graphfunct.h"
#include "decode.h"
#include "sim.h"
#include "channel.h"
#include "sub.h"

/* global variables */
extern unsigned short channelsimrand[3];

/***********************************
* generate AWGN vector of length n *
***********************************/
void awgnvector(channels * channel, int param)
{
   int i;

   for (i = 0; i < (*channel).length; i++)
      (*channel).noise[i] = (*channel).parameter[param] * gauss();
}

/****************************************
* generate Bernoulli vector of length n *
****************************************/
void bernoullivector(channels * channel, int param)
{
   double zufall;

   int i;

   for (i = 0; i < (*channel).length; i++)
   {
      zufall = erand48(channelsimrand);
      if (zufall < (*channel).parameter[param])
         (*channel).noise[i] = -1.0;
      else
         (*channel).noise[i] = 1.0;
   }
}

/*********************
* initialize channel *
*********************/
void initialize_channel(channels * channel, simulations * simulation, graphs * graph, int param,
                        int loop)
{
   int node;

   // clear the noise vector: this is probably not necessary
   // since it is later on changed anyway

   for (node = 0; node < (*channel).length; node++)
      (*channel).noise[node] = 0.0;

   switch ((*channel).type)
   {
   case BEC:
      // generate a +1/-1 vector with -1 indicating an erasure
      bernoullivector(channel, param);
      switch ((*simulation).decodertype)
      {
      case Belief:

         for (node = 0; node < (*channel).length; node++)
         {
            // an erasure has log-likelihood value 0, whereas a correct
            // value has a log-likelihood of magnitude equal to MAXMESSAGE
            // (it should of course be infinity)
            (*channel).noise[node] = ((*channel).noise[node] + 1.0) * (MAXMESSAGE / 2);
         }
         break;
      default:
			fprintf(stderr,"ERROR:\n");
         fprintf(stderr,"for the BEC channel, other than Belief Propagation has not been implemented yet\n");
         exit(1);
      }
      break;
   case BSC:
      // generate a +1/-1 vector with -1 indicating an error
      bernoullivector(channel, param);
      switch ((*simulation).decodertype)
      {
      case GalA:
         // nothing to be done
         break;
      case Belief:
         // convert into log-likelihoods 
         // the magnitude of the log-likelihood is log((1-eps)/eps)
         for (node = 0; node < (*channel).length; node++)
         {
            (*channel).noise[node] *=
               log((1.0 - (*channel).parameter[param]) / (*channel).parameter[param]);
         }
         break;
      default:
			fprintf(stderr,"ERROR:\n");
         fprintf(stderr,"other than GalA, decoder erasure and Belief probagation has not been implemented yet\n");
         exit(1);
         break;
      }
      break;
   case AWGNC:
      // vector of zero mean independent Gaussian random variables
      // with variance param^2 
      awgnvector(channel, param);

      switch ((*simulation).decodertype)
      {
      case Belief:
         for (node = 0; node < (*channel).length; node++)
         {
            (*channel).noise[node] =
               2 * ((*channel).noise[node] +
                    1.0) / ((*channel).parameter[param] * (*channel).parameter[param]);
            if (vnodedegree > 100)
               (*channel).noise[node] = 20.0;
         }
         break;
      default:
			fprintf(stderr,"ERROR:\n");
         fprintf(stderr,"not implemented yet (initialization of channel) \n");
         exit(1);
         break;
      }
      break;
   default:
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"sorry, this channel has not been implementd yet!\n");
		exit(1);
   }
}

/******************************************************
* this subroutine is called once the length of the    *
* code is determined; it allocates memory for         *
* noise vector, codeword, and decision                *
******************************************************/
void allocatechannelmemory(graphs * graph, channels * channel)
{
   // check if memory has been allocated already;
   // in this case length is set to -1 
   if ((*channel).length < 0)
   {
      (*channel).length = (*graph).vnodenum;
      (*channel).noise = (double *)allocate(sizeof(double) * (*channel).length);
      (*channel).codeword = (double *)allocate(sizeof(double) * (*channel).length);
      (*channel).decisions = (double *)allocate(sizeof(double) * (*channel).length);
   }
}

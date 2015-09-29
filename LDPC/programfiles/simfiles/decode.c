#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>
#include "graphstruct.h"
#include "simulation.h"
#include "graphfunct.h"
#include "sub.h"
#include "decode.h"
#include "sim.h"
#include <sys/time.h>

/* global variables */
extern unsigned short channelsimrand[3];


/************************************/
/* sign of a double variable */
/************************************/
double sign(double x)
{
   if (x > 0)
      return 1.0;

   if (x < 0)
      return -1;
   else
      return 0.0;
}

/************************************/
/* minimum of two doubles */
/************************************/
double mind(double x, double y)
{
   if (x < y)
      return x;
   else
      return y;
}

/************************************/
/* maximum of two doubles */
/************************************/
double maxd(double x, double y)
{
   if (x > y)
      return x;
   else
      return y;
}

/************************************
* copy the received values into the *
* appropriate places in the graph   *
************************************/
void initializegraph(graphs * graph, simulations * simulation, double *received)
{
   int node, socket;

   for (node = 0; node < (*graph).vnodenum; node++)
      vnodevalue = *(received + node);

   switch ((*simulation).decodertype)
   {
   case GalA:
      // the following is not really needed
      // since for Gallager A we never use cnodevalue
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         cnodevalue = 1.0;
      }

      // set all messages to zero
      for (node = 0; node < (*graph).vnodenum; node++)
         for (socket = 0; socket < vnodedegree; socket++)
            vnodeedge.message = 0.0;
      for (node = 0; node < (*graph).cnodenum; node++)
         for (socket = 0; socket < cnodedegree; socket++)
            cnodeedge.message = 0.0;
      break;
   case Belief:
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         if (cnodedegree == 1)
            cnodevalue = EXPMAXMESSAGE;
         else
            cnodevalue = 1.0;
      }

      // set all messages to zero 
      for (node = 0; node < (*graph).vnodenum; node++)
         for (socket = 0; socket < vnodedegree; socket++)
            vnodeedge.message = 0.0;
      for (node = 0; node < (*graph).cnodenum; node++)
         for (socket = 0; socket < cnodedegree; socket++)
            cnodeedge.message = 1.0;
      break;
   default:
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"other than GAla and Belief propagation are implemented yet (initializegraph\n");
      exit(1);
      break;
   }
}

/************************************
* message map at the variable nodes *
************************************/
void variablemessagemap(graphs * graph, int iteration, int decodertype)
{
   int node, socket;

   int tonode, tosocket;

   double sum, newmessage;

   switch (decodertype)
   {
   case GalA:
      // loop over all nodes 
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         // take sum over all inputs 
         // NOTE: in iteration zero the internal messages are zero
         // so that the outgoing message at a variable node is 
         // equal to the received value
         sum = 0.0;
         for (socket = 0; socket < vnodedegree; socket++)
            sum += sign(vnodeedge.message);

         // now determine all the outgoing messages 
         for (socket = 0; socket < vnodedegree; socket++)
         {
            tonode = vnodeedge.dest;
            tosocket = vnodeedge.socket;
            if ((int)fabs(sum - vnodeedge.message) == (vnodedegree - 1))
               cnodetoedge.message = sign(sum - vnodeedge.message);
            else
               cnodetoedge.message = vnodevalue;
         }
      }
      break;
     case Belief:
      // loop over all nodes 
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         // take sum over all inputs 
         sum = vnodevalue;
         for (socket = 0; socket < vnodedegree; socket++)
            sum += vnodeedge.message;

         // now determine all the outgoing messages 
         for (socket = 0; socket < vnodedegree; socket++)
         {
            tonode = vnodeedge.dest;
            tosocket = vnodeedge.socket;

            newmessage = (sum - vnodeedge.message);

            // bound the size of the messages 
            if (fabs(newmessage) > MAXMESSAGE)
            {
               if (newmessage > 0)
                  cnodetoedge.message = EXPMAXMESSAGE;
               else
                  cnodetoedge.message = EXPNEGMAXMESSAGE;
            }
            else
               cnodetoedge.message = exp(newmessage);
         }
      }
      break;
   default:
      printf("Not implemented yet3!\n");
      break;
   }
}

/*********************************
 * message map at the check nodes *
 *********************************/
void checkmessagemap(graphs * graph, int iteration, int decodertype)
{
   int node, socket;

   int tonode, tosocket;

   int erasurenum, pos;

   double prod;

   switch (decodertype)
   {
   case GalA:
      // loop over all nodes 
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         // take the product over all inputs
         prod = 1.0;
         for (socket = 0; socket < cnodedegree; socket++)
         {
            prod *= cnodeedge.message;
         }

         // now determine all outgoing messages 
         for (socket = 0; socket < cnodedegree; socket++)
         {
            tonode = cnodeedge.dest;
            tosocket = cnodeedge.socket;
            vnodetoedge.message = prod * cnodeedge.message;
         }
      }
      break;
    case Belief:
      // loop over all nodes 
      for (node = 0; node < (*graph).cnodenum; node++)
      {
         // combine all non-erasure inputs 
         prod = EXPMAXMESSAGE;
         erasurenum = 0;
         for (socket = 0; socket < cnodedegree; socket++)
         {
            if (cnodeedge.message != 1.0)
               prod = (1.0 + prod * cnodeedge.message) / (prod + cnodeedge.message);
            else
               erasurenum++;
         }

         // take cnodevalue into account 
         // if cnodevalue != 1 this means that node has degree one
         if (cnodevalue != 1.0)
            prod = (1.0 + prod * cnodevalue) / (prod + cnodevalue);

         switch (erasurenum)
         {
         case 0:
            // now determine all outgoing messages 
            for (socket = 0; socket < cnodedegree; socket++)
            {
               tonode = cnodeedge.dest;
               tosocket = cnodeedge.socket;
               vnodetoedge.message =
                  log((1.0 - prod * cnodeedge.message) / (prod - cnodeedge.message));

               // check overflow 
               if (fabs(vnodetoedge.message) > MAXMESSAGE)
                  if (vnodetoedge.message > 0)
                     vnodetoedge.message = MAXMESSAGE;
                  else
                     vnodetoedge.message = -MAXMESSAGE;
            }
            break;
         case 1:
            // if there is exactly one erasure input then 
            // then all but one output is an erasure 
            for (socket = 0; socket < cnodedegree; socket++)
            {
               tonode = cnodeedge.dest;
               tosocket = cnodeedge.socket;

               if (cnodeedge.message != 1.0)
                  vnodetoedge.message = 0.0;
               else
               {
                  vnodetoedge.message = log(prod);

                  // check overflow 
                  if (fabs(vnodetoedge.message) > MAXMESSAGE)
                     if (vnodetoedge.message > 0)
                        vnodetoedge.message = MAXMESSAGE;
                     else
                        vnodetoedge.message = -MAXMESSAGE;
               }
            }
            break;
         default:
            // if we have two or more erasure inputs 
            // then all outputs are erasures 
            for (socket = 0; socket < cnodedegree; socket++)
            {
               tonode = cnodeedge.dest;
               tosocket = cnodeedge.socket;
               vnodetoedge.message = 0.0;
            }
            break;
         }
      }
      break;
   default:
      printf("Not implementd yet (checkmessagemap) !\n");
      break;
   }
}

/***********************************
 * make decisions on current graph *
***********************************/
void makedecisions(graphs * graph, double *decisions, int iteration, int decodertype)
{
   int node, socket;

   double sum;

   if (0)
   {
      printf("in makedecisions\n");
      for (node = 0; node < (*graph).vnodenum; node++)
         printf("%f[%f ]\n", *(decisions + node), vnodevalue);
      printf("\n");
   }
   switch (decodertype)
   {
   case GalA:
      // the decision rule for Gallager A is not so clear;
      // here we assume that we decide on the received value unless
      // all other messages agree, in which case we decide on
      // this common value;

      // loop over all nodes 
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         // take sum over all inputs 
         sum = 0.0;
         for (socket = 0; socket < vnodedegree; socket++)
            sum += vnodeedge.message;

         socket = 0;
         if (fabs(sum) == (double)vnodedegree)
            *(decisions + node) = vnodeedge.message;
         else
            *(decisions + node) = vnodevalue;
      }
      break;
   case Belief:
      // loop over all nodes 
      for (node = 0; node < (*graph).vnodenum; node++)
      {
         // take sum over all inputs 
         sum = vnodevalue;
         for (socket = 0; socket < vnodedegree; socket++)
            sum += vnodeedge.message;

         if (sum > 0.0)
            *(decisions + node) = 1.0;
         if (sum < 0.0)
            *(decisions + node) = -1.0;
         if (sum == 0.0)
            *(decisions + node) = 0.0;
      }

      break;
   default:
      printf("Not implemented yet (makedecision) !\n");
      break;
   }
}

/*****************************
 * general decoding function *
*****************************/
void decoding(graphs * graph, channels * channel, simulations * simulation,
              errorcounts * errorcount)
{
   int iteration;

   int errors, olderrors;

   int node;

   // initializations
   iteration = 0;
   errors = 0;
   olderrors = (*graph).vnodenum + 1;

   // check the initial number of errors 
   makedecisions(graph, (*channel).decisions, iteration, (*simulation).decodertype);
   (*errorcount).initialerrors = counterrors(channel);
   errors = (*errorcount).initialerrors;
   switch ((*channel).type)
   {
   case BSC:
   case AWGNC:
      for (iteration = 0; iteration < (*simulation).numiterations; iteration++)
      {
         checkmessagemap(graph, iteration, (*simulation).decodertype);
         variablemessagemap(graph, iteration, (*simulation).decodertype);
         makedecisions(graph, (*channel).decisions, iteration, (*simulation).decodertype);
      }
      break;
   case BEC:
      for (iteration = 0; iteration < (*simulation).numiterations; iteration++)
      {
         if (errors < olderrors)
         {
            checkmessagemap(graph, iteration, (*simulation).decodertype);
            variablemessagemap(graph, iteration, (*simulation).decodertype);
            makedecisions(graph, (*channel).decisions, iteration, (*simulation).decodertype);

            // in iteration 0, messages check ->var have no meaning
            // sent only var-node values from var -> check, 
            // therefore first meaningfull decisions from iteration 1 on.
            if (iteration >= 1)
               olderrors = errors;

            errors = counterrors(channel);
         }
         else
         {
            break;
         }
      }
      break;
   default:
		fprintf(stderr,"ERROR:\n");
      fprintf(stderr,"channels others than BSC, AWGN and BEC are not yet implemented (decoding)\n");
      exit(1);
      break;
   }
}

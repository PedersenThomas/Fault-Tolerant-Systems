#include <stdio.h>
#include <math.h>
#include <sys/time.h>
#include "sub.h"
#include <string.h>
#include <time.h>

int main(int argc, const char *argv[])
{

   int i, j;
   int flag, nberrors;
   int nbedges;
   double diff, totaldiff;      // variables for computing the variance
   int confint;
   double bitE, blockE, bitESquare;
   double totaldiffsquare;
   double parameter, h;
   int expurgation, expurgationEdges, nbblocks, nfiles, length, numloops;
   double currentChannelParam;
   char filename[100], dataPrefixe[100], currentChannelParamString[100];
   char resultfile[100];
   char fictif[1000];
   char fictif1;
   int fictifInt;
   double *hist;
   int *errperloop;
   FILE *fp, *fpout;
   FILE *fpdata;
   double sumErr, sumErrSquare;
   int loop;
   int nbold;

   /* printf("h' = %lf \n", -log(e)+log(1-e)); //printf("h = %lf \n", -e*log(e)-(1-e)*log(1-e));
      exit(5); */
        /*************************************/
   /* read input from file param */
        /*************************************/
   if (argc == 1)
   {
      if ((fp = fopen("param", "r")) == NULL)
      {
         fprintf(stderr, "I can't open the file `param'\n");
         return;
      }
   }
   else
   {
      if ((fp = fopen(*++argv, "r")) == NULL)
      {
         fprintf(stderr, "I can't open the indicated parameter file.\n");
         return;
      }
   }
   skip(fp);

   fscanf(fp, "%d", &confint);
   // printf("confint=%d\n", confint);
   skip(fp);

   fscanf(fp, "%d", &expurgation);
   fscanf(fp, "%d", &expurgationEdges);
   // printf("exp=%d\n", expurgation);
   skip(fp);

   fscanf(fp, "%s", &dataPrefixe);
   // printf("prefixe=%s\n", dataPrefixe);
   skip(fp);

   fscanf(fp, "%d", &nfiles);
   // printf("nfiles=%d\n", nfiles);
   skip(fp);

   double pb[nfiles], pB[nfiles], channelParam[nfiles], varBit[nfiles], varMessage[nfiles],
      countExperiment[nfiles];

   int nbSamples[nfiles];
   int flagfiles[nfiles];

   for (j = 0; j < nfiles; j++)
   {
      fscanf(fp, "%lf", &currentChannelParam);
      channelParam[j] = currentChannelParam;
      sprintf(currentChannelParamString, "%6.5lf", currentChannelParam);
      sprintf(filename, "data/%s_%s", dataPrefixe, currentChannelParamString);

      // printf("filename = %s\n",filename); 

      if ((fpdata = fopen(filename, "r")) == NULL)
      {
         fprintf(stderr, "%s cannot be opened\n", filename);
         flagfiles[j] = 0;
      }
      else
      {
         fprintf(stderr, "%s has been opened\n", filename);
         flagfiles[j] = 1;


         blockE = 0.0;
         bitE = 0.0;
         bitESquare = 0.0;

         // initializations for variance computation
         totaldiff = 0.0;
         totaldiffsquare = 0.0;
         varBit[j] = 0.0;
         varMessage[j] = 0.0;

         fscanf(fpdata, "%s", &fictif);

         while (strcmp(fictif, "LENGTH"))
         {
            fscanf(fpdata, "%s", &fictif);
         }

         fscanf(fpdata, "%d", &length);
         printf("length %d\n", length);
         skip(fpdata);

         while (strcmp(fictif, "EDGES"))
         {
            fscanf(fpdata, "%s", &fictif);
         }

         fscanf(fpdata, "%d", &nbedges);
         printf("nbedges %d\n", nbedges);
         skip(fpdata);


         while (strcmp(fictif, "LOOPS"))
         {
            fscanf(fpdata, "%s", fictif);
         }

         fscanf(fpdata, "%d", &numloops);
         skip(fpdata);

         if (j == 0)
         {
            hist = (double *)allocate(sizeof(double) * ((length + 1) * (nfiles)));
            for (i = 0; i < (length + 1) * nfiles; i++)
            {
               hist[i] = 0.0;
            }
         }

         nbblocks = 0;
         countExperiment[j] = 0.0;
         flag = feof(fpdata);
         loop = 0;
         while (flag == 0)
         {
            fscanf(fpdata, "%d", &nbblocks);
            fscanf(fpdata, "%d", &nberrors);

            // read in data for computing variance
         //   fscanf(fpdata, "%lf", &diff);
            // fscanf(fpdata, "%d", &fictifInt);

            *(hist + j * (length + 1) + nberrors) += 1.0;

            //if (nberrors >= expurgation && nberrors > 0)
            if (nberrors > 0)
            {
               blockE += 1.0;
               bitE += (double)nberrors / (double)length;
               // bitESquare+=(double)(nberrors*nberrors);
               bitESquare += pow((double)nberrors / (double)length, 2);
            }

            if (diff >= expurgationEdges)
            {
               totaldiff += diff;
               totaldiffsquare += diff * diff;
               countExperiment[j] += 1.0;
            }

            flag = feof(fpdata);
         }
         fclose(fpdata);
         // printf("count=%lf\n",countExperiment); 
         // compute cdf from right
         for (i = length - 1; i >= 0; i--)
         {
            *(hist + j * (length + 1) + i) += *(hist + j * (length + 1) + i + 1);
         }


         for (i = length; i >= 0; i--)
         {
            *(hist + j * (length + 1) + i) =
               *(hist + j * (length + 1) + i) / (*(hist + j * (length + 1)));
         }

         pB[j] = (double)blockE / ((double)nbblocks);
         pb[j] = (double)bitE / ((double)nbblocks);

         // pb[j]=(double)bitE/((double)nbblocks*(double)length);

         // compute variance of wrong bits
         varBit[j] = (bitESquare - bitE * bitE / (double)nbblocks) / ((double)nbblocks);
         // varBit[j]=(bitESquare-bitE*bitE/(double)nbblocks)/((double)nbblocks*(double)length*(double)length);

         // compute variance of normalized number of messages in error
         varMessage[j] =
            (totaldiffsquare -
             totaldiff * totaldiff / countExperiment[j]) / (countExperiment[j] * (double)nbedges);
         nbSamples[j] = nbblocks;
      }
   }

   plotHist(hist, length, nfiles, flagfiles);

   printf("\n\n");
   printf("Error probability: [channel parameter, pB, pb, number of samples, variance of pb]  \n");
   fpout = fopen(".errorprobability", "w");
   for (j = 0; j < nfiles; j++)
   {
      if (flagfiles[j] == 1)
      {
         printf("%lf  %.10lf  %.10lf  %d  %.8lf  pointconf\n", channelParam[j], pB[j], pb[j],
                nbSamples[j], varBit[j]);
         fprintf(fpout, "%lf  %.10lf  %.10lf  %d  %.8lf  pointconf\n", channelParam[j], pB[j],
                 pb[j], nbSamples[j], varBit[j]);
      }
   }
   fclose(fpout);

   // printf("nb = %d, pB = %lf, pb =%lf\n", nbblocks, pB, pb);
   // h=-parameter*log(parameter)-(1-parameter)*log(1-parameter);
   // BSC
   // printf("%lf %lf l \n", parameter, pB);
   // printf("%lf %lf %d pointconf \n", parameter, pB, nbblocks);
   // AWGN
   // printf("%lf entropyofcrosseps %lf l \n", parameter, pB);
   // printf("%lf entropyofcrosseps %lf %d pointconf \n", parameter, pB, nbblocks);


   printf("\n\n\nDONE\n");
   return 0;
}

/************************************/
/* Collection of useful subroutines */
/* for general use.  */
/* Created: 8/12/96 by Ruediger */
/* Modified: 8/12/96 by Ruediger */
/************************************/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/time.h>

/************************************/
/* Skip all data of a file until */
/* next C "end of comment" symbol */
/* appears. verbose=1 will result */
/* in all skipped data being */
/* printed.  */
/************************************/
void skip(fp)
     FILE *fp;
{
   char cold, cnew;

   cold = ' ';

   while ((cnew = getc(fp)) != '/' || (cold != '*'))
   {
      cold = cnew;
   }

   return;
}


        /************************************/
/* Allocate a pointer to void of */
/* size n bytes.  */
/************************************/
void *allocate(n)
     int n;
{
   void *p;

   if ((p = malloc(sizeof(int) * n)) == NULL)
   {
      printf("/***********************************/");
      printf("/*    W   A   R   N   I   N   G    */");
      printf("/***********************************/");
      printf("/*      Not sufficient memory!     */");
      printf("/***********************************/");
   }

   return p;
}


void plotHist(double *hist, int length, int nfiles, int flagfiles[])
{
   FILE *fp;
   double step;
   int i, j;

   step = floor((double)length / 10.0);

   system("rm histogram.ps");

   if ((fp = fopen("histogram.ps", "w")) == NULL)
   {
      fprintf(stderr, "I can't open a result\n");
      return;
   }

   fprintf(fp, "%s!PS-Adobe-2.0 EPSF-3.0\n", "%");
   fprintf(fp, "%s%sBoundingBox: 0 0 367.5 231\n", "%", "%");
   fprintf(fp, "%s%sEndComments\n", "%", "%");

   fprintf(fp, "/xstart  0  def\n");
   fprintf(fp, "/xend  %d  def \n", length);
   fprintf(fp, "/xticdelta  %.4lf  def\n", step);
   fprintf(fp, "/xgriddelta %.4lf  def\n\n", step);

   fclose(fp);

   system("cat programfiles/ppfiles/pstemplate.ps >> histogram.ps");

   if ((fp = fopen("histogram.ps", "a")) == NULL)
   {
      fprintf(stderr, "I can't open a result\n");
      return;
   }

   for (j = 0; j < nfiles; j++)
   {
      if (flagfiles[j] == 1)
      {
         fprintf(fp, "0  %lf  m\n", *(hist + j * (length + 1)));
         for (i = 1; i <= length; i++)
         {
            fprintf(fp, "%d  %.7f  l\n", i, *(hist + j * (length + 1) + i) + 0.0000001);
         }
         fprintf(fp, "stroke\n\n\n");
      }
   }

   fprintf(fp, "\nstroke\n%sEOF", "%");

   fclose(fp);
}

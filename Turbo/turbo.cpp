#include <stdio.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include "random.h"

const int SEED=1000;
unsigned int N=128;
unsigned int ITERATIONS=20;
double No;

// zero mean RV with variance as given
double gaussian(double variance);
// encodes mesg into parity1, and if force it true it modifies mesg to
// force the encoder to the zero state by the last bit.
void encode(bool *mesg,bool *parity1,unsigned size, bool force);
void interleave(bool *mesg, unsigned size);
// binary addition with no carry
bool add(bool a,bool b);
void interleave(bool *mesg, unsigned size);
void deinterleave(bool *mesg, unsigned size);
void deinterleavedouble(double *mesg, unsigned size);
void interleavedouble(double *mesg, unsigned size);
void createinterleave(unsigned size);
void createencodetable();
void inttobool(unsigned state, bool *array, unsigned size);
void booltoint(bool *array, unsigned size, unsigned *state);
unsigned decode (double *channelmesg, double *parity1, double *parity2, unsigned size, bool *mesg);

unsigned *interleavearray;
unsigned *deinterleavearray;

// global information about the encoder used by the encoder and the decoder
// routine
// how many states are in the encoder (a power of 2)
unsigned numstates;
// log2(numstates)
unsigned memory;
// [2] = input, [16] = current state, tostate[2][16] = next state
unsigned *tostate[2];
// [2] = last input, [16] = current state, fromstate[2][16] = previous state
unsigned *fromstate[2];
// [2] = input, [16] = current state, output[2][16] = output of encoder
bool *output[2];
Random before;

main()
{
   bool *mesg;
   bool *parity1;
   bool *parity2;
   double *channelmesg;
   double *channelparity1;
   double *channelparity2;

   interleavearray = new unsigned[N];
   deinterleavearray = new unsigned[N];
   mesg = new bool[N];
   parity1 = new bool[N];
   parity2 = new bool[N];
   channelmesg = new double[N];
   channelparity1 = new double[N];
   channelparity2 = new double[N];

   // only needs to be done once
   createencodetable();

   bool firstloop=true;

   // change if you want to loop for other values of dB
   for (double dB=-4.0;dB<=-4.0;dB+=0.5)
   {
	   unsigned totalN=0;
	   unsigned totalerrors=0;
	   unsigned numiter;
      unsigned totaliter=0;
      unsigned numloops=0;

		// load the previous state
      if (firstloop)
      {
      	FILE *fp;
         char line[30];

         firstloop = false;

			if ((fp = fopen("laststate","r")) != NULL)
			{
	         fgets(line,30,fp);
	         dB = atof(line);
	         fgets(line,30,fp);
	         totalerrors = atoi(line);
	         fgets(line,30,fp);
	         totalN = atoi(line);
	         fgets(line,30,fp);
	         totaliter = atoi(line);
	         fgets(line,30,fp);
	         numloops = atoi(line);
            fclose(fp);
         }
		}

		// dB = 10*log(Eb/No) where Eb is 1
      No = 1/pow(10.0,dB/10.0);

      bool keepgoing=true;

	   do
	   {
			for (int x=0;x<N;x++)
		   	mesg[x] = before.boolrandom();

         // create a random interleaver for each decode trial
		   createinterleave(N);

		   encode(mesg,parity1,N,true);
		   interleave(mesg,N);
		   encode(mesg,parity2,N,false);
		   // deinterleave for the decoder
		   deinterleave(mesg,N);

			for (int x=0;x<N;x++)
		   {
		   	mesg[x] ? channelmesg[x] = 1.0 : channelmesg[x]=-1.0;
		      parity1[x] ? channelparity1[x]=1.0 : channelparity1[x]=-1.0;
		      parity2[x] ? channelparity2[x]=1.0 : channelparity2[x]=-1.0;
		   }

		   // add gaussian noise, mean=0, variance=No/2
			for (int x=0;x<N;x++)
		   {
		    	channelmesg[x] += gaussian(No/2);
				channelparity1[x] += gaussian(No/2);
		      channelparity2[x] += gaussian(No/2);
		   }

		   numiter = decode(channelmesg, channelparity1, channelparity2,N,mesg);

		   unsigned numerrors=0;

		   for (int x=0;x<N;x++)
		   {
		   	bool temp = channelmesg[x] == 1 ? true : false;

		      if (mesg[x] != temp)
		   		numerrors++;
		   }

	      totalerrors += numerrors;
	      totalN += N;
         totaliter += numiter;
         numloops++;

	      char name[30];

			// open a file for output
	      sprintf (name,"%dN%lfdB",N,dB);

	      FILE *fp;

	      // if file exists, we will append to it
	      fp = fopen(name,"a");

			printf ("N=%d, e=%d, totITER=%d, numITER=%d, No=%lf, tote=%d, totn=%d, dB=%lf\n",N,numerrors,ITERATIONS,numiter,No,totalerrors,totalN,dB);
         fprintf (fp,"N=%d, e=%d, totITER=%d, numITER=%d, No=%lf, tote=%d, totn=%d, dB=%lf\n",N,numerrors,ITERATIONS,numiter,No,totalerrors,totalN,dB);

	      fclose(fp);

         // save this state in case we need to start over again
         fp = fopen ("laststate","w");

			fprintf(fp,"%lf\n",dB);
         fprintf(fp,"%u\n",totalerrors);
         fprintf(fp,"%u\n",totalN);
			fprintf(fp,"%u\n",totaliter);
			fprintf(fp,"%u\n",numloops);

         fclose(fp);

			// a number of conditions for stopping the simulation for this dB value
			if (totalerrors> 1000)
         	keepgoing = false;
         else if (totalN>500000 && totalerrors > 100)
         	keepgoing = false;
         else if (totalN>1E6 && totalerrors > 10)
         	keepgoing = false;
         else if (totalN>5E6 && totalerrors > 5)
				keepgoing = false;
			else if (totalN>7.5E6 && totalerrors > 3)
				keepgoing = false;
			else if (totalN>1E7)
         	keepgoing=false;
		}
		while (keepgoing == true); // 10 million is as high as I'm willing to go
      // 1000 errors should be enough for good stats.

      // add a summary of the last run to the summary file
		FILE *fp;

      fp = fopen("summary","a");

	   printf ("dB = %lf, N=%ld, error=%ld, avgiter = %lf, interleaver size=%d, original encoder\n",dB, totalN,totalerrors,(double)totaliter/(double)numloops,N);
      fprintf(fp,"dB = %lf, N=%ld, error=%ld, avgiter = %lf, interleaver size=%d, original encoder\n",dB, totalN,totalerrors,(double)totaliter/(double)numloops,N);

	   fclose(fp);
	}

   delete interleavearray;
   delete deinterleavearray;
   delete mesg;
   delete parity1;
   delete parity2;
   delete channelmesg;
   delete channelparity1;
   delete channelparity2;

   // use repeat n code, n=3 in this case.  For large probability of error this code
   // will beat turbo code, turbo is way better for lesser probability of error
/*
	for (int x=0;x<N;x++)
   	mesg[x] = parity1[x] = parity2[x] = before.boolrandom();

	for (int x=0;x<N;x++)
   {
   	mesg[x] ? channelmesg[x] = 1.0 : channelmesg[x]=-1.0;
      parity1[x] ? channelparity1[x]=1.0 : channelparity1[x]=-1.0;
      parity2[x] ? channelparity2[x]=1.0 : channelparity2[x]=-1.0;
   }

	numerrors=0;
	// add white guassian noise
  	for (int x=0;x<N;x++)
   {
   	bool temp;

   	channelmesg[x] += gaussian(No/2);

      if (channelmesg[x] > 0)
      	temp = true;
      else
      	temp = false;

      if (mesg[x] != temp)
         numerrors++;

		channelparity1[x] += gaussian(No/2);
      channelparity2[x] += gaussian(No/2);
   }

   printf ("%ld\n",numerrors);

   numerrors=0;
   // make decisions based on majority rules
   for (int x=0;x<N;x++)
   {
   	int count=0;

		if (channelmesg[x] > 0)
      	count ++;
      else
      	count--;

      if (channelparity1[x] > 0)
      	count++;
      else
      	count--;

      if (channelparity2[x] > 0)
      	count++;
      else
      	count--;

      bool temp;

      if (count > 0)
			temp = true;
      else
      	temp = false;

      if(mesg[x] != temp)
      	numerrors ++;
   }

   printf ("%ld\n",numerrors);
*/
}

void createencodetable()
{
	bool *boolstate;
   bool *newstate;

	numstates = 16;
   memory = 4;

   // create arrays used by encode and decode
   output[0] = new bool[numstates];
   output[1] = new bool[numstates];
   fromstate[0] = new unsigned[numstates];
   fromstate[1] = new unsigned[numstates];
   tostate[0] = new unsigned[numstates];
   tostate[1] = new unsigned[numstates];

   boolstate = new bool[memory];
   newstate = new bool[memory];

	for (unsigned input=0;input<2;input++)
		for (unsigned intstate=0;intstate<numstates;intstate++)
      {
      	bool boolinput = (input == 0) ? false : true;

			inttobool(intstate,boolstate,memory);

			// calculate output due to the input
			output[input][intstate] = add(boolinput,boolstate[0]);
			output[input][intstate] = add(output[input][intstate],boolstate[3]);
			output[input][intstate] = add(output[input][intstate],boolstate[1]);
			output[input][intstate] = add(output[input][intstate],boolstate[2]);
			output[input][intstate] = add(output[input][intstate],boolstate[3]);

			// calculate new states
			newstate[3] = boolstate[2];
	     	newstate[2] = boolstate[1];
			newstate[1] = boolstate[0];
         newstate[0] = add(add(boolinput,boolstate[0]),boolstate[3]);
         // from s' to s
      	booltoint (newstate,memory,&tostate[input][intstate]);
         // from s to s'
         fromstate[input][tostate[input][intstate]] = intstate;
      }

   delete boolstate;
   delete newstate;
}


/*
void createencodetable()
{
	numstates = 128;
   memory = 7;

   // create arrays used by encode and decode
   output[0] = new bool[numstates];
   output[1] = new bool[numstates];
   fromstate[0] = new unsigned[numstates];
   fromstate[1] = new unsigned[numstates];
   tostate[0] = new unsigned[numstates];
   tostate[1] = new unsigned[numstates];

   yesno[0] = new bool[numstates];
   yesno[1] = new bool[numstates];

   bool *yesno[2];
   Random r;

	for (unsigned input=0;input<2;input++)
		for (unsigned intstate=0;intstate<numstates;intstate++)
			yesno[input][intstate] = false;

	for (unsigned input=0;input<2;input++)
		for (unsigned intstate=0;intstate<numstates;intstate++)
      {
      	bool output
         unsigned nextstate;

      	do
         {
         	output = r.boolrandom();
            nextstate=r.longrandom();
         }
         while(yesno[input][nextstate] == true);

         yesno[input][nextstate]=false;

	while (done < 2*numstates)
   {
   	bool input = a.boolrandom();
      unsigned state = a.unsignedrandom(numstates);

      if(yesno[input][state] == false)
      {
      	yesno[input][state] = true;
         done++;
         tostate[input]




	for (unsigned input=0;input<2;input++)
		for (unsigned intstate=0;intstate<numstates;intstate++)
      {
      	bool boolinput = (input == 0) ? false : true;

			inttobool(intstate,boolstate,memory);

			// calculate output due to the input
			output[input][intstate] = add(boolinput,boolstate[0]);
			output[input][intstate] = add(output[input][intstate],boolstate[3]);
			output[input][intstate] = add(output[input][intstate],boolstate[1]);
			output[input][intstate] = add(output[input][intstate],boolstate[2]);
			output[input][intstate] = add(output[input][intstate],boolstate[3]);

			// calculate new states
			newstate[3] = boolstate[2];
	     	newstate[2] = boolstate[1];
			newstate[1] = boolstate[0];
         newstate[0] = add(add(boolinput,boolstate[0]),boolstate[3]);
         // from s' to s
      	booltoint (newstate,memory,&tostate[input][intstate]);
         // from s to s'
         fromstate[input][tostate[input][intstate]] = intstate;
      }

   delete boolstate;
   delete newstate;
}
*/

void inttobool(unsigned state, bool *array, unsigned size)
{
	for (unsigned x=0;x<size;x++)
   {
   	unsigned next = state >> 1;

		if ((next << 1) == state)
      	array[x] = false;
      else
      	array[x] = true;

      state = next;
   }
}

void booltoint(bool *array, unsigned size, unsigned *state)
{
	*state = 0;

   for (int x=0;x<size;x++)
   	if (array[x] == true)
      	(*state) |= (1 << x);
}

unsigned decode (double *mesg, double *parity1, double *parity2, unsigned size, bool *boolmesg)
{
	static double **a[2];
   static double **b[2];
   static double *L[2];
   static double **gamma[2][2];
   static double **gammaE[2][2];
   static bool initialized=false;
   static unsigned lastsize=0;
   unsigned returnvalue=ITERATIONS;

   // minimize new's and delete's to only when needed
   if (size != lastsize && initialized == true)
   {
	   // delete all the arrays and rebuild
	   for (int y=0;y<2;y++)
	   	for (int x=0;x<2;x++)
	      {
	         for (int z=0;z<lastsize;z++)
	         {
	  	      	delete gamma[y][x][z];
	  	      	delete gammaE[y][x][z];
	         }

	      	delete gamma[y][x];
	      	delete gammaE[y][x];
	  		}

	   // create L[encoder #]
	   for (int y=0;y<2;y++)
	      delete L[y];

	   // create alpha[encoder #][k][state]
	   for (int x=0;x<2;x++)
	   {
	   	for (int y=0;y<lastsize;y++)
	      {
	         delete a[x][y];
	         delete b[x][y];
		   }

	      delete a[x];
	      delete b[x];
		}
   }

   if (initialized == false || size != lastsize)
   {
   	initialized = true;
      lastsize = size;

	   // create the arrays dynamically at runtime, delete at end of routine
	   // create gamma[encoder #][uk][k][state]
	   for (int y=0;y<2;y++)
	   	for (int x=0;x<2;x++)
	      {
	      	gamma[y][x] = new double*[size];
	         gammaE[y][x] = new double*[size];
	         for (int z=0;z<size;z++)
	         {
	  	      	gamma[y][x][z] = new double[numstates];
	            gammaE[y][x][z] = new double[numstates];
	         }
	  		}

	   // create L[encoder #]
	   for (int y=0;y<2;y++)
	      L[y] = new double[size];

	   // create alpha[encoder #][k][state]
	   for (int x=0;x<2;x++)
	   {
	      a[x] = new double*[size];
	      b[x] = new double*[size];
	      // each Yk has 'numstates' values of gamma
	   	for (int y=0;y<size;y++)
	      {
	         a[x][y] = new double[numstates];
	         b[x][y] = new double[numstates];
		   }
		}
   }

   // initialization of iteration arrays
   for (int x=0;x<numstates;x++)
   {
   	a[0][0][x] = b[0][size-1][x] = a[1][0][x] = (x==0) ? 1.0 : 0.0;
      // extrinsic information from 2-1
   }

	// initialization of extrinsic information array from decoder 2, used in decoder 1
   for (int x=0;x<size;x++)
   	L[1][x] = 0.0;

	// 4*Eb/No
   double Lc = (4.0*1.0)/No;

   for (int c=0;c<ITERATIONS;c++)
   {
		// k from 0 to N-1 instead of 1 to N
	   for (int k=0;k<size;k++)
	   {
	   	// calculate the gamma(s',s);
	   	for (int input=0;input<2;input++)
			{
				double uk = (input == 0) ? -1.0 : 1.0;

	      	for (int s=0;s<numstates;s++)
	         {
	         	double xk = (output[input][s] == 0) ? -1.0 : 1.0;

		      	gammaE[0][input][k][s]=exp(0.5*Lc*parity1[k]*xk);
		      	gamma[0][input][k][s]=exp(0.5*uk*(L[1][k]+Lc*mesg[k]))*gammaE[0][input][k][s];
	         }
	      }
	   }

	   // calculate the alpha terms
	   // from 1 to N-1, 0 is precalculated, N is never used
		for (int k=1;k<size;k++)
	   {
			double temp=0;

	   	// calculate denominator
	      for (int state=0;state<numstates;state++)
		     	temp += a[0][k-1][fromstate[0][state]]*gamma[0][0][k-1][fromstate[0][state]] + a[0][k-1][fromstate[1][state]]*gamma[0][1][k-1][fromstate[1][state]];

	     	for (int state=0;state<numstates;state++)
				a[0][k][state] = (a[0][k-1][fromstate[0][state]]*gamma[0][0][k-1][fromstate[0][state]] + a[0][k-1][fromstate[1][state]]*gamma[0][1][k-1][fromstate[1][state]])/temp;
		}

		// from N-1 to
		for (int k=size-1;k>=1;k--)
	   {
	   	double temp=0;

	   	// calculate denominator
	      for (int state=0;state<numstates;state++)
		     	temp += a[0][k][fromstate[0][state]]*gamma[0][0][k][fromstate[0][state]] + a[0][k][fromstate[1][state]]*gamma[0][1][k][fromstate[1][state]];

	   	for (int state=0;state<numstates;state++)
	      	b[0][k-1][state] = (b[0][k][tostate[0][state]]*gamma[0][0][k][state] + b[0][k][tostate[1][state]]*gamma[0][1][k][state])/temp;
	   }

	   for (int k=0;k<size;k++)
	   {
	   	double min=0;

	      // find the minimum product of alpha, gamma, beta
	      for (int u=0;u<2;u++)
	      	for (int state=0;state<numstates;state++)
	         {
	         	double temp=a[0][k][state]*gammaE[0][u][k][state]*b[0][k][tostate[u][state]];

	            if ((temp < min && temp != 0)|| min == 0)
	            	min = temp;
	         }

	      // if all else fails, make min real small
	      if (min == 0 || min > 1)
	      	min = 1E-100;

	      double topbottom[2];

		   for (int u=0;u<2;u++)
			{
		   	topbottom[u]=0.0;

	   	   for(int state=0;state<numstates;state++)
	         	topbottom[u] += (a[0][k][state]*gammaE[0][u][k][state]*b[0][k][tostate[u][state]]);
			}

	      if (topbottom[0] == 0)
	      	topbottom[0] = min;
	      else if (topbottom[1] == 0)
	      	topbottom[1] = min;

	      L[0][k] = (log(topbottom[1]/topbottom[0]));
	   }

	   interleavedouble(L[0],size);
	   // remember to deinterleave for next iteration
	   interleavedouble(mesg,size);

	   // start decoder 2
	   // code almost same as decoder 1, could combine code into one but too lazy
	   for (int k=0;k<size;k++)
	   {
	   	// calculate the gamma(s',s);
	   	for (int input=0;input<2;input++)
			{
				double uk = (input == 0) ? -1.0 : 1.0;

	      	for (int s=0;s<numstates;s++)
	         {
	         	double xk = (output[input][s] == 0) ? -1.0 : 1.0;

		      	gammaE[1][input][k][s]=exp(0.5*Lc*parity2[k]*xk);
		      	gamma[1][input][k][s]=exp(0.5*uk*(L[0][k]+Lc*mesg[k]))*gammaE[1][input][k][s];
	         }
	      }
	   }

	   // calculate the alpha terms
		for (int k=1;k<size;k++)
	   {
			double temp=0;

	   	// calculate denominator
	      for (int state=0;state<numstates;state++)
		     	temp += a[1][k-1][fromstate[0][state]]*gamma[1][0][k-1][fromstate[0][state]] + a[1][k-1][fromstate[1][state]]*gamma[1][1][k-1][fromstate[1][state]];

	     	for (int state=0;state<numstates;state++)
				a[1][k][state] = (a[1][k-1][fromstate[0][state]]*gamma[1][0][k-1][fromstate[0][state]] + a[1][k-1][fromstate[1][state]]*gamma[1][1][k-1][fromstate[1][state]])/temp;
		}

	   // in the first iteration, set b[1][N-1] = a[1][N-1] for decoder 2
	   // this decoder can't be terminated to state 0 because of the interleaver
	   // the performance loss is supposedly neglible.
      if (c==0)
	   {
			double temp=0;

		  	// calculate denominator
		   for (int state=0;state<numstates;state++)
			  	temp += a[1][size-1][fromstate[0][state]]*gamma[1][0][size-1][fromstate[0][state]] + a[1][size-1][fromstate[1][state]]*gamma[1][1][size-1][fromstate[1][state]];

		  	for (int state=0;state<numstates;state++)
				b[1][size-1][state] = (a[1][size-1][fromstate[0][state]]*gamma[1][0][size-1][fromstate[0][state]] + a[1][size-1][fromstate[1][state]]*gamma[1][1][size-1][fromstate[1][state]])/temp;
	   }

		for (int k=size-1;k>=1;k--)
	   {
	   	double temp=0;

	   	// calculate denominator
	      for (int state=0;state<numstates;state++)
		     	temp += a[1][k][fromstate[0][state]]*gamma[1][0][k][fromstate[0][state]] + a[1][k][fromstate[1][state]]*gamma[1][1][k][fromstate[1][state]];

	   	for (int state=0;state<numstates;state++)
	      	b[1][k-1][state] = (b[1][k][tostate[0][state]]*gamma[1][0][k][state] + b[1][k][tostate[1][state]]*gamma[1][1][k][state])/temp;
	   }

	   for (int k=0;k<size;k++)
	   {
	   	double min = 0;

	      // find the minimum product of alpha, gamma, beta
	      for (int u=0;u<2;u++)
	      	for (int state=0;state<numstates;state++)
	         {
	         	double temp=a[1][k][state]*gammaE[1][u][k][state]*b[1][k][tostate[u][state]];

	            if ((temp < min && temp != 0)|| min == 0)
	            	min = temp;
	         }
	      // if all else fails, make min real small
	      if (min == 0 || min > 1)
	      	min = 1E-100;

			double topbottom[2];

		   for (int u=0;u<2;u++)
			{
		   	topbottom[u]=0.0;

	   	   for(int state=0;state<numstates;state++)
					topbottom[u] += (a[1][k][state]*gammaE[1][u][k][state]*b[1][k][tostate[u][state]]);
			}

	      if (topbottom[0] == 0)
	      	topbottom[0] = min;
	      else if (topbottom[1] == 0)
	      	topbottom[1] = min;

	      L[1][k] = (log(topbottom[1]/topbottom[0]));
	   }

		deinterleavedouble(mesg,size);
	   deinterleavedouble(L[1],size);
		// get L[0] back to normal after decoder 2
		deinterleavedouble(L[0],size);

      bool temp=true;
      for (int k=0;k<size;k++)
			if (boolmesg[k] != ((Lc*mesg[k] + L[0][k] + L[1][k] > 0.0) ? true : false))
				temp = false;

      // we can quit prematurely since it has been decoded
      if (temp==true)
      {
      	returnvalue = c;
			c=ITERATIONS;
      }
	}
	// end decoder 2

   // make decisions
   for (int k=0;k<size;k++)
		if ((Lc*mesg[k] + L[0][k] + L[1][k]) > 0)
      	mesg[k] = 1.0;
      else
      	mesg[k] = -1.0;

      return returnvalue;
}

void deinterleavedouble(double *mesg, unsigned size)
{
	double *temp;

   temp = new double[size];

   for (int x=0;x<size;x++)
   	temp[x] = mesg[x];

	for (int x=0;x<size;x++)
		mesg[deinterleavearray[x]] = temp[x];

   delete temp;
}

void interleavedouble(double *mesg, unsigned size)
{
	double *temp;

   temp = new double[size];

   for (int x=0;x<size;x++)
   	temp[x] = mesg[x];

   for (int x=0;x<size;x++)
   	mesg[interleavearray[x]] = temp[x];

   delete temp;
}

void interleave(bool *mesg,unsigned size)
{
	bool *temp;

   temp = new bool[size];

   for (int x=0;x<size;x++)
   	temp[x] = mesg[x];

   for (int x=0;x<size;x++)
   	mesg[interleavearray[x]] = temp[x];

   delete temp;
}

void deinterleave(bool *mesg,unsigned size)
{
	bool *temp;

   temp = new bool[size];

   for (int x=0;x<size;x++)
   	temp[x] = mesg[x];

	for (int x=0;x<size;x++)
		mesg[deinterleavearray[x]] = temp[x];

   delete temp;
}

void createinterleave(unsigned size)
{
   bool *yesno;

   yesno = new bool[size];

   for (int x=0;x<N;x++)
   	yesno[x]=false;

	// create an interleave array
   for (int x=0;x<N;x++)
   {
   	unsigned  val;

      do
      {
      	val=before.longrandom(N);
      }
		while(yesno[val] == true);

      yesno[val] = true;
		interleavearray[x] = val;
      deinterleavearray[val] = x;
	}

   delete yesno;
}

void encode(bool *mesg,bool *parity,unsigned size, bool force)
{

	unsigned state=0;

  	for (int x=0;x<size;x++)
	{
		// force the encoder to zero state at the end
     	if (x>=size-memory && force)
      {
			if (tostate[0][state]&1)
         	mesg[x] = true;
         else
         	mesg[x] = false;
      }

		// can't assume the bool type has an intrinsic value of 0 or 1
		// may differ from platform to platform
      int uk = mesg[x] ? 1 : 0;

		// calculate output due to new mesg bit
      parity[x] = output[uk][state];
      // calculate the new state
		state = tostate[uk][state];
	}
}

bool add(bool a, bool b)
{
   return a==b ? false : true;
}

double gaussian(double variance)
{
	// static becuase we don't want to have it initialized each time we go in
   double returnvalue=0;
   double k;

	k = sqrt(variance/2.0);

   // add 24 uniform RV to obtain a simulation of normality
   for (int x=0;x<24;x++)
		returnvalue += before.doublerandom();

	return k*(returnvalue-0.5*24);
}

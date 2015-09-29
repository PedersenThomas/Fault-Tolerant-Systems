Turbo Code simulator

Mathys Walma (walma@sce.carleton.ca), June 29, 1998

Based on paper:
[1] William E. Ryan, "A Turbo Code Tutorial,"
http://telsat.nmsu.edu/~wryan/turbo2c.ps

A copy of the paper is included in the archive.

There is a lot of diagnostic code in the main() part of the program.  Also
included in the main() is code so that the simulation can start where it left
off if the program is stopped for some reason.  After each turbo code run, the
important variables are saved in a file to facilitate this.

The main routine that implements the BCJR decode algorithm is decode().
Thanks to [1] for the pseudo-code.  Decode takes a number of arguments, most
are self explanatory.

unsigned decode (double *channelmesg, double *parity1, double *parity2,
unsigned size, bool *mesg);

* channelmesg is the AWGN corrupted message
* parity1 is the AWGN corrupted parity bits from the first recursive
convolutional encoder
* parity2 is the AWGN corrupted parity bits from the second recursive
convolutional encoder and interleaver combination
* size is the size of the interleaver
* mesg is the orignal message that is compared after each iteration to the
decoded message.  If they are the same, the decoder exits early
* the value returned is the number of iterations done before successful
decoding

After each iteration encode() checks the decoded message with the original
message to see if the message has been decoded properly.  If it has, no more
iterations are done, thereby saving time in the simulation.  This also
provides useful information as to the distribution of the number of iterations
for different Eb/No values.

The Eb/No value used in the program (the dB variable) isn't the one usually
quoted in the literature.  The Eb/No in the literature is for uncoded bits.
The Eb/No in the program is for the coded bits.  For example. in a rate 1/3
code the values would be 4.77 dB apart [10*log(1/3)].

The decode() routine isn't very efficient.  During debugging I noticed that in
the 'for' loops often the only thing that would change was a sign bit between
loops, and so values could be precalculated and then based on the sign bit the
appropriate value could be chosen.

As well, the code for parity1 and parity2 information could have been combined
into one, making it easy to extend the decoder into a lower rate decoder
(ie. 1/4 or 1/5).  As it is, the code for parity1 decoding and parity2
decoding is almost identical.

The recursive convolutional encoder can be changed with minimal effort, a few
global variables, and of course the createencodetable routine need changing.
The createencodetable routine creates a look up table that represents the
state transition diagram of the encoder.  The encode/decode routines then uses
this look up table to encode/deocde blocks of information.

The interleaver size can be modified by changing the global variable N.

SEED is the random number seed and can be used to get reproducable results.

The random number generator comes from Numberical Recipes in C and is included
as a C++ object in a separate file (random.cpp ripped from another project).

The Gaussian generator gaussian(double variance) isn't terribly efficient, but
is numerically valid after comparing to theoretical values for variance, mean,
and examining histograms of generated values.  It uses the Central Limit
Theorem, by adding 24 uniform random numbers to approximate a gaussian random
variable.  Other routines were tried but didn't come as close to the
theoretical values that this routine did.  The numbers are always generated
with a zero mean.

The program was compiled as is with Borland C++ 5.0 under Windows 95 (as a
32bit console appliation), and also with gcc under SUN OS with no problems
(math library needed for gcc {-lm}).

I am always interested in hearing from people that used this program.

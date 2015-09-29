all: sim mackay2lth

%debug=-g

sim:sim.c sim.h graph.c sub.c sub.h channel.c channel.h decode.c decode.h encode.c encode.h simulation.h readparam.c readparam.h evaluation.c evaluation.h 
	gcc -g -o sim sim.c graph.c sub.c channel.c decode.c encode.c readparam.c evaluation.c  -lm  -O3
	cp sim ../../sim

mackay2lth:mackay2lth.c mackay2lth.h sub.c sub.h graphstruct.h graphfunct.h 
	gcc -g -o mackay2lth mackay2lth.c  sub.c graph.c -lm  -O3

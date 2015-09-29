all: pp 

debug=-g

pp: pp.c sub.c sub.h 
	gcc $(debug) -o pp pp.c sub.c -lm  
	cp pp ../../pp
	cp param ../../param_pp

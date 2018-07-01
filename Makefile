
FLAGS= -lglut -lGLEW -lGL


all : bezier
	./bezier.out

0: bezier
	./bezier.out 0

1: bezier
	./bezier.out 1

2: bezier
	./bezier.out 2


bezier : Bezier.cpp
	g++ Bezier.cpp -o bezier.out $(FLAGS)
	
joao: Bezier_joao.cpp
	g++ Bezier_joao.cpp -o joao.out $(FLAGS)
	./joao.out

yasmin: Bezier_yasmin.cpp
	g++ Bezier_yasmin.cpp -o yasmin.out $(FLAGS)
	./yasmin.out

clear:
	rm *.out
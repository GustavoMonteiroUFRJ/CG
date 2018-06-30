
FLAGS= -lglut -lGLEW -lGL


all : gustavo
	./gustavo.out

0: gustavo
	./gustavo.out 0

1: gustavo
	./gustavo.out 1

2: gustavo
	./gustavo.out 2

	
gustavo : Bezier_gustavo.cpp
	g++ Bezier_gustavo.cpp -o gustavo.out $(FLAGS)
	
joao: Bezier_joao.cpp
	g++ Bezier_joao.cpp -o joao.out $(FLAGS)
	./joao.out

yasmin: Bezier_yasmin.cpp
	g++ Bezier_yasmin.cpp -o yasmin.out $(FLAGS)
	./yasmin.out

clear:
	rm *.out
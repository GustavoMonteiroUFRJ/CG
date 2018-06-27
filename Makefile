
FLAGS= -lglut -lGLEW -lGL


all : gustavo

gustavo : Bezier_gustavo.cpp
	g++ Bezier_gustavo.cpp -o gustavo.out $(FLAGS)
	./gustavo.out


joao: Bezier_joao.cpp
	g++ Bezier_joao.cpp -o joao.out $(FLAGS)
	./joao.out

yasmin: Bezier_yasmin.cpp
	g++ Bezier_yasmin.cpp -o yasmin.out $(FLAGS)
	./yasmin.out

clear:
	rm *.out
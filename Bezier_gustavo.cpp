#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define NPOINTS 20 //numero de pontos adicionados pelo usuario
#define DIVBEZIER 20  //numero de pontos na curva de bezier

#define ALTURA_BLOCO 0.15
#define LARGURA_BLOCO 0.15
int quantidade_de_blocos;

float obistaculos[200][4];
#define X 0
#define Y 1
#define ALTURA 2
#define LARGURA 3

#define FAZE 1


const float t = 1 / (float) DIVBEZIER;
GLfloat points_x[NPOINTS]; //Vetor de coordenadas x dos pontos
GLfloat points_y[NPOINTS]; //Vetor de coordenadas  dos pontos
GLfloat curva_x[NPOINTS - 2][DIVBEZIER]; //Vetor de coordenadas x da curva
GLfloat curva_y[NPOINTS - 2][DIVBEZIER]; //Vetor de coordenadas y da curva
GLint point_idx = -1; //Contador de pontos
GLint bezier_idx = -1; //Contador de curvas



void imprime_bloco(float x, float y, float altura, float largura )
{
		glBegin(GL_QUADS);
		glVertex2d(x, y);
		glVertex2d(x + largura, y);
		glVertex2d(x + largura, y + altura);
		glVertex2d(x, y + altura);
		glEnd();
}

int ponto_invalido(float x, float y)
{
	return 0; // essa funcao existe para checar se o bloco gerado aleatoriamente esta em um lugar valido ou nao.
}

void gera_blocos_aleatorios(){
	// iniciando o gerador de numeros aleatórios
	srand(time(0));
	
	quantidade_de_blocos = 10;
	float x,y;
	for(int i = 0; i < quantidade_de_blocos; i++)
	{	
		do
		{	
			x = 1.0*rand()/RAND_MAX; // nuero entre 0 e 1 
			y = 1.0*rand()/RAND_MAX; // nuero entre 0 e 1 
			x = 2*x -1; // nuero entre -1 e 1 
			y = 2*y -1;	// nuero entre -1 e 1

		} while (ponto_invalido(x,y));

		obistaculos[i][X] = x;
		obistaculos[i][Y] = y;
		obistaculos[i][ALTURA] = ALTURA_BLOCO;
		obistaculos[i][LARGURA] = LARGURA_BLOCO;
	}
}


void gera_blocos_faze_1(){	

 	quantidade_de_blocos = 2;

	obistaculos[0][X] = -0.7;
	obistaculos[0][Y] = -0.4;
	obistaculos[0][ALTURA] = 0.7;
	obistaculos[0][LARGURA] = 0.6;
	
	obistaculos[1][X] = 0.2;
	obistaculos[1][Y] = -0.2;
	obistaculos[1][ALTURA] = 0.7;
	obistaculos[1][LARGURA] = 0.6;
}


//Desenha os obstaculos no mapa e a area final
void mapa(void)
{
	int i = 2, j = 0, end = 0;
	int aux1, aux2;
	float cxj, cyj;

	// ponto inicial
	glColor3f(0.0f, 0.0f, 1.0f);
	imprime_bloco(0.8, -1.0, 0.2, 0.2);

	// ponto final
	glColor3f(0.0f, 1.0f, 0.0f);
	imprime_bloco(-1.0,0.8,0.2,0.2);

	glColor3f(1.0, 0.0, 0.0);
	for (i=0 ; i < quantidade_de_blocos; i++)
	{
		imprime_bloco(obistaculos[i][X], obistaculos[i][Y], obistaculos[i][ALTURA], obistaculos[i][LARGURA]);
	}
}


void init_game(int faze)
{
	
	switch (faze)
	{
		case 0:
			gera_blocos_aleatorios();
			break;
	
		case 1:
			gera_blocos_faze_1();
			break;
		
		default:
			printf("Faze escolida nao reconhecida");
	}

}


//YASMIM

//Remove curvas de Bezier
void removeBezier() {
	for (int i = 0; i <= DIVBEZIER; i++) {
		curva_x[bezier_idx][i] = 0.0;
		curva_y[bezier_idx][i] = 0.0;
	}
}




//Aplica Bezier Cubico
void bezierQuad(GLfloat p1_x, GLfloat p2_x, GLfloat p3_x, GLfloat p1_y, GLfloat p2_y, GLfloat p3_y) {
	for (int i = 0; i <= DIVBEZIER; i++) {
		curva_x[bezier_idx][i] = (pow(1 - t*i, 2) * p1_x) +
			(2 * t*i * (1 - t*i) * p2_x) + (pow(t*i, 2) * p3_x);
		curva_y[bezier_idx][i] = (pow(1 - t * i, 2) * p1_y) +
			(2 * t*i * (1 - t*i) * p2_y) + (pow(t*i, 2) * p3_y);
	}
}


void mouse(int button, int state, int x, int y) {
	//Adiciona pontos e curvas
	//Jogador cria pontos clicando no botao esquerdo do mouse
	if (point_idx < NPOINTS - 2) {
		//Criacao de pontos e curvas
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			//Adiciona um ponto a lista
			point_idx++;

			//Pega coordenadas do ponto passado
			points_x[point_idx] = (2.0*x) / WINDOW_WIDTH - 1.0;
			points_y[point_idx] = 1.0 - (2.0*y) / WINDOW_HEIGHT;

			//Existem mais de 4 pontos em tela
			//Adiciona nova curva
			if (point_idx > 3) {
				bezierQuad((points_x[point_idx - 3] + points_x[point_idx - 2]) / 2, 
					points_x[point_idx - 2], (points_x[point_idx - 2] + points_x[point_idx - 1]) / 2,
					(points_y[point_idx - 3] + points_y[point_idx - 2]) / 2, points_y[point_idx - 2],
					(points_y[point_idx - 2] + points_y[point_idx - 1]) / 2);

				bezier_idx++;
				bezierQuad((points_x[point_idx - 2] + points_x[point_idx - 1]) / 2,
					points_x[point_idx - 1], points_x[point_idx],
					(points_y[point_idx - 2] + points_y[point_idx - 1]) / 2,
					points_y[point_idx - 1], points_y[point_idx]);
			}
			//Existem 4 pontos em tela
			//Adiciona segunda curva
			else if (point_idx == 3) {
				bezierQuad(points_x[point_idx - 3], points_x[point_idx - 2],
					(points_x[point_idx - 2] + points_x[point_idx - 1]) / 2,
					points_y[point_idx - 3], points_y[point_idx - 2],
					(points_y[point_idx - 2] + points_y[point_idx - 1]) / 2);

				bezier_idx++;
				bezierQuad((points_x[point_idx - 2] + points_x[point_idx - 1]) / 2,
					points_x[point_idx - 1], points_x[point_idx], 
					(points_y[point_idx - 2] + points_y[point_idx - 1]) / 2,
					points_y[point_idx - 1], points_y[point_idx]);
			}
			//Existem 3 pontos em tela
			//Cria primeira curva
			else if (point_idx == 2) {
				bezier_idx++;
				bezierQuad(points_x[point_idx - 2], points_x[point_idx - 1], points_x[point_idx],
					points_y[point_idx - 2], points_y[point_idx - 1], points_y[point_idx]);
			}
		}
	}

	//Remocao de pontos e curvas
	//Jogador remove pontos clicanco no botao direito do mouse
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN) {
		//Existem mais de duas curvas
		//Remove ultima curva e ultimo ponto
		if (point_idx > 3) {
			removeBezier();
			bezier_idx--;

			points_x[point_idx] = 0.0;
			points_y[point_idx] = 0.0;
			point_idx--;

			bezierQuad((points_x[point_idx - 2] + points_x[point_idx - 1]) / 2, points_x[point_idx - 1], 
				points_x[point_idx], (points_y[point_idx - 2] + points_y[point_idx - 1]) / 2,
				points_y[point_idx - 1], points_y[point_idx]);
		}
		//Existem duas curvas
		//Remove ultima curva e ultimo ponto
		else if (point_idx == 3) {
			removeBezier();
			bezier_idx--;

			points_x[point_idx] = 0.0;
			points_y[point_idx] = 0.0;
			point_idx--;

			bezierQuad(points_x[point_idx - 2], points_x[point_idx - 1], points_x[point_idx],
				points_y[point_idx - 2], points_y[point_idx - 1], points_y[point_idx]);
		}
		//Existe apenas uma curva
		//Remove curva e ultimo ponto
		else if (point_idx == 2) { 
			removeBezier();
			bezier_idx--;

			points_x[point_idx] = 0.0;
			points_y[point_idx] = 0.0;
			point_idx--;
		}
		//Nao existem curvas
		//Remove o ultimo ponto
		else if (point_idx > -1) { 
			points_x[point_idx] = 0.0;
			points_y[point_idx] = 0.0;
			point_idx--;
		}
	}

	glutPostRedisplay();
}

void display(void) {
	//Indica a cor da tela
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	mapa();

	//Define cor atual
	glColor3f(1.0, 1.0, 1.0);

	//Desenha pontos
	glPointSize(4.0);
	glBegin(GL_POINTS);
	for (int i = 0; i <= point_idx; i++) {
		glVertex2d(points_x[i], points_y[i]);
	}
	glEnd();

	//Desenha retas (Curva de Bezier)
	glBegin(GL_LINE_STRIP);
	for (int i = 0; i <= bezier_idx; i++) 
		for(int j = 0; j <= DIVBEZIER; j++)
			glVertex2d(curva_x[i][j], curva_y[i][j]);
	glEnd();

	//Exibe o conteudo do Frame Buffer
	glFlush();
	//Troca de buffer (duble buffer)
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	init_game(FAZE);

	//Modo do Display
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	//Define tamanho de tela
	glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
	//Inicia tela centralizada
	glutInitWindowPosition((glutGet(GLUT_SCREEN_WIDTH) - WINDOW_WIDTH) / 2,
		(glutGet(GLUT_SCREEN_HEIGHT) - WINDOW_HEIGHT) / 2);
	//Cria uma janela
	glutCreateWindow("Isso eh uma janela");

	//Inicia a funcao display()
	glutDisplayFunc(display);
	//Recebe comandos do mouse
	glutMouseFunc(mouse);
	//Inicia gerenciador de eventos
	glutMainLoop();

	return 0;
}
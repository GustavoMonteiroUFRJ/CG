#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NPOINTS 20 //Tamanho do vetor de pontos; se NPOINTS = 5, usuario pode inserir 4 pontos
#define DIVBEZIER 20 //Numero de pontos na curva de bezier

#define NUMOBSTACULOS 15 //Numero maximo de obstaculos no mapa
#define MINOBSTACULOS 10 //Numero minimo de obstaculos no mapa
#define TAMOBSTACULO 15	//Tamanho do obstaculo

int numeroObstaculos;
float obstaculoX[200];
float obstaculoY[200];
const float t = 1 / (float) DIVBEZIER;
GLfloat points_x[NPOINTS]; //Vetor de coordenadas x dos pontos
GLfloat points_y[NPOINTS]; //Vetor de coordenadas  dos pontos
GLfloat curva_x[NPOINTS - 2][DIVBEZIER]; //Vetor de coordenadas x da curva
GLfloat curva_y[NPOINTS - 2][DIVBEZIER]; //Vetor de coordenadas y da curva
GLint point_idx = -1; //Contador de pontos
GLint bezier_idx = -1; //Contador de curvas

//Desenha os pontos iniciais e finais (a principio, fiz eles fixos)
void pontos(void)
{
	glColor3f(0.0f, 0.0f, 1.0f);
	glBegin(GL_QUADS);	//quadrado de cima esquerda
	glVertex2d(-1, 0.8);
	glVertex2d(-0.8, 0.8);
	glVertex2d(-0.8, 1);
	glVertex2d(-1, 1);
	glEnd();
	glBegin(GL_QUADS);	//quadrado de baixo direita
	glVertex2d(1, -0.8);
	glVertex2d(0.8, -0.8);
	glVertex2d(0.8, -1);
	glVertex2d(1, -1);
	glEnd();
}

//Desenha os obstaculos no mapa e a area final
void mapa(void)
{
	int i = 2, j = 0, end = 0;
	int aux1, aux2;
	float cxj, cyj;

	pontos(); //Desenha ponto inicial e final


	if (obstaculoX[0] == 2) {
		float coord_inicialx[(NUMOBSTACULOS + 5)];
		float coord_inicialy[(NUMOBSTACULOS + 5)];
		coord_inicialx[0] = -1;
		coord_inicialy[0] = 1;
		coord_inicialx[1] = 1;
		coord_inicialy[1] = -1;
		//Escolhe um numero aleatorio de obstaculos
		aux1 = rand();
		numeroObstaculos = ((aux1 % NUMOBSTACULOS) + 2 + MINOBSTACULOS);

		//Descobre pontos para o obstaculo, sempre deixando um espa�o entre eles e n�o fechando os pontos iniciais e finais
		while (i < numeroObstaculos)
		{
			aux1 = rand();
			aux2 = rand();
			coord_inicialx[i] = ((aux1 % (200 - TAMOBSTACULO)) / 100.0) - 1;
			coord_inicialy[i] = ((aux2 % (200 - TAMOBSTACULO)) / 100.0) - 1;
			while (j < i) //Confere se h� espa�o entre os obstaculos
			{
				if (((coord_inicialx[i] >(coord_inicialx[j] + (TAMOBSTACULO / 100.0) + 0.15)) || (coord_inicialx[i] < (coord_inicialx[j] - 0.15))) && ((coord_inicialy[i] > (coord_inicialy[j] + (TAMOBSTACULO / 30.0) + 0.15)) || (coord_inicialy[i] < (coord_inicialy[j] - 0.15))))
				{
					obstaculoX[i] = coord_inicialx[i];
					obstaculoY[i] = coord_inicialy[i];
				}
				else
				{
					obstaculoX[i] = coord_inicialx[i] + TAMOBSTACULO / 100.0 + 0.1;
					break;
				}
				j++;
			}
			i++; //Passo pro proximo ponto
			j = 0;
		}
		obstaculoX[0] = 1;
	}

	i = 2;
	glColor3f(0.0, 1.0, 0.0);
	while (i < numeroObstaculos)
	{
		cxj = obstaculoX[i] + TAMOBSTACULO / 100.0;
		cyj = obstaculoY[i] + TAMOBSTACULO / 100.0;
		//Desenha quadrados
		glBegin(GL_QUADS);
		glVertex2d(obstaculoX[i], obstaculoY[i]);
		glVertex2d(obstaculoX[i], cyj);
		glVertex2d(cxj, cyj);
		glVertex2d(cxj, obstaculoY[i]);
		glEnd();
		i++;
	}
}

//Remove curvas de Bezier
void removeBezier() {
	for (int i = 0; i <= DIVBEZIER; i++) {
		curva_x[bezier_idx][i] = NULL;
		curva_y[bezier_idx][i] = NULL;
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

			points_x[point_idx] = NULL;
			points_y[point_idx] = NULL;
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

			points_x[point_idx] = NULL;
			points_y[point_idx] = NULL;
			point_idx--;

			bezierQuad(points_x[point_idx - 2], points_x[point_idx - 1], points_x[point_idx],
				points_y[point_idx - 2], points_y[point_idx - 1], points_y[point_idx]);
		}
		//Existe apenas uma curva
		//Remove curva e ultimo ponto
		else if (point_idx == 2) { 
			removeBezier();
			bezier_idx--;

			points_x[point_idx] = NULL;
			points_y[point_idx] = NULL;
			point_idx--;
		}
		//Nao existem curvas
		//Remove o ultimo ponto
		else if (point_idx > -1) { 
			points_x[point_idx] = NULL;
			points_y[point_idx] = NULL;
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
	for (int i = 0; i <= point_idx; i++) 
		glVertex2d(points_x[i], points_y[i]);
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
	srand(time(0));
	obstaculoX[0] = 2;

	glutInit(&argc, argv);

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
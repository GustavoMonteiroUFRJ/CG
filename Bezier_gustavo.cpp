#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NPOINTS 20 //numero de pontos adicionados pelo usuario
#define DIVBEZIER 20  //numero de pontos na curva de bezier

#define NUMOBSTACULOS 15	//numero maximo de obstaculos no mapa
#define MINOBSTACULOS 10	//numero minimo de obstaculos no mapa
#define TAMOBSTACULO 15	//tamanho do obstaculo

const float t = 1 / (float)DIVBEZIER;
GLfloat points_x[NPOINTS], points_y[NPOINTS];
GLfloat curva_x[200], curva_y[200];
GLint point_idx = -1;
GLint bezier_idx = -1;

//JO�O

float obstaculoX[200];
float obstaculoY[200];
int numeroObstaculos;


void imprime_bloco(float x, float y, float altura, float largura )
{
		glBegin(GL_QUADS);
		glVertex2d(x, y);
		glVertex2d(x + largura, y);
		glVertex2d(x + largura, y + altura);
		glVertex2d(x, y + altura);
		glEnd();
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


	if (obstaculoX[0] == 2) {
		float coord_inicialx[(NUMOBSTACULOS + 5)];
		float coord_inicialy[(NUMOBSTACULOS + 5)];

		coord_inicialx[0] = -1;
		coord_inicialy[0] = 1;
		coord_inicialx[1] = 1;
		coord_inicialy[1] = -1;
		//escolhe um numero aleatorio de obstaculos
		aux1 = rand();
		numeroObstaculos = ((aux1 % NUMOBSTACULOS) + 2 + MINOBSTACULOS);

		//descobre pontos para o obstaculo, sempre deixando um espa�o entre eles e n�o fechando os pontos iniciais e finais
		while (i < numeroObstaculos)
		{
			aux1 = rand();
			aux2 = rand();
			coord_inicialx[i] = ((aux1 % (200 - TAMOBSTACULO)) / 100.0) - 1;
			coord_inicialy[i] = ((aux2 % (200 - TAMOBSTACULO)) / 100.0) - 1;
			while (j < i)	//confere se h� espa�o entre os obstaculos
			{
				if (((coord_inicialx[i] >(coord_inicialx[j] + (TAMOBSTACULO / 100.0) + 0.15)) || (coord_inicialx[i] < (coord_inicialx[j] - 0.15))) && ((coord_inicialy[i] > (coord_inicialy[j] + (TAMOBSTACULO / 30.0) + 0.15)) || (coord_inicialy[i] < (coord_inicialy[j] - 0.15))))
				{
					obstaculoX[i] = coord_inicialx[i];
					obstaculoY[i] = coord_inicialy[i];
				}
				else
				{
					obstaculoX[i] = coord_inicialx[i] + TAMOBSTACULO/100.0 + 0.1;
					break;
				}
				j++;
			}
			i++;	//passo pro proximo ponto
			j = 0;
		}
		obstaculoX[0] = 1;
	}

	glColor3f(1.0, 0.0, 0.0);
	for (i=2 ; i < numeroObstaculos; i++)
	{
		imprime_bloco(obstaculoX[i], obstaculoY[i], TAMOBSTACULO / 100.0, TAMOBSTACULO / 100.0);
	}
}

//YASMIM

void bezierCub(int p1, int p2, int p3, int p4) {
	for (int i = 0; i <= DIVBEZIER; i++) {
		bezier_idx++;

		curva_x[bezier_idx] = (pow(1 - t * i, 3) * points_x[p1]) + (3 * t*i * pow(1 - t * i, 2) * points_x[p2]) +
			(3 * pow(t*i, 2) * (1 - t * i) * points_x[p3]) + (pow(t*i, 3) * points_x[p4]);
		curva_y[bezier_idx] = (pow(1 - t * i, 3) * points_y[p1]) + (3 * t*i * pow(1 - t * i, 2) * points_y[p2]) +
			(3 * pow(t*i, 2) * (1 - t * i) * points_y[p3]) + (pow(t*i, 3) * points_y[p4]);
	}
}

void bezierQuad(int p1, int p2, int p3) {
	for (int i = 0; i <= DIVBEZIER; i++) {
		bezier_idx++;

		curva_x[bezier_idx] = (pow(1 - t * i, 2) * points_x[p1]) +
			(2 * t*i * (1 - t * i) * points_x[p2]) + (pow(t*i, 2) * points_x[p3]);
		curva_y[bezier_idx] = (pow(1 - t * i, 2) * points_y[p1]) +
			(2 * t*i * (1 - t * i) * points_y[p2]) + (pow(t*i, 2) * points_y[p3]);
	}
}

void mouse(int button, int state, int x, int y) {
	if (point_idx < NPOINTS - 1) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			point_idx++;

			points_x[point_idx] = (2.0*x) / WINDOW_WIDTH - 1.0;
			points_y[point_idx] = 1.0 - (2.0*y) / WINDOW_HEIGHT;

			//Aplica Bezier Quadratico
			//if (point_idx % 2 == 0 && point_idx != 0) 
			//	bezierQuad(point_idx - 2, point_idx - 1, point_idx);

			//Aplica Bezier Cubico
			if (point_idx % 3 == 0 && point_idx != 0)
				bezierCub(point_idx - 3, point_idx - 2, point_idx - 1, point_idx);
		}
	}

	glutPostRedisplay();
}

void display(void) {
	//Indica a cor da tela
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	mapa();	//adi��o: JO�O

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
	for (int i = 0; i <= bezier_idx; i++) {
		glVertex2d(curva_x[i], curva_y[i]);
	}
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
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define NPOINTS 100 //tamanho do vetor de pontos; se NPOINTS = 5, usuario pode inserir 4 pontos
#define DIVBEZIER 20 //numero de pontos na curva de bezier

const float t = 1 / (float) DIVBEZIER;
GLfloat points_x[NPOINTS], points_y[NPOINTS];
GLfloat curva_x[NPOINTS - 2][DIVBEZIER], curva_y[NPOINTS - 2][DIVBEZIER];
GLint point_idx = -1;
GLint bezier_idx = -1;

void removeBezier() {
	for (int i = 0; i <= DIVBEZIER; i++) {
		curva_x[bezier_idx][i] = 0.0;
		curva_y[bezier_idx][i] = 0.0;
	}
}

void bezierQuad(GLfloat p1_x, GLfloat p2_x, GLfloat p3_x, GLfloat p1_y, GLfloat p2_y, GLfloat p3_y) {
	for (int i = 0; i <= DIVBEZIER; i++) {
		curva_x[bezier_idx][i] = (pow(1 - t*i, 2) * p1_x) +
			(2 * t*i * (1 - t*i) * p2_x) + (pow(t*i, 2) * p3_x);
		curva_y[bezier_idx][i] = (pow(1 - t * i, 2) * p1_y) +
			(2 * t*i * (1 - t*i) * p2_y) + (pow(t*i, 2) * p3_y);
	}
}

void mouse(int button, int state, int x, int y) {
	if (point_idx < NPOINTS - 2) {
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
			//Adiciona um ponto a lista
			point_idx++;

			//Pega coordenadas do ponto passado
			points_x[point_idx] = (2.0*x) / WINDOW_WIDTH - 1.0;
			points_y[point_idx] = 1.0 - (2.0*y) / WINDOW_HEIGHT;

			//Aplica Bezier Quadratico
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
			if (point_idx == 3) {
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
			else if (point_idx == 2) {
				bezier_idx++;
				bezierQuad(points_x[point_idx - 2], points_x[point_idx - 1], points_x[point_idx],
					points_y[point_idx - 2], points_y[point_idx - 1], points_y[point_idx]);
			}
		}
	}

	glutPostRedisplay();
}

void display(void) {
	//Indica a cor da tela
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

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
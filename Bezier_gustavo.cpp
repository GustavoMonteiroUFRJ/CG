#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define NPOINTS 20 //numero de pontos adicionados pelo usuario
#define DIVBEZIER 20  //numero de pontos na curva de bezier

#define ALTURA_BLOCO 0.25
#define LARGURA_BLOCO 0.25
int quantidade_de_blocos;

float obistaculos[200][4];
#define X 0
#define Y 1
#define ALTURA 2
#define LARGURA 3

int faze;


#define LIMIT 1000
int quantidade_de_pontos = 0;
const float t = 1 / (float) DIVBEZIER;
GLfloat pontos_de_controle[30][2];
GLfloat curva[LIMIT+1][2];



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

float combinacao(int n, int x)
{
    if (x > n){
        printf("Erro, conbinacao trocada");
        exit(-1);
    }
	float resposta = 1.0;
	if (x >  n/2)
	{
		x = n-x;
	}
	for(size_t i = 0; i < x; i++)
	{
		resposta *= 1.0*(n-i)/(x-i);
	}
	return resposta;
}

void bezier(int quantidade_de_pontos)
{
	int n = quantidade_de_pontos-1;
	float passo = 1.0 / (LIMIT-1);
	float t = 0.0;
	for(size_t i = 0; i < LIMIT; i++)
	{
		
		curva[i][X] = 0;
		curva[i][Y] = 0;
		for(size_t j = 0; j < quantidade_de_pontos; j++)
		{	
			curva[i][X] += combinacao(n,j)*pow(1-t,n-j)*pow(t,j)*pontos_de_controle[j][0];
			curva[i][Y] += combinacao(n,j)*pow(1-t,n-j)*pow(t,j)*pontos_de_controle[j][1];
		}
		t += passo;
	}
}

void init_game(int num_faze)
{
	faze = num_faze;
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

void mouse(int button, int state, int x, int y) {
	//Adiciona pontos e curvas
	//Jogador cria pontos clicando no botao esquerdo do mouse
	if (quantidade_de_pontos < NPOINTS - 1) 
	{
		//Criacao de pontos e curvas
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
		{	
			if(quantidade_de_pontos < 2)
			{
				// Adiciona o preimeiro e o segundo ponto
				pontos_de_controle[quantidade_de_pontos][X] = (2.0*x) / WINDOW_WIDTH - 1.0;
				pontos_de_controle[quantidade_de_pontos][Y] = 1.0 - (2.0*y) / WINDOW_HEIGHT;
			}
			else
			{
				// manter o segundo ponto smepre no ultimo lugar!
				pontos_de_controle[quantidade_de_pontos][X] = pontos_de_controle[quantidade_de_pontos-1][X];
				pontos_de_controle[quantidade_de_pontos][Y] = pontos_de_controle[quantidade_de_pontos-1][Y];
				
				//adiciona o novo ponto
				pontos_de_controle[quantidade_de_pontos-1][X] = (2.0*x) / WINDOW_WIDTH - 1.0;
				pontos_de_controle[quantidade_de_pontos-1][Y] = 1.0 - (2.0*y) / WINDOW_HEIGHT;
				
			}
			// incrementa o contador
			quantidade_de_pontos++;

			bezier(quantidade_de_pontos);
		}
	}
	//Remocao de pontos e curvas
	//Jogador remove pontos clicanco no botao direito do mouse
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN ) {
		
		if(quantidade_de_pontos > 0)
		{
			quantidade_de_pontos--;
			if(quantidade_de_pontos >= 2)
			{
				pontos_de_controle[quantidade_de_pontos-1][X] = pontos_de_controle[quantidade_de_pontos][X];
				pontos_de_controle[quantidade_de_pontos-1][Y] = pontos_de_controle[quantidade_de_pontos][Y];
			}		
			bezier(quantidade_de_pontos);
		}
		else
		{
			init_game(faze);
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
	for (int i = 0; i < quantidade_de_pontos; i++) {
		glVertex2d(pontos_de_controle[i][X], pontos_de_controle[i][Y]);
	}
	glEnd();

	if(quantidade_de_pontos >= 2){
		//Desenha retas (Curva de Bezier)
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < LIMIT; i++)
			glVertex2d(curva[i][X], curva[i][Y]);
		glEnd();
	}
	//Exibe o conteudo do Frame Buffer
	glFlush();
	//Troca de buffer (duble buffer)
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	init_game(0);

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
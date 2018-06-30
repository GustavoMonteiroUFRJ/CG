#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define MAX_PONTOS 40 //numero de pontos adicionados pelo usuario

#define ALTURA_BLOCO 0.25
#define LARGURA_BLOCO 0.25

float obistaculos[200][4];
int quantidade_de_obistaculos = 0;
// grupos de defines para trabalhar com os obistaculos
#define X 0
#define Y 1
#define ALTURA 2
#define LARGURA 3


int fase = 0; // armazena a fase do jogo

#define LIMIT 300	
int quantidade_de_pontos = 0;
GLfloat pontos_de_controle[MAX_PONTOS+2][2];
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

int bloco_invalido(float x, float y, float altura, float largura)
{
	if(x + largura > 1) return 1;
	if(y + altura > 1) return 1;
	if(x < -0.8 && y + altura > 0.8) return 1;
	if(x + largura > 0.8 && y < -0.8) return 1;
	return 0; // essa funcao existe para checar se o bloco gerado aleatoriamente esta em um lugar valido ou nao.
}

void gera_blocos_aleatorios(){
	// iniciando o gerador de numeros aleatórios
	srand(time(0));
	
	quantidade_de_obistaculos = 20;
	float x,y;
	for(int i = 0; i < quantidade_de_obistaculos; i++)
	{	
		do
		{	
			x = 1.0*rand()/RAND_MAX; // nuero entre 0 e 1 
			y = 1.0*rand()/RAND_MAX; // nuero entre 0 e 1 
			x = 2*x -1; // nuero entre -1 e 1 
			y = 2*y -1;	// nuero entre -1 e 1

		} while (bloco_invalido(x,y,ALTURA_BLOCO,LARGURA_BLOCO));

		obistaculos[i][X] = x;
		obistaculos[i][Y] = y;
		obistaculos[i][ALTURA] = ALTURA_BLOCO;
		obistaculos[i][LARGURA] = LARGURA_BLOCO;
	}
}

void gera_blocos_fase_1(){	

 	quantidade_de_obistaculos = 2;

	obistaculos[0][X] = -0.7;
	obistaculos[0][Y] = -0.4;
	obistaculos[0][ALTURA] = 0.7;
	obistaculos[0][LARGURA] = 0.6;
	
	obistaculos[1][X] = 0.2;
	obistaculos[1][Y] = -0.2;
	obistaculos[1][ALTURA] = 0.7;
	obistaculos[1][LARGURA] = 0.6;
}

void gera_blocos_fase_2(){	

 	quantidade_de_obistaculos = 1;

	obistaculos[0][X] = -0.7;
	obistaculos[0][Y] = -0.4;
	obistaculos[0][ALTURA] = 1.2;
	obistaculos[0][LARGURA] = 1.4;
}

//Desenha os obstaculos no mapa e a area final
void mapa(void)
{
	// ponto inicial
	glColor3f(0.0f, 0.0f, 1.0f);
	imprime_bloco(0.8, -1.0, 0.2, 0.2);

	// ponto final
	glColor3f(0.0f, 1.0f, 0.0f);
	imprime_bloco(-1.0,0.8,0.2,0.2);

	glColor3f(1.0, 0.0, 0.0);
	for (int i=0 ; i < quantidade_de_obistaculos; i++)
	{
		imprime_bloco(obistaculos[i][X], obistaculos[i][Y], obistaculos[i][ALTURA], obistaculos[i][LARGURA]);
	}
}

float combinacao(int n, int x)
{
    if (x > n){
        printf("Erro, conbinacao trocada\n");
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

void init_game(int num_fase)
{
	printf("New game! Faze %d\n",fase);
	fase = num_fase;	
	switch (fase)
	{
		case 0:
			gera_blocos_aleatorios();
			break;
	
		case 1:
			gera_blocos_fase_1();
			break;
		case 2:
			gera_blocos_fase_2();
			break;

		default:
			printf("fase escolida nao reconhecida\n");
	}

}

int no_intervalo(float p, float t1, float t2){
    if (t1 > t2){
        float aux = t1;
        t1 = t2;
        t2 = aux;
    }
    if( p > t1 && p < t2)
	{
        return 1 ;
	}
    else
	{
        return 0;
	}

}

int colisao_x(float *p1, float *p2, float y, float x1, float x2)
{
    int ret = 0;
    if( no_intervalo(y, p1[Y], p2[Y]) )
    {
        float t = (y - p2[Y]) / (p1[Y] - p2[Y]);
        float x_intercecao = p1[X]*t + (1-t)*p2[X];

        if(no_intervalo(x_intercecao, x1, x2))
		{
            ret = 1;
        }
    }
    return ret;
}

int colisao_y(float *p1, float *p2, float x, float y1, float y2)
{
    int ret = 0;
    if( no_intervalo(x, p1[X], p2[X]) )
    {
        float t = (x - p2[X]) / (p1[X] - p2[X]);
        float y_intercecao = p1[Y]*t + (1-t)*p2[Y];

        if(no_intervalo(y_intercecao, y1, y2)){
            ret = 1;
        }
    }
    return ret;
}

int colisao_entre_segmentos(float *p1, float *p2, float* q1, float* q2){

	int ret = -1; 
    if(q1[X] == q2[X])
    {
        ret =  colisao_y(p1, p2, q1[X], q1[Y], q2[Y]) ;
    }
    else if(q1[Y] == q2[Y])
    {
        ret = colisao_x(p1, p2, q1[Y], q1[X], q2[X]);
    }
    else
    {
        printf("Bad colistion! Um dos segmentos não é perpendicular ao eixo\n");
        printf("Feture ainda não implemetada\n");
    }
}

int colisao()
{
	float p1[2], p2[2];
	for(int i = 1; i < LIMIT; i++)
	{
		for(int j = 0; j < quantidade_de_obistaculos; j++)
		{
			p1[X] = obistaculos[j][X];
			p1[Y] = obistaculos[j][Y];
			p2[X] = obistaculos[j][X] + obistaculos[j][LARGURA];
			p2[Y] = obistaculos[j][Y];
			if( colisao_entre_segmentos(curva[i-1], curva[i], p1, p2) ) return 1;
			p1[X] = p2[X];
			p1[Y] = p2[Y];
			p2[X] = obistaculos[j][X] + obistaculos[j][LARGURA];
			p2[Y] = obistaculos[j][Y] + obistaculos[j][ALTURA];
			if( colisao_entre_segmentos(curva[i-1], curva[i], p1, p2) ) return 1;
			p1[X] = p2[X];
			p1[Y] = p2[Y];
			p2[X] = obistaculos[j][X];
			p2[Y] = obistaculos[j][Y] + obistaculos[j][ALTURA];
			if( colisao_entre_segmentos(curva[i-1], curva[i], p1, p2) ) return 1;
			p1[X] = p2[X];
			p1[Y] = p2[Y];
			p2[X] = obistaculos[j][X];
			p2[Y] = obistaculos[j][Y];
			if( colisao_entre_segmentos(curva[i-1], curva[i], p1, p2) ) return 1;
		}
	}
	return 0;
}

void mouse(int button, int state, int x, int y) {
	//Adiciona pontos e curvas
	//Jogador cria pontos clicando no botao esquerdo do mouse
	if (quantidade_de_pontos < MAX_PONTOS - 1) 
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
			glutPostRedisplay();
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
			init_game(fase); // efeito pratico apenas em blocos aleatórios
		}
		glutPostRedisplay();
	}

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
		if(colisao()){
			printf("Colidiu\n");
		}else{
			printf("Passou!!!\n");
		}
	}

	//Exibe o conteudo do Frame Buffer
	glFlush();
	//Troca de buffer (duble buffer)
	glutSwapBuffers();
}

int main(int argc, char** argv) {

	glutInit(&argc, argv);

	if(argc > 1){
		fase = atoi(argv[1]);
	}

	init_game(fase);

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
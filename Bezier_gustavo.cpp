#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <GL/glut.h>

#define WINDOW_WIDTH 600
#define WINDOW_HEIGHT 600
#define MAX_PONTOS 40 // Numero de pontos adicionados pelo usuario

#define ALTURA_BLOCO 0.25
#define LARGURA_BLOCO 0.25

// Informacoes do bloco inicial
#define x_inicial 0.8
#define y_inicial -1.0
#define altura_inicial 0.2
#define largura_inicial 0.2

// Informacoes do bloco final
#define x_final -1.0
#define y_final 0.8
#define altura_final 0.2
#define largura_final 0.2


float obistaculos[200][4];
int quantidade_de_obistaculos = 20;
// Grupos de defines para trabalhar com os obistaculos
#define X 0
#define Y 1
#define ALTURA 2
#define LARGURA 3


int fase = 0; // Armazena a fase do jogo

#define LIMIT 5000	
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

// Verifica se os blocos adicionados alatoriamente estao em posicoes invalidas.
int bloco_invalido(float x, float y, float altura, float largura)
{
	if(x + largura > 1) return 1; // Fora do cmapo 
	if(y + altura > 1) return 1; // Fora do cmapo 
	if(x < x_final + largura_final  && y + altura > y_final) return 1; // Em cima do bloco final
	if(x + largura > x_inicial && y < y_inicial + altura_inicial) return 1; // Em cima do bloco inicial
	return 0; // Bloco valido!
}

// Preenche o vetor "obstaculso[]" com blocos aleatorios.
void gera_blocos_aleatorios()
{	
	// Iniciando o gerador de numeros aleatórios
	srand(time(0));
	
	// quantidade_de_obistaculos = 20; // Apenas nesse caso, sera inicializada pela main.
	float x,y;
	for(int i = 0; i < quantidade_de_obistaculos; i++)
	{	
		do
		{	
			x = 1.0*rand()/RAND_MAX; // Nuero entre 0 e 1 
			y = 1.0*rand()/RAND_MAX; // Nuero entre 0 e 1 
			x = 2*x -1; // Nuero entre -1 e 1 
			y = 2*y -1;	// Nuero entre -1 e 1

		} while (bloco_invalido(x,y,ALTURA_BLOCO,LARGURA_BLOCO));

		obistaculos[i][X] = x;
		obistaculos[i][Y] = y;
		obistaculos[i][ALTURA] = ALTURA_BLOCO;
		obistaculos[i][LARGURA] = LARGURA_BLOCO;
	}
}

// Preenche o vetor "obstaculso[]" com blocos pre definidos.
void gera_blocos_fase_1()
{	
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

// Preenche o vetor "obstaculso[]" com blocos pre definidos.
void gera_blocos_fase_2()
{	
 	quantidade_de_obistaculos = 1;

	obistaculos[0][X] = -0.7;
	obistaculos[0][Y] = -0.4;
	obistaculos[0][ALTURA] = 1.2;
	obistaculos[0][LARGURA] = 1.4;
}

// Desenha os obstaculos no mapa e as area final e inicial 
void mapa(void)
{
	// Aria inicial
	glColor3f(0.0f, 0.0f, 1.0f);
	imprime_bloco(x_inicial, y_inicial, altura_inicial, altura_inicial);

	// Aria final
	glColor3f(0.0f, 1.0f, 0.0f);
	imprime_bloco(x_final, y_final, altura_final, altura_final);

	// Blocos de obstaculos
	glColor3f(1.0, 0.0, 0.0);
	for (int i=0 ; i < quantidade_de_obistaculos; i++)
	{
		imprime_bloco(obistaculos[i][X], obistaculos[i][Y], obistaculos[i][ALTURA], obistaculos[i][LARGURA]);
	}
}

// Ferramenta matematica
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
		/*
		Obs: possui erro numerico
		porem apenas para valores muito grandes, e o erro eh bem pequeno
		para as grandezas da aplicao atual funciona muito bem
		*/ 
		resposta *= 1.0*(n-i)/(x-i);
	}
	return resposta;
}

// Curva de Bezier para N pontos de controle.
void bezier(int quantidade_de_pontos)
{
	int n = quantidade_de_pontos-1; // Questao de legibilidade
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

// Inicia o jogo ecolhendo a fase
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
			printf("Fase escolida nao reconhecida!\n");
	}
}

// Ferramente matematica, cehga se p esta entre [t1 , t2]
int no_intervalo(float p, float t1, float t2)
{
    if (t1 > t2){ // Manter t2 > t1
        float aux = t1;
        t1 = t2;
        t2 = aux;
    }
    if( p > t1 && p < t2)
        return 1;
    else
        return 0;
}

// Calcula colisao entre um segmento p1p2 e um segmento (x1,y)(x2,y)
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

// Calcula colisao entre um segmento p1p2 e um segmento (x,y1)(x,y2)
int colisao_y(float *p1, float *p2, float x, float y1, float y2)
{
    int ret = 0;
    if( no_intervalo(x, p1[X], p2[X]) )
    {
        float t = (x - p2[X]) / (p1[X] - p2[X]);
        float y_intercecao = p1[Y]*t + (1-t)*p2[Y];

        if(no_intervalo(y_intercecao, y1, y2))
		{
            ret = 1;
        }
    }
    return ret;
}

// Calcula colisao entre um segmento qualquer e um segmento perpendicular a um eixo
int colisao_entre_segmentos(float *p1, float *p2, float* q1, float* q2){

    if(q1[X] == q2[X])
    {
        return colisao_y(p1, p2, q1[X], q1[Y], q2[Y]);
    }
    else if(q1[Y] == q2[Y])
    {
        return colisao_x(p1, p2, q1[Y], q1[X], q2[X]);
    }
    else
    {
        printf("Bad colistion! Um dos segmentos não é perpendicular ao eixo\n");
        printf("Feture ainda não implemetada\n");
		exit(-1);
    }
	return -1;
}

// Calcula colisao entre a curva de bezire e os obistaculos
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

void incere_ponto(float x, float y)
{
	if(quantidade_de_pontos == 0)
	{
		if(no_intervalo(x, x_inicial, x_inicial + largura_inicial) && 
			no_intervalo(y, y_inicial, y_inicial + altura_inicial)){
			// Dentro do campo inicial
			// Adiciona o preimeiro ponto
			pontos_de_controle[quantidade_de_pontos][X] = x;
			pontos_de_controle[quantidade_de_pontos][Y] = y;
		}
		else return;
	}
	else if (quantidade_de_pontos == 1)
	{
		if(no_intervalo(x ,x_final ,x_final + largura_final) && 
			no_intervalo(y,y_final,y_final + altura_final)){
			// Dentro do campo final
			// Adiciona o segundo ponto
			pontos_de_controle[quantidade_de_pontos][X] = x;
			pontos_de_controle[quantidade_de_pontos][Y] = y;
		}
		else return;
	}
	else
	{
		// Manter o segundo ponto smepre no ultimo lugar!
		pontos_de_controle[quantidade_de_pontos][X] = pontos_de_controle[quantidade_de_pontos-1][X];
		pontos_de_controle[quantidade_de_pontos][Y] = pontos_de_controle[quantidade_de_pontos-1][Y];
		
		// Adiciona o novo ponto
		pontos_de_controle[quantidade_de_pontos-1][X] = x;
		pontos_de_controle[quantidade_de_pontos-1][Y] = y;
		
	}
	// Incrementa o contador
	quantidade_de_pontos++;
}

float distancia_entre_pontos(float* p1, float* p2)
{
	float x1 = p1[X];
	float y1 = p1[Y];
	float x2 = p2[X];
	float y2 = p2[Y];
	return sqrt(pow(x1-x2,2)+pow(y1-y2,2));
}

void incere_ponto2(float x, float y)
{
	if(quantidade_de_pontos == 0)
	{
		if(no_intervalo(x, x_inicial, x_inicial + largura_inicial) && 
			no_intervalo(y, y_inicial, y_inicial + altura_inicial)){
			// Dentro do campo inicial
			// Adiciona o preimeiro ponto
			pontos_de_controle[quantidade_de_pontos][X] = x;
			pontos_de_controle[quantidade_de_pontos][Y] = y;
		}
		else return;
	}
	else if (quantidade_de_pontos == 1)
	{
		if(no_intervalo(x ,x_final ,x_final + largura_final) && 
			no_intervalo(y,y_final,y_final + altura_final)){
			// Dentro do campo final
			// Adiciona o segundo ponto
			pontos_de_controle[quantidade_de_pontos][X] = x;
			pontos_de_controle[quantidade_de_pontos][Y] = y;
		}
		else return;
	}
	else
	{
		// Manter o segundo ponto smepre no ultimo lugar!
		pontos_de_controle[quantidade_de_pontos][X] = pontos_de_controle[quantidade_de_pontos-1][X];
		pontos_de_controle[quantidade_de_pontos][Y] = pontos_de_controle[quantidade_de_pontos-1][Y];
		
		float p_limit[2];
		p_limit[X]=-1.0;
		p_limit[Y]=1.0;

		float p_atual[2];
		p_atual[X] = x;
		p_atual[Y] = y;

		int index_minimo = 1;
		int index_maximo = quantidade_de_pontos-2;
		float distancia_maxima = distancia_entre_pontos(p_limit, pontos_de_controle[0]);
		float distancia_atual = distancia_entre_pontos(p_atual, pontos_de_controle[0]);

		// primeiro chute do index
		int index = index_minimo + index_maximo * distancia_atual / distancia_maxima;
		if(distancia_atual > distancia_entre_pontos(pontos_de_controle[index], pontos_de_controle[0]))
		{	
			do{
				index++;
				if(index == quantidade_de_pontos-1) break;
			}
			while(distancia_atual > distancia_entre_pontos(pontos_de_controle[index], pontos_de_controle[0]));
			
		}
		else if (index > 1)
		{
			while(distancia_atual < distancia_entre_pontos(pontos_de_controle[index-1], pontos_de_controle[0]))
			{
				index--;
				if(index == 1) break;
			}
		}
		if(index < quantidade_de_pontos-1)
		{
			for(int i = quantidade_de_pontos-1; i > index; i--)
			{
				pontos_de_controle[i][X] = pontos_de_controle[i-1][X];
				pontos_de_controle[i][Y] = pontos_de_controle[i-1][Y];
			}
		}
		// Adiciona o novo ponto
		pontos_de_controle[index][X] = x;
		pontos_de_controle[index][Y] = y;

		
		printf("[ ");
		for(int i = 0; i < quantidade_de_pontos; i++)
		{
			printf("(%f,%f), ",pontos_de_controle[i][X],pontos_de_controle[i][Y]);
		}
		printf("(%f,%f) ]\n",pontos_de_controle[quantidade_de_pontos][X],pontos_de_controle[quantidade_de_pontos][Y]);
		
		
	}
	// Incrementa o contador
	quantidade_de_pontos++;
}
void remove_ponto(float x, float y)
{
	if(quantidade_de_pontos > 0)
	{
		quantidade_de_pontos--;
		if(quantidade_de_pontos >= 2)
		{
			pontos_de_controle[quantidade_de_pontos-1][X] = pontos_de_controle[quantidade_de_pontos][X];
			pontos_de_controle[quantidade_de_pontos-1][Y] = pontos_de_controle[quantidade_de_pontos][Y];
		}	
	}
	else
	{
		init_game(fase); // Efeito pratico apenas em blocos aleatórios
	}
}


// Funcao que trata do evento do clik
// Adiciona e remove pontos de controle
void mouse(int button, int state, int x, int y) 
{
	// passando os vlaores x e y para os valores entre -1 e 1
	float px = (2.0*x) / WINDOW_WIDTH - 1.0;
	float py = 1.0 - (2.0*y) / WINDOW_HEIGHT;

	// Adiciona pontos e curvas
	// Jogador cria pontos clicando no botao esquerdo do mouse
	if (quantidade_de_pontos < MAX_PONTOS - 1) 
	{
		// Criacao de pontos e curvas
		if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) 
		{	
			// incere_ponto(px,py);
			incere_ponto2(px,py);
			// Recalcula toda a curva 
			bezier(quantidade_de_pontos);
			// Atualiza a tela
			glutPostRedisplay();
		}
	}

	// Remocao de pontos e curvas
	// Jogador remove pontos clicanco no botao direito do mouse
	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN )
	{	
		remove_ponto(px,py);
		// Recalcula toda a curva 
		bezier(quantidade_de_pontos);
		// Atualiza a tela
		glutPostRedisplay();
	}

}

// Funcao que desenha na tela
void display(void) {
	
	// Indica a cor da tela
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClear(GL_COLOR_BUFFER_BIT);

	// Desenha os blocos do mapa
	mapa();

	// Define cor atual
	glColor3f(1.0, 1.0, 1.0);

	// Desenha pontos de controle
	glPointSize(4.0);
	glBegin(GL_POINTS);
	for (int i = 0; i < quantidade_de_pontos; i++) {
		glVertex2d(pontos_de_controle[i][X], pontos_de_controle[i][Y]);
	}
	glEnd();

	// Desenha a curva de Bezier)
	if(quantidade_de_pontos >= 2){
		glBegin(GL_LINE_STRIP);
		for (int i = 0; i < LIMIT; i++)
			glVertex2d(curva[i][X], curva[i][Y]);
		glEnd();
		if(colisao()){
			printf("Colidiu\n");
		}else{
			printf("Parabens!!! %d pontos usados!\n", quantidade_de_pontos - 2);
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
	if(argc > 2){
		quantidade_de_obistaculos = atoi(argv[2]);
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
	glutCreateWindow("Bezier Game");
	//Inicia a funcao display()
	glutDisplayFunc(display);
	//Recebe comandos do mouse
	glutMouseFunc(mouse);
	//Inicia gerenciador de eventos
	glutMainLoop();

	return 0;
}
// **********************************************************************
// PUCRS/Escola PolitŽcnica
// COMPUTA‚ÌO GRçFICA
//
// Programa basico para criar aplicacoes 2D em OpenGL
//
// Marcio Sarroglia Pinho
// pinho@pucrs.br
// **********************************************************************

// Para uso no Xcode:
// Abra o menu Product -> Scheme -> Edit Scheme -> Use custom working directory
// Selecione a pasta onde voce descompactou o ZIP que continha este arquivo.
//

#include <iostream>
#include <cmath>
#include <ctime>
#include <fstream>


using namespace std;

#ifdef WIN32
#include <windows.h>
#include <glut.h>
#else
include <sys/time.h>
#endif

#ifdef __APPLE__
#include <GLUT/glut.h>
#endif

#ifdef __linux__
#include <glut.h>
#endif

#include "Ponto.h"
#include "Poligono.h"

#include "Temporizador.h"

#define FILE2 "Triangulo.txt"
#define FILE1 "Retangulo.txt"

Temporizador T;
double AccumDeltaT=0;

Poligono A, B, uni,intersec, diferencaAB, diferencaBA, A_aux, B_aux;

// Limites l—gicos da ‡rea de desenho
Ponto Min, Max;
Ponto Meio, Terco, Largura;

bool desenha = false;

float angulo=0.0;

// **********************************************************************
//    Calcula o produto escalar entre os vetores V1 e V2
// **********************************************************************
double ProdEscalar(Ponto v1, Ponto v2)
{
    return v1.x*v2.x + v1.y*v2.y+ v1.z*v2.z;
}
// **********************************************************************
//    Calcula o produto vetorial entre os vetores V1 e V2
// **********************************************************************
void ProdVetorial (Ponto v1, Ponto v2, Ponto &vresult)
{
    vresult.x = v1.y * v2.z - (v1.z * v2.y);
    vresult.y = v1.z * v2.x - (v1.x * v2.z);
    vresult.z = v1.x * v2.y - (v1.y * v2.x);
}
/* ********************************************************************** */
/*                                                                        */
/*  Calcula a interseccao entre 2 retas (no plano "XY" Z = 0)             */
/*                                                                        */
/* k : ponto inicial da reta 1                                            */
/* l : ponto final da reta 1                                              */
/* m : ponto inicial da reta 2                                            */
/* n : ponto final da reta 2                                              */
/*                                                                        */
/* s: valor do par‰metro no ponto de intersecao (sobre a reta KL)         */
/* t: valor do par‰metro no ponto de intersecao (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;


    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // n‹o h‡ intersecao

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // h‡ intersecao
}
///////////////////////////////////////////////////////////////////////////////////////////
Ponto return_point_intersec2d(Ponto k, Ponto l, Ponto m, Ponto n)
{
    double det,s,t;
    Ponto newPonto;

    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);


    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    newPonto = Ponto(k.x + s*(l.x - k.x), k.y + s*(l.y - k.y));
    return newPonto; // h‡ intersecao
}


// **********************************************************************
bool HaInterseccao(Ponto k, Ponto l, Ponto m, Ponto n)
{
    int ret;
    double s,t;

    ret = intersec2d( k,  l,  m,  n, s, t);
    if (!ret) return false;
    if (s>=0.0 && s <=1.0 && t>=0.0 && t<=1.0)
    {

       // cout << m.x + t*(n.x - m.x) << ":" << m.y + t*( n.y - m.y) << endl;
        return true;
    }
    else return false;
}
// **********************************************************************
//
// **********************************************************************
void LePoligono(const char *nome, Poligono &P)
{
    ifstream input;
    input.open(nome, ios::in);
    if (!input)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Lendo arquivo " << nome << "...";
    string S;
    int nLinha = 0;
    unsigned int qtdVertices;

    input >> qtdVertices;

    for (int i=0; i< qtdVertices; i++)
    {
        double x,y;
        // Le cada elemento da linha
        input >> x >> y;
        if(!input)
            break;
        nLinha++;
        P.insereVertice(Ponto(x,y));
    }
    cout << "Poligono lido com sucesso!" << endl;

}

// **********************************************************************
//
// **********************************************************************
void init()
{
    Ponto MinAux, MaxAux;

    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Le o primeiro pol’gono
    LePoligono(FILE1, A);
    A.obtemLimites(Min, Max);
    cout << "\tMinimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();

    // Le o segundo pol’gono
    LePoligono(FILE2, B);
    B.obtemLimites(MinAux, MaxAux);
    cout << "\tMinimo:"; MinAux.imprime();
    cout << "\tMaximo:"; MaxAux.imprime();

    // Atualiza os limites globais ap—s cada leitura
    Min = ObtemMinimo(Min, MinAux);
    Max = ObtemMaximo(Max, MaxAux);

    cout << "Limites Globais" << endl;
    cout << "\tMinimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();
    cout <<  endl;

    // Ajusta a largura da janela l—gica
    // em funcao do tamanho dos pol’gonos
    Largura.x = Max.x-Min.x;
    Largura.y = Max.y-Min.y;

    // Calcula 1/3 da largura da janela
    Terco = Largura;
    double fator = 1.0/3.0;
    Terco.multiplica(fator, fator, fator);

    // Calcula 1/2 da largura da janela
    Meio.x = (Max.x+Min.x)/2;
    Meio.y = (Max.y+Min.y)/2;
    Meio.z = (Max.z+Min.z)/2;




}

double nFrames=0;
double TempoTotal=0;
// **********************************************************************
//
// **********************************************************************
void animate()
{
    double dt;
    dt = T.getDeltaT();
    AccumDeltaT += dt;
    TempoTotal += dt;
    nFrames++;

    if (AccumDeltaT > 1.0/30) // fixa a atualizacao da tela em 30
    {
        AccumDeltaT = 0;
        //angulo+=0.05;
        glutPostRedisplay();
    }
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
}
// **********************************************************************
//  void reshape( int w, int h )
//  trata o redimensionamento da janela OpenGL
//
// **********************************************************************
void reshape( int w, int h )
{
    // Reset the coordinate system before modifying
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    // Define a area a ser ocupada pela area OpenGL dentro da Janela
    glViewport(0, 0, w, h);
    // Define os limites logicos da area OpenGL dentro da Janela
    glOrtho(Min.x,Max.x,
            Min.y,Max.y,
            0,1);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}
// **********************************************************************
//
// **********************************************************************
void DesenhaEixos()
{
    glBegin(GL_LINES);
    //  eixo horizontal
        glVertex2f(Min.x,Meio.y);
        glVertex2f(Max.x,Meio.y);
    //  eixo vertical 1
        glVertex2f(Min.x + Terco.x,Min.y);
        glVertex2f(Min.x + Terco.x,Max.y);
    //  eixo vertical 2
        glVertex2f(Min.x + 2*Terco.x,Min.y);
        glVertex2f(Min.x + 2*Terco.x,Max.y);
    glEnd();
}
// **********************************************************************
//  void display( void )
//
// **********************************************************************
void display( void )
{

	// Limpa a tela coma cor de fundo
	glClear(GL_COLOR_BUFFER_BIT);

    // Define os limites lógicos da área OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();

    //Desenha os dois pol’gonos no canto superior esquerdo
    glPushMatrix();
    glTranslatef(0, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,1,0); // R, G, B  [0..1]
    A.desenhaPoligono();
    glColor3f(1,0,0); // R, G, B  [0..1]
    B.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol’gono A no meio, acima
    glPushMatrix();
    glTranslatef(Terco.x, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(0,1,0); // R, G, B  [0..1]
    uni.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol’gono B no canto superior direito
    glPushMatrix();
    glTranslatef(Terco.x*2, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,0,0); // R, G, B  [0..1]
    B.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol’gono A no canto inferior esquerdo
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,1,0); // R, G, B  [0..1]
    A.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol’gono B no meio, abaixo
    glPushMatrix();
    glTranslatef(Terco.x, 0, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,0,0); // R, G, B  [0..1]
    B.desenhaPoligono();
    glPopMatrix();

	glutSwapBuffers();
}

void insereVertice(Ponto p, Poligono &Poli, Ponto novo)
{
    int iter;
     for(iter = 0; iter < Poli.getNVertices(); iter++) ///procura onde devemos inserir o ponto novo
                {
                    if(Poli.getVertice(iter).x == p.x && Poli.getVertice(iter).y == p.y )
                    {
                         Poli.insereVertice(novo, iter+1);
                         return;
                    }
                }
}

void testaInterseccao(Poligono &p1, Poligono &p2, bool markedP1[], bool markedP2[])
{
    bool alreadyAdded, debug, isInside = false;
    Ponto auxA1,auxA2, auxB1,auxB2, newPonto;
    ///// teste de interseccao
    for(int i = 0; i < p1.getNVertices(); i++)
    {
        for(int j = 0; j < p2.getNVertices(); j++)
        {
            if(j == p2.getNVertices() - 1 )
            {
                auxB1 = p2.getVertice(j);
                auxB2 = p2.getVertice(0);
            }
            else
            {
                auxB1 = p2.getVertice(j);
                auxB2 = p2.getVertice(j+1);
            }
            if (i == p1.getNVertices() - 1)
            {
                auxA1 = p1.getVertice(i);
                auxA2 = p1.getVertice(0);
            }
            else
            {
                auxA1 = p1.getVertice(i);
                auxA2 = p1.getVertice(i+1);
            }

            debug = HaInterseccao(auxA1,auxA2,auxB1,auxB2);

            if(debug == true) /// insere o vertice da interseccao se ela existir
            {

                alreadyAdded = false;
                newPonto = return_point_intersec2d(auxA1,auxA2,auxB1,auxB2);
               // for(int iter = 0; iter < p1.getNVertices(); iter++)
           //     {
                    if((newPonto.x == auxA2.x && newPonto.y == auxA2.y) || (newPonto.x == auxA1.x && newPonto.y == auxA1.y)) /// Os pontos estavam sendo inseridos multiplas vezes, isso impede q o mesmo ponto apareca denovo
                    {
                     //   cout << "Already Added" << endl;
                        alreadyAdded = true;
                    }

            //    }
                if(alreadyAdded == false)
                {

                    insereVertice(auxA1,p1,newPonto);
                    insereVertice(auxB1,p2,newPonto);
                    /*
                    cout << "Intersecao em ";
                    auxA1.imprime();
                    auxA2.imprime();
                    auxB1.imprime();
                    auxB2.imprime();
                    cout << "Ponto ";

                    */
                    newPonto.imprime();
                    cout << "PONTO ADICIONADO" <<endl;
                }
            }


        }

    }
}
bool findPoint(Poligono a, Ponto p)
{
    for(int iter = 0; iter < a.getNVertices(); iter++)
        {
            if((p.x == a.getVertice(iter).x && p.y == a.getVertice(iter).y)) /// Os pontos estavam sendo inseridos multiplas vezes, isso impede q o mesmo ponto apareca denovo
            {
             return true;
            }

        }
    return false;
}
void classifica(Poligono a, Poligono b, bool (&mark)[100])
{
    Ponto classificacao, auxClass;
    for(int i = 0; i < a.getNVertices(); i++)
    {
        if(i == a.getNVertices() - 1 )
                classificacao = Ponto((a.getVertice(i).x + a.getVertice(0).x)/2,(a.getVertice(i).y + a.getVertice(0).y)/2); ///Ponto medio da aresta
            else
                classificacao = Ponto((a.getVertice(i).x + a.getVertice(i+1).x)/2,(a.getVertice(i).y + a.getVertice(i+1).y)/2); ///Ponto medio da aresta

        auxClass = Ponto(0.0,classificacao.y);
        int intersecCount = 0;
        for(int j = 0; j < b.getNVertices();j++)
        {
            if(j == b.getNVertices() -1)  ///ligar o ultimo ponto com o primeiro
            {
                /*
                classificacao.imprime();
                auxClass.imprime();
                b.getVertice(j).imprime();
                b.getVertice(0).imprime();
                cout << endl;
                */
                if(HaInterseccao(classificacao,auxClass,b.getVertice(j),b.getVertice(0)) == true)
                {

                    intersecCount++;
                }
            }
            else
            {
                /*
                classificacao.imprime();
                auxClass.imprime();
                b.getVertice(j).imprime();
                b.getVertice(j+1).imprime();
                cout << endl;
                */
                if(HaInterseccao(classificacao,auxClass,b.getVertice(j),b.getVertice(j+1)) == true)
                {

                    intersecCount++;
                }
            }

        }
        if (intersecCount % 2 == 0) ///se o numero de interseccoes for par o ponto esta fora do poligono
            mark[i] = false;
        else
            mark[i] = true;
    }
}
//########################### UNIAO #####################################
Poligono uniao(Poligono a, Poligono b)
{
    unsigned long vertA, vertB;
    bool debug;
    vertA = a.getNVertices();
    vertB = b.getNVertices();
    Poligono newA = Poligono(), newB = Poligono(), uni = Poligono();
    Ponto classificacao, auxClass;
    bool markA[100], markB[100], isInside = false;

    /// Criacao de poligonos auxiliares, para mantermos os poligonos originais intactos
    for (int i = 0; i < vertA; i++)
    {
        newA.insereVertice(a.getVertice(i),i);
    }
    for(int i = 0; i < vertB; i++)
    {
        newB.insereVertice(b.getVertice(i),i);
    }
    ////////////////////////////////////////////////////////////////////


    testaInterseccao(newA,newB,markA,markB);

    classifica(newA,B,markA);
    classifica(newB,A,markB);


    newA.imprime();
    cout << endl;
    for(int i = 0; i < newA.getNVertices(); i++)
        cout << markA[i];
    cout << endl;
    newB.imprime();
    cout << endl;
    for(int i = 0; i < newB.getNVertices(); i++)
        cout << markB[i];

    cout << endl;

    int verts = newA.getNVertices();
    bool currentPoly = false; //true = poligono 1, false = poligono 2
    bool visitedA[100], visitedB[100], done = false;
    bool changePoly = false;
    Ponto last;
    cout << changePoly << endl;

   while(!done) /// Construcao do poligono uniao A-B
   {
    bool auxA,auxB;
       if(currentPoly == true) ///Comecamos analisando o poligono A
       {
           auxA = true; ///variavel de controle que sinaliza se todos os vertices ja foram visitados
           for(int i = 0; i < newA.getNVertices();i++)
           {
                if(newA.getVertice(i).x == last.x && newA.getVertice(i).y == last.y)
                    changePoly = false; /// variavel de controle que sinaliza que trocamos o Poligono analisado, ela serve para encontrarmos de qual vertices devemos continuar quando trocarmos
                if(changePoly == false)
                {
                    if (visitedA[i] == false)
                    {
                       auxA = false; /// marcar como falso significa que encontramos um vertice ainda nao visitado, ou seja ainda nao acabamos
                       if (markA[i] == false)
                       {
                                cout << "adding A" << " ";
                                newA.getVertice(i).imprime();
                                cout << endl;
                                /// adicionamos a aresta ao poligono, caso a marcacao dizer que a aresta esta fora
                                if(uni.getNVertices() == 0)
                                    uni.insereVertice(newA.getVertice(i));
                                if(i == newA.getNVertices() -1 )
                                        uni.insereVertice(newA.getVertice(0));
                                else
                                        uni.insereVertice(newA.getVertice(i+1));
                                visitedA[i] = true;
                       }
                       else
                       {
                           if(uni.getNVertices() != 0)
                           {

                            cout << "Troca A ";
                            newA.getVertice(i).imprime();
                            cout << endl;
                            last = uni.getVertice(uni.getNVertices()-1);;
                            currentPoly = !currentPoly; /// se a aresta estiver dentro do outro poligono, trocamos para ele
                            changePoly = true;
                           }
                            visitedA[i] = true;
                       }
               }
           }


        }
        if(auxA == true)
            if(auxB == true) ///Se os dois poligonos estiverem com seus vertices todos visitados terminamos o processo
                done = true;
            else
                currentPoly = !currentPoly;
       }
       else
       {
            auxB = true;
            for(int i = 0; i < newB.getNVertices();i++)
            {
                if(newB.getVertice(i).x == last.x && newB.getVertice(i).y == last.y)
                    changePoly = false;
                if(changePoly == false)
                {
                    if (visitedB[i] == false)
                   {
                       auxB = false;
                       if (markB[i] == false)
                       {
                                cout << "adding B " << " ";
                                newB.getVertice(i).imprime();
                                cout << endl;
                                if(uni.getNVertices() == 0)
                                    uni.insereVertice(newB.getVertice(i));
                                if(i == newB.getNVertices() -1 )
                                        uni.insereVertice(newB.getVertice(0));
                                else
                                        uni.insereVertice(newB.getVertice(i+1));

                                visitedB[i] = true;
                       }
                       else
                       {
                            if(uni.getNVertices() != 0)
                           {
                               cout << "Troca B ";
                                newB.getVertice(i).imprime();
                                cout << endl;
                               visitedB[i] = true;
                               last = uni.getVertice(uni.getNVertices()-1);
                               currentPoly = !currentPoly;
                               changePoly = true;
                           }

                       }
                   }
                }

            }
           if(auxB == true)
                if(auxA == true)
                    done = true;
                else
                    currentPoly = !currentPoly;

       }
   }
    uni.removeVertice();
    uni.imprime();
    cout << endl;
    return uni;
}
//#######################################################################
Poligono intersecao(Poligono a, Poligono b)
{

    return a;
}

// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo nœmero de segundos e informa quanto frames
// se passaram neste per’odo.
// **********************************************************************
void ContaTempo(double tempo)
{
    Temporizador T;

    unsigned long cont = 0;
    cout << "Inicio contagem de " << tempo << "segundos ..." << flush;
    while(true)
    {
        tempo -= T.getDeltaT();
        cont++;
        if (tempo <= 0.0)
        {
            cout << "fim! - Passaram-se " << cont << " frames." << endl;
            break;
        }
    }

}
// **********************************************************************
//  void keyboard ( unsigned char key, int x, int y )
//
// **********************************************************************

void keyboard ( unsigned char key, int x, int y )
{

	switch ( key )
	{
		case 27:        // Termina o programa qdo
			exit ( 0 );   // a tecla ESC for pressionada
			break;
        case 't':
            ContaTempo(3);
            break;
        case 'u':
            uni = uniao(A,B);
            break;
        case 'i':
            intersec = intersecao(A,B);
        case ' ':
            desenha = !desenha;
        break;
		default:
			break;
	}
}
// **********************************************************************
//  void arrow_keys ( int a_keys, int x, int y )
//
//
// **********************************************************************
void arrow_keys ( int a_keys, int x, int y )
{
	switch ( a_keys )
	{
		case GLUT_KEY_UP:       // Se pressionar UP
			glutFullScreen ( ); // Vai para Full Screen
			break;
	    case GLUT_KEY_DOWN:     // Se pressionar UP
								// Reposiciona a janela
            glutPositionWindow (50,50);
			glutReshapeWindow ( 700, 500 );
			break;
		default:
			break;
	}
}

// **********************************************************************
//  void main ( int argc, char** argv )
//
// **********************************************************************



int  main ( int argc, char** argv )
{
    cout << "Programa OpenGL" << endl;

    glutInit            ( &argc, argv );
    glutInitDisplayMode (GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB );
    glutInitWindowPosition (0,0);

    // Define o tamanho inicial da janela grafica do programa
    glutInitWindowSize  ( 1000, 500);

    // Cria a janela na tela, definindo o nome da
    // que aparecera na barra de titulo da janela.
    glutCreateWindow    ( "Primeiro Programa em OpenGL" );

    // executa algumas inicializações
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // será chamada automaticamente quando
    // for necessário redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalidacao da tela. A funcao "display"
    // será chamada automaticamente sempre que a
    // m‡quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // será chamada automaticamente quando
    // o usuário alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // será chamada automaticamente sempre
    // o usuário pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" será chamada
    // automaticamente sempre o usuário
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}

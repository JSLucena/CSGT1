// **********************************************************************
// PUCRS/Escola Polit?cnica
// COMPUTA??O GR?FICA
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


//// TR-RT => NOK, acho q eh lixo
//// TR-O1 => Parece OK
//// TR-O2 => OK, LGTM
//// TR-O3 => OK, LGTM
//// TR-04 => OK, LGTM
//// RT-O1 => NOK, alem do lixo caiu no caso especial
//// RT-02 => SEM TESTE
//// RT-03 => FALHA TOTAL
//// RT-04 => SEM TESTE
//// O1-O2 => NOK, caiu no caso especial
//// O1-O3 => OK, LGTM
//// 01-04 => Parece OK
//// O2-O3 => NAO SERA TRATADO
//// O2-O4 => SEM TESTE
//// O3-O4 => FALHA TOTAL
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

#define FILE2 "Objeto3.txt"
#define FILE1 "Objeto4.txt"

Temporizador T;
double AccumDeltaT=0;

Poligono A, B, uni,intersec, diferencaAB, diferencaBA, A_aux, B_aux;

Poligono newA = Poligono(), newB = Poligono();
bool markA[100], markB[100];
// Limites l?gicos da ?rea de desenho
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
/* s: valor do par?metro no ponto de intersecao (sobre a reta KL)         */
/* t: valor do par?metro no ponto de intersecao (sobre a reta MN)         */
/*                                                                        */
/* ********************************************************************** */
int intersec2d(Ponto k, Ponto l, Ponto m, Ponto n, double &s, double &t)
{
    double det;


    det = (n.x - m.x) * (l.y - k.y)  -  (n.y - m.y) * (l.x - k.x);

    if (det == 0.0)
        return 0 ; // n?o h? intersecao

    s = ((n.x - m.x) * (m.y - k.y) - (n.y - m.y) * (m.x - k.x))/ det ;
    t = ((l.x - k.x) * (m.y - k.y) - (l.y - k.y) * (m.x - k.x))/ det ;

    return 1; // h? intersecao
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
    return newPonto; // h? intersecao
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

////##########################################################################
void insereVerticeSearch(Ponto p, Poligono &Poli, Ponto novo)
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
    bool alreadyAddedp1,alreadyAddedp2, debug, isInside = false;
    Ponto auxA1,auxA2, auxB1,auxB2, newPonto;
    /// teste de interseccao
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

                alreadyAddedp1 = false;
                alreadyAddedp2 = false;
                newPonto = return_point_intersec2d(auxA1,auxA2,auxB1,auxB2);
                for(int iter = 0; iter < p1.getNVertices(); iter++)
                {
                    if(newPonto.x == p1.getVertice(iter).x && newPonto.y == p1.getVertice(iter).y) /// Os pontos estavam sendo inseridos multiplas vezes, isso impede q o mesmo ponto apareca denovo
                    {
                        cout << "Already Added P1" << endl;
                        alreadyAddedp1 = true;
                    }

               }
               for(int iter = 0; iter < p2.getNVertices(); iter++)
                {
                    if(newPonto.x == p2.getVertice(iter).x && newPonto.y == p2.getVertice(iter).y) /// Os pontos estavam sendo inseridos multiplas vezes, isso impede q o mesmo ponto apareca denovo
                    {
                        cout << "Already Added P2" << endl;
                        alreadyAddedp2 = true;
                    }

               }
                if(alreadyAddedp1 == false)
                {
                    insereVerticeSearch(auxA1,p1,newPonto);
                }
                if(alreadyAddedp2 == false)
                    insereVerticeSearch(auxB1,p2,newPonto);

                newPonto.imprime();
                cout << "PONTO ADICIONADO" <<endl;
                }
            }


        }

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

/////#########################################################################




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
Poligono uniao(Poligono a, Poligono b)
{
    unsigned long vertA, vertB;
    bool debug;
    vertA = a.getNVertices();
    vertB = b.getNVertices();
  //  Poligono newA = Poligono(), newB = Poligono();
    Poligono uni = Poligono();
    Ponto classificacao, auxClass;
//    bool markA[100], markB[100], isInside = false;
    int verts = newA.getNVertices();
    bool currentPoly = true; //true = poligono 1, false = poligono 2
    bool visitedA[100], visitedB[100], done = false;
    for(int i = 0; i < 100;i++)
    {
        visitedA[i] = false;
        visitedB[i] = false;
    }
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
                {
                    cout << "Found";
                    last.imprime();
                    cout << endl;
                    changePoly = false; /// variavel de controle que sinaliza que trocamos o Poligono analisado, ela serve para encontrarmos de qual vertices devemos continuar quando trocarmos
                }
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
                {
                    cout << "Found ";
                    last.imprime();
                    cout << endl;
                     changePoly = false;
                }
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

                               last = uni.getVertice(uni.getNVertices()-1);
                               currentPoly = !currentPoly;
                               changePoly = true;
                           }
                           visitedB[i] = true;

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

   for(int i =0; i < uni.getNVertices()- 1;i++) ///Remover algum vertice que foi adicionado duplo
   {
        for(int j = i+1; j < uni.getNVertices();j++)
        {
            if(uni.getVertice(i).x == uni.getVertice(j).x && uni.getVertice(i).y == uni.getVertice(j).y)
                uni.removeVertice(j);
        }
   }

    uni.imprime();
    cout << endl;
    return uni;
}
//#######################################################################
Poligono intersecao(Poligono a, Poligono b)
{

    unsigned long vertA, vertB;
    bool debug;
    vertA = a.getNVertices();
    vertB = b.getNVertices();
//    Poligono newA = Poligono(), newB = Poligono()
    Poligono inter = Poligono();
    Ponto classificacao, auxClass;
   // bool markA[100], markB[100], isInside = false;
    int verts = newA.getNVertices();
    bool currentPoly = true; //true = poligono 1, false = poligono 2
    bool visitedA[100], visitedB[100], done = false;
    bool changePoly = false;
    for(int i = 0; i < 100;i++)
    {
        visitedA[i] = false;
        visitedB[i] = false;
    }
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
                       if (markA[i] == true)
                       {
                                cout << "adding A" << " ";
                                newA.getVertice(i).imprime();
                                cout << endl;
                                /// adicionamos a aresta ao poligono, caso a marcacao dizer que a aresta esta fora
                                if(uni.getNVertices() == 0)
                                    inter.insereVertice(newA.getVertice(i));
                                if(i == newA.getNVertices() -1 )
                                        inter.insereVertice(newA.getVertice(0));
                                else
                                        inter.insereVertice(newA.getVertice(i+1));
                                visitedA[i] = true;
                       }
                       else
                       {
                           if(inter.getNVertices() != 0)
                           {

                            cout << "Troca A ";
                            newA.getVertice(i).imprime();
                            cout << endl;
                            last = inter.getVertice(inter.getNVertices()-1);;
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
                       if (markB[i] == true)
                       {
                                cout << "adding B " << " ";
                                newB.getVertice(i).imprime();
                                cout << endl;
                                if(inter.getNVertices() == 0)
                                    inter.insereVertice(newB.getVertice(i));
                                if(i == newB.getNVertices() -1 )
                                        inter.insereVertice(newB.getVertice(0));
                                else
                                        inter.insereVertice(newB.getVertice(i+1));

                                visitedB[i] = true;
                       }
                       else
                       {
                            if(inter.getNVertices() != 0)
                           {
                               cout << "Troca B ";
                                newB.getVertice(i).imprime();
                                cout << endl;
                               visitedB[i] = true;
                               last = inter.getVertice(inter.getNVertices()-1);
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

    inter.imprime();
    cout << endl;
    return inter;
}
Poligono diferenca(Poligono a, Poligono b, bool mark[100], bool markUm[100])
{
    unsigned long vertA, vertB;
    bool debug;
    vertA = a.getNVertices();
    vertB = b.getNVertices();
 //   Poligono newA = Poligono(), newB = Poligono();
    Poligono diff = Poligono(), invertB = Poligono();
    Ponto classificacao, auxClass;
   // bool markA[100], markB[100],, isInside = false;
    bool invertMarkB[100];

    /// Inverte o poligono B e suas marcacoes em relacao ao A
    for(int i = 0; i < b.getNVertices();i++)
    {
        //bool invertMark;
       // invertMark = mark[b.getNVertices()-1-i];
        invertB.insereVertice(b.getVertice(b.getNVertices()-1 -i ));
       // invertMarkB[i] = mark[b.getNVertices()-1-i];
    }
    invertB.imprime();
    classifica(invertB,a,invertMarkB);
    cout << endl;
    for(int i = 0; i < b.getNVertices(); i++)
        cout << invertMarkB[i];

    cout << endl;


    int verts = a.getNVertices();
    bool currentPoly = true; //true = poligono 1, false = poligono 2
    bool visitedA[100], visitedB[100], done = false;
    bool changePoly = false;
    for(int i = 0; i < 100;i++)
    {
        visitedA[i] = false;
        visitedB[i] = false;
    }
    Ponto last;
    cout << changePoly << endl;
   while(!done) /// Construcao do poligono uniao A-B
   {
    bool auxA,auxB;
       if(currentPoly == true) ///Comecamos analisando o poligono A
       {
           auxA = true; ///variavel de controle que sinaliza se todos os vertices ja foram visitados
           for(int i = 0; i < a.getNVertices();i++)
           {
                if(a.getVertice(i).x == last.x && a.getVertice(i).y == last.y)
                     {
                    cout << "Found";
                    last.imprime();
                    cout << endl;
                    changePoly = false; /// variavel de controle que sinaliza que trocamos o Poligono analisado, ela serve para encontrarmos de qual vertices devemos continuar quando trocarmos
                }
                if(changePoly == false)
                {
                    if (visitedA[i] == false)
                    {
                       auxA = false; /// marcar como falso significa que encontramos um vertice ainda nao visitado, ou seja ainda nao acabamos;
                       if (markUm[i] == false)
                       {
                                cout << "adding A" << " ";
                                a.getVertice(i).imprime();
                                if(i == a.getNVertices() -1 )
                                    a.getVertice(0).imprime();
                                else
                                    a.getVertice(i+1).imprime();
                                cout << endl;
                                /// adicionamos a aresta ao poligono, caso a marcacao dizer que a aresta esta fora
                                if(diff.getNVertices() == 0)
                                    diff.insereVertice(a.getVertice(i));
                                if(i == a.getNVertices() -1 )
                                            diff.insereVertice(a.getVertice(0));
                                else
                                            diff.insereVertice(a.getVertice(i+1));
                                visitedA[i] = true;
                       }
                       else
                       {
                           if(diff.getNVertices() != 0)
                           {

                            cout << "Troca A ";
                            a.getVertice(i).imprime();
                            cout << endl;
                            last = diff.getVertice(diff.getNVertices()-1);;
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
            for(int i = 0; i < invertB.getNVertices();i++)
            {
                if(invertB.getVertice(i).x == last.x && invertB.getVertice(i).y == last.y)
                {
                     {
                    cout << "Found";
                    last.imprime();
                    cout << endl;
                    changePoly = false; /// variavel de controle que sinaliza que trocamos o Poligono analisado, ela serve para encontrarmos de qual vertices devemos continuar quando trocarmos
                }
                }

                if(changePoly == false)
                {
                    if (visitedB[i] == false)
                   {
                        auxB = false;
                       if (invertMarkB[i] == true)
                       {


                               cout << "adding B " << " ";
                                invertB.getVertice(i).imprime();
                                if(i == invertB.getNVertices() -1 )
                                    invertB.getVertice(0).imprime();
                                else
                                    invertB.getVertice(i+1).imprime();
                                cout << endl;
                                if(diff.getNVertices() == 0)
                                    diff.insereVertice(invertB.getVertice(i));
                                if(i == invertB.getNVertices() -1 )
                                            diff.insereVertice(invertB.getVertice(0));
                                else
                                            diff.insereVertice(invertB.getVertice(i+1));

                                visitedB[i] = true;


                       }
                       else
                       {
                            if(diff.getNVertices() != 0)
                           {
                               cout << "Troca B ";
                                invertB.getVertice(i).imprime();
                                cout << endl;
                               visitedB[i] = true;
                               last = diff.getVertice(diff.getNVertices()-1);
                               currentPoly = !currentPoly;
                               changePoly = true;
                           }
                            visitedB[i] = true;

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

   for(int i =0; i < diff.getNVertices()- 1;i++) ///Remover algum vertice que foi adicionado duplo
   {
        for(int j = i+1; j < diff.getNVertices();j++)
        {

            if(diff.getVertice(i).x == diff.getVertice(j).x && diff.getVertice(i).y == diff.getVertice(j).y)
                diff.removeVertice(j);
        }
   }

    diff.imprime();
    cout << endl;
    return diff;
}
void init()
{
    Ponto MinAux, MaxAux;

    // Define a cor do fundo da tela (AZUL)
    glClearColor(0.0f, 0.0f, 1.0f, 1.0f);

    // Le o primeiro pol?gono
    LePoligono(FILE1, A);
    A.obtemLimites(Min, Max);
    cout << "\tMinimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();

    // Le o segundo pol?gono
    LePoligono(FILE2, B);
    B.obtemLimites(MinAux, MaxAux);
    cout << "\tMinimo:"; MinAux.imprime();
    cout << "\tMaximo:"; MaxAux.imprime();

    // Atualiza os limites globais ap?s cada leitura
    Min = ObtemMinimo(Min, MinAux);
    Max = ObtemMaximo(Max, MaxAux);

    cout << "Limites Globais" << endl;
    cout << "\tMinimo:"; Min.imprime();
    cout << "\tMaximo:"; Max.imprime();
    cout <<  endl;

    // Ajusta a largura da janela l?gica
    // em funcao do tamanho dos pol?gonos
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




    /// Criacao de poligonos auxiliares, para mantermos os poligonos originais intactos
    for (int i = 0; i < A.getNVertices(); i++)
    {
        newA.insereVertice(A.getVertice(i),i);
    }
    for(int i = 0; i < B.getNVertices(); i++)
    {
        newB.insereVertice(B.getVertice(i),i);
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


    uni = uniao(A,B);
    intersec = intersecao(A,B);
    diferencaAB = diferenca(newA,newB,markB, markA);
    diferencaBA = diferenca(newB,newA,markA, markB);


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
    /*
    if (TempoTotal > 5.0)
    {
        cout << "Tempo Acumulado: "  << TempoTotal << " segundos. " ;
        cout << "Nros de Frames sem desenho: " << nFrames << endl;
        cout << "FPS(sem desenho): " << nFrames/TempoTotal << endl;
        TempoTotal = 0;
        nFrames = 0;
    }
    */
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

    // Define os limites l?gicos da ?rea OpenGL dentro da Janela
	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<
	// Coloque aqui as chamadas das rotinas que desenham os objetos
	// <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<

	glLineWidth(1);
	glColor3f(1,1,1); // R, G, B  [0..1]
    DesenhaEixos();

    //Desenha os dois pol?gonos no canto superior esquerdo
    glPushMatrix();
    glTranslatef(0, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,1,0); // R, G, B  [0..1]
    A.desenhaPoligono();
    glColor3f(1,0,0); // R, G, B  [0..1]
    B.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol?gono uniao no meio, acima
    glPushMatrix();
    glTranslatef(Terco.x, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(0,1,0); // R, G, B  [0..1]
    uni.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol?gono intersecao no canto superior direito
    glPushMatrix();
    glTranslatef(Terco.x*2, Meio.y, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(1,0,1); // R, G, B  [0..1]
    intersec.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol?gono A-B canto inferior esquerdo
    glPushMatrix();
    glTranslatef(0, 0, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(0,1,1); // R, G, B  [0..1]
    diferencaAB.desenhaPoligono();
    glPopMatrix();

    // Desenha o pol?gono B-A no meio, abaixo
    glPushMatrix();
    glTranslatef(Terco.x, 0, 0);
    glScalef(0.33, 0.5, 1);
    glLineWidth(2);
    glColor3f(0,1,1); // R, G, B  [0..1]
    diferencaBA.desenhaPoligono();
    glPopMatrix();

	glutSwapBuffers();
}



bool findPoint(Poligono a, Ponto p)
{
    if(a.getNVertices() != 0)
        for(int iter = 0; iter < a.getNVertices(); iter++)
            {
                if((p.x == a.getVertice(iter).x && p.y == a.getVertice(iter).y)) /// Os pontos estavam sendo inseridos multiplas vezes, isso impede q o mesmo ponto apareca denovo
                {
                    p.imprime();
                    a.getVertice(iter).imprime();
                    cout << "true";
                 return true;
                }

            }
        cout << "false";
    return false;
}

//########################### UNIAO #####################################


salvaPoligono()
{
    ofstream output;
    string nome = "Uniao.txt";
    output.open(nome, ios::out);
    if (!output)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Escrevendo arquivo " << nome << "...";
    output << uni.getNVertices() << endl;
    for (int i=0; i< uni.getNVertices(); i++)
    {
        output << uni.getVertice(i).x <<" "<< uni.getVertice(i).y << endl;
    }
    cout << "Fechando arquivo" << endl;
    output.close();

    nome = "Interseccao.txt";
    output.open(nome, ios::out);
    if (!output)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Escrevendo arquivo " << nome << "...";
    output << intersec.getNVertices() << endl;

    for (int i=0; i< intersec.getNVertices(); i++)
    {
        output << intersec.getVertice(i).x <<" "<< intersec.getVertice(i).y << endl;
    }
    cout << "Fechando arquivo" << endl;
    output.close();

    nome = "DiferencaAB.txt";
    output.open(nome, ios::out);
    if (!output)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Escrevendo arquivo " << nome << "...";
    output << diferencaAB.getNVertices() << endl;
    for (int i=0; i< diferencaAB.getNVertices(); i++)
    {
        output << diferencaAB.getVertice(i).x <<" "<< diferencaAB.getVertice(i).y << endl;
    }
    cout << "Fechando arquivo" << endl;
    output.close();

    nome = "DiferencaBA.txt";
    output.open(nome, ios::out);
    if (!output)
    {
        cout << "Erro ao abrir " << nome << ". " << endl;
        exit(0);
    }
    cout << "Escrevendo arquivo " << nome << "...";
    output << diferencaBA.getNVertices() << endl;
    for (int i=0; i< diferencaBA.getNVertices(); i++)
    {
        output << diferencaBA.getVertice(i).x <<" "<< diferencaBA.getVertice(i).y << endl;
    }
    cout << "Fechando arquivo" << endl;
    output.close();


}
// **********************************************************************
// ContaTempo(double tempo)
//      conta um certo n?mero de segundos e informa quanto frames
// se passaram neste per?odo.
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
		case 27:
            // Termina o programa qdo
            salvaPoligono();
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
            break;
        case 'd':
             diferencaBA = diferenca(newB,newA,markA, markB);
            diferencaAB = diferenca(newA,newB,markB, markA);

            break;
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

    // executa algumas inicializa??es
    init ();

    // Define que o tratador de evento para
    // o redesenho da tela. A funcao "display"
    // ser? chamada automaticamente quando
    // for necess?rio redesenhar a janela
    glutDisplayFunc ( display );

    // Define que o tratador de evento para
    // o invalidacao da tela. A funcao "display"
    // ser? chamada automaticamente sempre que a
    // m?quina estiver ociosa (idle)
    glutIdleFunc(animate);

    // Define que o tratador de evento para
    // o redimensionamento da janela. A funcao "reshape"
    // ser? chamada automaticamente quando
    // o usu?rio alterar o tamanho da janela
    glutReshapeFunc ( reshape );

    // Define que o tratador de evento para
    // as teclas. A funcao "keyboard"
    // ser? chamada automaticamente sempre
    // o usu?rio pressionar uma tecla comum
    glutKeyboardFunc ( keyboard );

    // Define que o tratador de evento para
    // as teclas especiais(F1, F2,... ALT-A,
    // ALT-B, Teclas de Seta, ...).
    // A funcao "arrow_keys" ser? chamada
    // automaticamente sempre o usu?rio
    // pressionar uma tecla especial
    glutSpecialFunc ( arrow_keys );

    // inicia o tratamento dos eventos
    glutMainLoop ( );

    return 0;
}

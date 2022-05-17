/*
** UC: 21111 - Sistemas Operativos
** e-fólio B  2021-22 (pteste.c)
**
** Aluno: 2100927 - Ivo Baptista 
*/

#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <pthread.h>
#include <string.h>
#define MAXSTR 255      /* numero maximo da string */
#define MAXTRF 30       /* numero maximo de tarefas */

/************************************************************
    Variáveis globais
************************************************************/
void *tarefa(void *arg);          /* prototipo funcao tarefa */
pthread_mutex_t mtx_num_erros;  
pthread_mutex_t *mtx_processada;  /* ponteiro para o array de mutexes a ser alocado */
int total_de_linhas, total_de_erros;

//--------------------------------------------------
//      definição da struct
//--------------------------------------------------
typedef struct Numeros{
   int numero1, numero2, esperado, resultado, processada;
} Numeros;

Numeros numeros[1000];

//--------------------------------------------------
// Funcão Combinacoes
//
// Função combinaçoes fornecida para teste
//--------------------------------------------------
int Combinacoes(int n, int r) {
  int i, combinacoes;

  if (n < r || r < 1)
     return 0;

  i = 1;
  combinacoes = 1;
  while (i <= r) {
     combinacoes *= (n - r + i);
     combinacoes /= i;
     i++;
  }

  return combinacoes;
}

/************************************************************
    Programa Principal
************************************************************/
int main(int argc, char **argv) {

/************************************************************
    Variáveis locais
************************************************************/
   char frase[MAXSTR];
   FILE *file;
   file = fopen(argv[2], "r");
   if (file == NULL)
   {
      printf("Erro ao abrir o ficheiro!\n");
      exit(1);
   }

   int linhas = 0;

   int i,r,n,va;
   
   pthread_t      trfid[MAXTRF];    /* variavel para ID das tarefas */
   pthread_attr_t trfatr;           /* variavel para atributos das tarefas */

   
//--------------------------------------------------
//  Verificar validade dos argumentos 
//--------------------------------------------------
   va=1;    /* flag validade argumentos */
   if( argc!=3 )
      va=0;              /* numero de argumentos invalido */
      else {
         n= atoi(argv[1]);
         if( !((n>=1 && n<=15) || n==20 || n==30) )
         va=0;           /* numero de tarefas invalido */
      }
   if( !va ) {
      printf("\n Deve respeitar os argumentos \n com n=1-15, 20, 30 mais o ficheiro de texto casos.txt\n\n");
      exit(1);
   }

/************************************************************
        iniciar a tarefa principal
************************************************************/
    while( fgets(frase, MAXSTR, file) )  //Inicio do ciclo while
    {
      numeros[linhas].processada = 0;  /* variavel para contabilizar os procesados */
      // Lee e converte os dados do ficheiro de string para inteiros
      char *num;
      num =strtok(frase, " ");
      numeros[linhas].numero1 = atoi(num);  
      num = strtok(NULL, " ");
      numeros[linhas].numero2 = atoi(num);
      num = strtok(NULL, " ");
      numeros[linhas].esperado = atoi(num);         
      linhas++; // contador total de linhas que lee no ficheiro txt
      // DEBUG
      // printf("valor do ficheiro txt = %d %d %d\n", x, y, z);

    } // Fechando o while

    total_de_linhas = linhas;
    total_de_erros = 0; 

    /* Testando se memoria foi alocada correctamente */
    mtx_processada = malloc(total_de_linhas * sizeof(pthread_mutex_t));
    if( mtx_processada == (pthread_mutex_t *) NULL) {
      perror("Erro alocando o array de mutexes");
      return 1;
    }
/************************************************************
            inicializar array de mutex
************************************************************/
    for( i = 0; i < total_de_linhas; i++ ) 
       pthread_mutex_init(mtx_processada+i, NULL);
   /* inicializar variavel de atributos com valores por defeito */
    pthread_attr_init(&trfatr);
    pthread_mutex_init( &mtx_num_erros, NULL); 
   /* modificar estado de desacoplamento para "joinable" */
    pthread_attr_setdetachstate(&trfatr, PTHREAD_CREATE_JOINABLE); 
   /* inicializar mutex com valores por defeito para os atributos */
   
   /* inicializar estruturas de dados que servem de argumento das tarefas */
    for(i=0; i<n; i++) {

      //DEBUG       
      // printf("leer Numeros: %d %d %d\n", numeros[i].numero1, numeros[i].numero2,numeros[i].esperado); 
      // printf("Erro no caso %d: input (%d,%d) esperado/observado:%d/%d\n\n",linhas, numeros[i].numero1, numeros[i].numero2,numeros[i].esperado,numeros[i].resultado);

/************************************************************
      criar e iniciar execucao de tarefa
************************************************************/
      r = pthread_create(&trfid[i], &trfatr, tarefa, (void*) NULL);
      if( r ) {
         /* erro ! */
         perror("Erro na criacao da tarefa!");
         exit(1);
      }
   }

/************************************************************
     esperar que as tarefas criadas terminem
************************************************************/
   for(i=0; i<n; i++)
      pthread_join( trfid[i], (void **) NULL);
   /* libertar recursos associados ao mutex */
   pthread_mutex_destroy(&mtx_num_erros);
   for(i = 0; i < total_de_linhas; i++)
      pthread_mutex_destroy(mtx_processada+i); //destruimos o mutex de &mtx_processada, que podemos tambem escrever mtx_processada+i ou mtx_processada[i]
   free(mtx_processada); // liberando a memória alocada para todo o array.
      /*    a terminar a tarfefa principal    */

/************************************************************
      Mostra os resultados
************************************************************/
 printf("Resultados:\n");
 printf("Sucesso: %i\n",total_de_linhas-total_de_erros);
 printf("Falhas: %i\n",total_de_erros);
 printf("Total casos: %i\n",total_de_linhas);

   for(i=0; i<total_de_linhas-1; i++)
      if( numeros[i].esperado != numeros[i].resultado ) {
       printf("Erro no caso %d: input (%d,%d) esperado/observado:%d/%d\n",i+1, numeros[i].numero1, numeros[i].numero2,numeros[i].esperado,numeros[i].resultado);
      }
   fclose(file); //aqui fecha o ficheiro 
   return 0; //0 - SUCESSO ou 1 - ERRO -- return(0);
}
// FIM PROGRAMA

/************************************************************
    Função de tarefas
************************************************************/

void *tarefa(void *arg)
{
    int i;  
    /* A iniciar sub-tarefa */

    /* Ciclo for das tarefas */
    for(i=0; i< total_de_linhas; i++) {
       
        pthread_mutex_lock(mtx_processada+i);
        if( numeros[i].processada ) {  // protegemos a i-ésima posição do array numeros.
            pthread_mutex_unlock(mtx_processada+i);
            continue;
        } else {  

            numeros[i].processada = 1;
            pthread_mutex_unlock(mtx_processada+i);
        }
        numeros[i].resultado = Combinacoes(numeros[i].numero1, numeros[i].numero2); //Chamando a função Combinacoes(n,r)  
        if( numeros[i].esperado != numeros[i].resultado ) { 
            //DEBUG
            //printf("Erro no caso %d: input (%d,%d) esperado/observado:%d/%d\n\n",i, numeros[i].numero1, numeros[i].numero2,numeros[i].esperado,numeros[i].resultado);
            pthread_mutex_lock(&mtx_num_erros);
            total_de_erros++;
            pthread_mutex_unlock(&mtx_num_erros);
        }   
        
    }   
    /* acesso deve ser o mais breve possivel */

    return (void*) NULL;
 }
/* EOF */

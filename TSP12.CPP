// Compilado con Turbo C++ ver. 3.0
//========================================================
/* ENUNCIADO DEL TSP (Traveling Salesman Problem):
Una persona tiene que recorrer un cierto número de 
ciudades que están todas interconectadas con todas.
Es decir, siempre se puede ir de una hacia otra en 
cualquier dirección.
Otro dato que se tiene, es cuánto sale ir de una 
a otra. A los efectos prácticos, vamos a suponer que 
viajar desde la ciudad 1 hasta la ciudad 2, sale lo 
mismo que viajar desde 2 hasta 1.
El problema consiste en construir un itinerario que pase 
por todas las ciudades una sola vez, y que termine en el 
mismo lugar inicial, pero con la particularidad que sea 
el más barato, costo minimo. ¡Eso es todo!
---------------------------------------------------------
El Problema del Viajante de Comercio (TSP), es un problema
de complejidad NP-completo. Esto es así, porque el número
de posibles soluciones crece exponencialmente con el número
de nodos del grafo (ciudades), y rápidamente sobrepasa las 
capacidades de cálculo de las computadoras mas potentes.
-------------------------------------------------------*/

#include <dos.h>
#include <io.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>

struct loading
{char numeracion[3];   //numeracion del archivo de entrada
 int N;		//N=numero total de ciudades
 int *cost;
};

struct nodo
{int *estado;
 int prof;		//prof=nivel de profundidad del arbol
 int f;			  //funcion de evaluacion f=h+g , siendo g el costo
 struct nodo *next;	// inter-ciudad acumulado y h una estimacion del
};			// costo por recorrer (minimalista)

struct punteros_lista
//estructura de puntereros a lista de nodos: abiertos y actuales
{struct nodo *abiertos;
 struct nodo *actual;
};

//=============================================================
void main(int argc, char *argv[])
{
//-----prototipos y variables-----

void argumentos(int);

struct loading *load_matrix(int,char **);
struct loading *fdin;
//printf("\n El string numeracion es:%s.\n",fdin->numeracion);


//hacemos calculos de minimizacion de costos interciudad, antes de iniciar la busqueda
int *minimos_iniciales(struct loading*);

struct nodo *nod_actual,*abiertos,*nod_sucesor,*nod_nulo;
struct punteros_lista *extraido;

struct nodo *insert_abiertos(struct nodo *, struct nodo *, struct nodo *);

void extract_abiertos(struct nodo *,struct nodo *, struct punteros_lista *);

struct time tini, tfin;


FILE *fileout;
char *namefileout="out";  //comienzo del nombre del archivo de salida

long cantidad_nodos=1;

unsigned char dmin=0,dseg=0,dhund=0;  //variables delta de tiempo

int *aux_estado, *buffer;

int ciudad_inicial=0,j,i,x,n,*cost_min;

//-----fin de prototipos y variables-----

argumentos(argc);	//verificamos los argumentos ingresados

gettime(&tini);  //tomamos el tiempo inicial de ejecucion

//llenado de la matriz de costos interciudad
fdin=load_matrix(argc,argv);
n=fdin->N;
//printf("\nEl string numeracion es:%s.\n",fdin->numeracion);

//creacion del nodo nulo, la idea es apuntar a NULL como terminador de la lista
nod_nulo=(struct nodo *)malloc(sizeof(struct nodo));

abiertos=nod_nulo; //inicializo a abiertos como vacio

//definimos el vector cost_min, que es el resultado de los minimos_iniciales
cost_min=minimos_iniciales(fdin);

//creacion de estructura para extraer nodos de abiertos
extraido = (struct punteros_lista *)malloc(sizeof(struct punteros_lista));

//creacion del primer nodo inicial, apuntado pro el nodo actual
nod_actual=(struct nodo *)malloc(sizeof(struct nodo));
nod_actual->prof=0;
nod_actual->estado=(int *)malloc(sizeof(int));
*nod_actual->estado=ciudad_inicial;
nod_actual->f=0;
for (i=0;i<n;i++)
	{nod_actual->f=nod_actual->f+*(cost_min+i);	//es el nivel 0 de profundidad
	}																						//ya que: f=h porque aun no cambiamos
																							//de estado (g=0). Por eso sumamos
																							//todos los cost_min
nod_actual->next=nod_nulo;
//fin de la creacion del nodo actual en actual

do
	{//se pueden generar sucesores a actual, la cantidad max. de sucesores=prof
	 //esta definida por la cant. de ciudades
		if (nod_actual->prof<n-1)
			{i=0;
			 while (i<n) //se pueden seguir generando sucesores?
				{for (j=0;(j<=nod_actual->prof)&&(*(nod_actual->estado+j)!=i);j++);
				 if (j==nod_actual->prof+1) //no se visito la ciudad analizada
					{//generamos un sucesor
					 nod_sucesor = (struct nodo *)malloc(sizeof(struct nodo));
					 if (nod_sucesor==NULL)
						 {printf("\nERROR: No hay memoria suficiente para generar un sucesor.\n");
						  printf("ERROR en la profundiad nod_actual ---> %d\n",nod_actual->prof);
							exit(1);
						 };
					 nod_sucesor->prof=nod_actual->prof+1;
					 nod_sucesor->estado=(int *)malloc((nod_sucesor->prof+1)*sizeof(int));
					 if (nod_sucesor->estado==NULL)
						{printf("\nERROR: No hay memoria suficiente para generar un sucesor.\n");
						 printf("ERROR en la profundiad nod_sucesor ---> %d\n",nod_sucesor->prof);
						 exit(1);
						};
					 for (j=0;j<=nod_actual->prof;j++)
						{*(nod_sucesor->estado+j)=*(nod_actual->estado+j);
						}
					 *(nod_sucesor->estado+j)=i;
					 nod_sucesor->f=nod_actual->f-cost_min[i]+*(fdin->cost+i+*(nod_sucesor->estado+j-1)*n);
					 cantidad_nodos++;
					 //terminamos de generar un sucesor
					 abiertos=insert_abiertos(nod_sucesor,abiertos,nod_nulo);
					}// if  encontro que la ciudad no fue visitada
				 i++;
				}//si estamos aca, se pudieron generar
			 free(nod_actual->estado);
			 free(nod_actual);
			}

		else //no se puede generar ningun sucesor sin repetir estado
			{//agrego la ciudad_inicial al final
			 aux_estado=nod_actual->estado;
			 nod_actual->prof++;
			 nod_actual->estado=(int *)malloc((nod_actual->prof+1)*sizeof(int));
			 if (nod_actual->estado==NULL)
				{printf("\nERROR: No hay memoria suficiente para generar un sucesor.\n");
				 printf("ERROR en la profundiad nod_actual (2) ---> %d\n",nod_actual->prof);
				 exit(1);
				};
			 for (j=0;j<nod_actual->prof;j++)
				{*(nod_actual->estado+j)=*(aux_estado+j);
			}
			 *(nod_actual->estado+j)=ciudad_inicial;
			 free(aux_estado);
			 nod_actual->f=nod_actual->f-cost_min[ciudad_inicial]+*(fdin->cost+ciudad_inicial+*(nod_actual->estado+j-1)*n);
			 //fin: agrego ciudad_inicial al final

		 //introducimos actual en abiertos
			 abiertos=insert_abiertos(nod_actual,abiertos,nod_nulo);
			}
		//extraer actual de abiertos
		extract_abiertos(abiertos,nod_nulo, extraido);
		nod_actual=extraido->actual;
		abiertos=extraido->abiertos;
	}while(nod_actual->prof!=n); //condicion del DO-WHILE

// Si estamos aca, es porque terminamos el algoritmo y encontramos el camino optimo.

//Creamos el archivo de salida
printf("\n");
strcat(namefileout,fdin->numeracion);
strcat(namefileout,"gdg.txt");	//damos el nombre de salida solicitado
//printf("\n El string namefileout es:%s y numeracion es:%s.\n",namefileout,fdin->numeracion);
fileout=fopen(namefileout,"w");
if (fileout==NULL)
	{printf("\nNo se pudo abrir el archivo de datos.\n");
	}
else
	{printf("\nEl archivo de datos de salida sera:%s.\n",namefileout);
	}
printf("\nLa secuencia de estados a recorrer seria:\n");
for (i=0;i<=n;i++)		//recorremos los estados minimos calculados
	{fprintf(fileout,"%d ",*(nod_actual->estado+i)+1);	//el +1 es porque el primer
	 printf("%d\n",*(nod_actual->estado+i)+1);					//estado es el 1 y no el 0.
	}
fprintf(fileout,"\n%d\n",nod_actual->f);
printf("\nEl costo total es: %d\n",nod_actual->f);
fprintf(fileout,"%d\n",cantidad_nodos);
printf("\nLa cantidad de nodos que se abrieron fue: %d\n",cantidad_nodos);

delay(250);	// le agregamos 1/4 de seg para ver el funcionamiento
gettime(&tfin);

/*--------------------------------------------------
En dos.h esta definida la estructura de tiempo

struct time {
  unsigned char  ti_min;   // minutes
  unsigned char  ti_hour;  // hours
  unsigned char  ti_hund;  // hundredths of seconds
  unsigned char  ti_sec;   // seconds
};
--------------------------------------------------*/

//reacomodamos los tiempos, para que no nos de un tiempo de precesamiento negativo
if (tfin.ti_hund<tini.ti_hund)
	{tfin.ti_hund=tfin.ti_hund+100;
	 tfin.ti_sec--;
	}
if (tfin.ti_sec<tini.ti_sec)
	{tfin.ti_sec=tfin.ti_sec+60;
	 tfin.ti_min--;
	}
/*
printf("\nDesglose de tiempos:\n");
printf("min inicial %d\n",tini.ti_min);
printf("seg inicial %d\n",tini.ti_sec);
printf("centesimas de seg iniciales %d\n",inicial.ti_hund);
printf("min inicial %d\n",tfin.ti_min);
printf("seg inicial %d\n",tfin.ti_sec);
printf("centesimas de seg finales %d\n",tfin.ti_hund);
*/

dmin=tfin.ti_min-tini.ti_min;
dseg=tfin.ti_sec-tini.ti_sec;
dhund=tfin.ti_hund-tini.ti_hund;

printf("\nDemora del algoritmo=%dmin %dseg %dcentesimas de seg.\n",dmin,dseg,dhund);

fprintf(fileout,"\n%d : %d . %d;",dmin,dseg,dhund);

fclose(fileout);  //cerramos el archivo de salida
}

//=============================================================
//verificacion de cantidad de parametros indicados
void argumentos(int argc)
{
 switch (argc)
	{case 1:printf("ATENCION: No se ingreso argumento.\n");
		printf("Un ejemplo seria: TSPx IN_xx.TXT <ENTER>\n");
		exit(1);
	 case 3:printf("ATENCION: Se ha ingresado mal el argumento.\n");
		printf("Un ejemplo seria: TSPx IN_xx.TXT <ENTER>\n");
		exit(1);
	 case 4:printf("ATENCION: Se ha ingresado mal el argumento.\n");
		printf("Un ejemplo seria: TSPx IN_xx.TXT <ENTER>\n");
		exit(1);
	 case 2:printf("Se ha ingresado correctamente el argumento.\n");
		printf("Si no funciona, verifique que el archivo se encuentra en el path.\n");
		break;
	 default:printf("Buuuu!!!   Nene dejale la compu a tu papi.\n");
		 exit(1);
	}
}


//=============================================================
struct loading *load_matrix(int argc, char *argv[])
{
struct loading *aux;
char caracter, *caracteres;
int fd,i,j,k;	//fd:file descriptor
/*
//vemos la numeracion del archivo de entrada con formato: IN_xx.txt
aux->numeracion[0]=argv[1][3];	//segundo string (nombre del archivo), primer digito
aux->numeracion[1]=argv[1][4];	//segundo string (nombre del archivo), segundo digito
aux->numeracion[2]=NULL;
printf("\nLa numeracion del archivo de entrada es:%s",aux->numeracion);
*/
fd=open(argv[1], O_RDONLY | O_TEXT);	//abrimos el archivo en solo lectura y texto
//fin calculo numeracion

//desde aqui obtenemos N
caracter='a';   //tiene que ser igual a cualquier cosa, menos ";"
for (i=0;caracter!=';';i++)	// detector de ";"
	{read(fd, &caracter, 1);
	}
lseek(fd, 0, 0);
caracteres=(char *)malloc(i*sizeof(char));
read(fd, caracteres,i);
*(caracteres+i-1)=NULL;	//agrego el null al final del string caracteres sino el atoi() no anda
												//caracteres contiene la cantidad de ciudades en formato ascii
aux=(struct loading *)malloc(sizeof(struct loading));
aux->N=atoi(caracteres);
free(caracteres);
//ya tenemos N

//desde aqui llenado de costos
//pido memoria para la matriz bidimensional de costos ya que el tamaño depende del archivo
aux->cost=(int *)malloc((aux->N*aux->N)*sizeof(int));
lseek(fd,2,1);

for (i=0;i!=(aux->N-1);i++)
	{for (j=i+1;j!=(aux->N);j++)
		{caracter='a';   //tiene que ser igual a cualquier cosa menos ";"
		 for (k=0;caracter!=';';k++)
			{read(fd, &caracter,1);
			}
		 lseek(fd,-k,1);
		 caracteres = (char *)malloc(k*sizeof(char));
		 read(fd,caracteres,k);
		 *(caracteres+k-1)=NULL;
		 *(aux->cost+i+j*aux->N)=atoi(caracteres);
		 free(caracteres);
		 *(aux->cost+j+i*aux->N)=*(aux->cost+i+j*aux->N);
		}
	}
close(fd);
//sobreescribimos la diagonal principal con un costo cercano a infinito
for (i=0;i<aux->N;i++)
	{*(aux->cost+i+i*aux->N)=32767;	//maximo valor del tipo int
	} //fin del llenado de la diagonal

//vemos la numeracion del archivo de entrada con formato: IN_xx.txt
aux->numeracion[0]=argv[1][3];	//segundo string (nombre del archivo), primer digito
aux->numeracion[1]=argv[1][4];	//segundo string (nombre del archivo), segundo digito
aux->numeracion[2]=NULL;
printf("\nLa numeracion del archivo de entrada es:%s",aux->numeracion);

//fin del llenado de costos
return(aux);
}



//=============================================================
int* minimos_iniciales(struct loading *fdin)
{
/*almacena en un vector de long. N, el costo minimo interciudad de cada
columna de la matriz de costos interciudad.
EJ.:
	a- El costo minimo que hay para llegar de la cuidad 1 a cualquier otra
	b- El costo minimo que hay para llegar de la cuidad 2 a cualquier otra
	c- El costo minimo que hay para llegar de la cuidad 3 a cualquier otra
	d- etc
*/

int i,j,*minima;
minima=(int *)malloc(fdin->N*sizeof(int));

for (j=0;j<fdin->N;j++)
	{*(minima+j)=*(fdin->cost+j*fdin->N);
	 for (i=0;i<fdin->N;i++)
		{if (*(fdin->cost+j*fdin->N+i)<*(minima+j))
			{*(minima+j)=*(fdin->cost+j*fdin->N+i);
		  }
		}
	}
return(minima);
}//NOTA: esto se realiza una sola vez en todo el programa!!!



//=============================================================
struct nodo *insert_abiertos(struct nodo *sucesor,struct nodo *abiertos,struct nodo *nulo)
{
struct nodo *aux, *aux_anterior;
if (abiertos==nulo) //la lista de nodos abiertos esta vacia?
	{abiertos=sucesor;	//si
	 abiertos->next=nulo;
	 return(abiertos);
	}
else //la lista de nodos abiertos no esta vacia
	{aux=abiertos;
	 while (sucesor->f>=aux->f) //sucesor es igual o peor que el nodo de abiertos examinado?
		{if (aux->next==nulo)//es el nodo examinado el ultimo de abiertos?
			{aux->next=sucesor;
			 sucesor->next=nulo;
			 return(abiertos);
			}
		else //el nodo examinado de abiertos no es el ultimo
			{aux_anterior=aux;
			 aux=aux->next;
			}
		}
	sucesor->next=aux;
	if (aux==abiertos)
		{abiertos=sucesor;
		}
	else
		{aux_anterior->next=sucesor;
		}
	return(abiertos);
	}
}


//=============================================================
void extract_abiertos(struct nodo *abiertos, struct nodo *nulo, struct punteros_lista *extraido)
{
if (abiertos->next==nulo) //hay un solo nodo en abiertos?
	{extraido->abiertos=nulo;	//apuntamos al final
	}
else //hay mas de un nodo en abiertos
	{extraido->abiertos=abiertos->next;	//apuntamos al siguiente
	}
extraido->actual=abiertos;	//siempre apuntamos al comienzo
}


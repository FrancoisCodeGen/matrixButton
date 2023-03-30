#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

#include <wiringPi.h>
#include "matrixButton.h"
#include "check.h"


/* 

Pins PHYSIQUES de la raspberry PI

int rowPins[SIZE_ROW_PINS]          = {13, 15, 29, 31};
int columnPins[SIZE_COLUMN_PINS]    = {22, 37, 35, 33};

*/

//Références des fonctions
void joypiButtonMatrixScruting(int edgeType, void (*function)(int));
void PrepareButtons();
void ThreadFn(void * args);
void ThreadJoypiButtonMatrixScruting(void *args);

//Struct des arguments passés aux Threads
typedef struct 
{
    void (*fn) (int);
    int param;

} fnArgs;

//Tableaux des GPIO
int rowPins[SIZE_ROW_PINS]          = {2, 3, 21, 22};
int columnPins[SIZE_COLUMN_PINS]    = {6, 25, 24, 23};

//Matrices d'Etat
int oldState[SIZE_ROW_PINS][SIZE_COLUMN_PINS] = {
                                {1, 1, 1, 1},
                                {1, 1, 1, 1},
                                {1, 1, 1, 1},
                                {1, 1, 1, 1}
                               };

int currentState[SIZE_ROW_PINS][SIZE_COLUMN_PINS] = {
                                {1, 1, 1, 1},
                                {1, 1, 1, 1},
                                {1, 1, 1, 1},
                                {1, 1, 1, 1}
                               };                               



void PrepareButtons(void)
{   
    //Fonction qui initialise la mise en place de la matrice de bouton

    int i, j;

    for(j = 0; j < SIZE_ROW_PINS; j++)
    {
        pinMode (rowPins[j], INPUT);
        pullUpDnControl(rowPins[j], PUD_UP);
    }

    for (i = 0; i < SIZE_COLUMN_PINS; i++)
    {
        pinMode (columnPins[i], OUTPUT);
        digitalWrite (columnPins[i],  HIGH) ;
    }
}


void joypiButtonMatrixScruting(int edgeType, void (*function)(int))
{
    //Fonction de scruting de la matrice. 
    //Elle permet de détecter les changements d'état de la matrice
    //Elle est appélée toutes les 50 ms et executée dans un thread pour ne pas bloquer le main   
    
    int i, j, delta;
    pthread_t threadID;
    pthread_attr_t attrThread;
    fnArgs *myFn;
    int errorHandler;


    /* thread attribute detached */
    errorHandler = pthread_attr_init(&attrThread);
    PTHREAD_CHECK(errorHandler, "Erreur configuration de l'attribut de thread");

    errorHandler = pthread_attr_setdetachstate(&attrThread, PTHREAD_CREATE_DETACHED);
    PTHREAD_CHECK(errorHandler, "Erreur configuration de l'attribut de thread");

    for (j = 0; j < SIZE_COLUMN_PINS; j++)
    {
        /* On met chaque colonne à la fois sur low pour les analiser */
        digitalWrite(columnPins[j],  LOW);
        for(i = 0; i < SIZE_ROW_PINS; i++)
        {
            currentState[i][j] = digitalRead(rowPins[i]);
            delta = currentState[i][j] - oldState[i][j];

            switch(delta)
            {
                /* si le button ne change pas d'état on n'a rien à faire */
                case 0: 
                    break;

                /* si le button vient d'être appuyé (EDGE RISING) */
                case -1:
                    if (edgeType == INT_EDGE_RISING || edgeType == INT_EDGE_BOTH)
                    {
                        myFn = (fnArgs *) malloc(sizeof(fnArgs));
                        CHECK_NOT(myFn, NULL, "Erreur Malloc");

                        myFn->fn = (void *)function;
                        myFn->param = (i*4)+j+1;

                        //Lancement de la fonction utilisateur dans un Thread
                        errorHandler = pthread_create(&threadID, &attrThread, (void *) ThreadFn, (void *) myFn);
                        PTHREAD_CHECK(errorHandler, "Erreur création Thread");
                    }
                    break;

                /* si le button vient d'être relache (EDGE FALLING) */
                case 1:
                    if (edgeType == INT_EDGE_FALLING || edgeType == INT_EDGE_BOTH)
                    {
                        myFn = (fnArgs *) malloc(sizeof(fnArgs));
                        CHECK_NOT(myFn, NULL, "Erreur Malloc");

                        myFn->fn = (void *)function;
                        myFn->param = (i*4)+j+1;

                        //Lancement de la fonction utilisateur dans un Thread
                        errorHandler = pthread_create(&threadID, &attrThread, (void *) ThreadFn, (void *) myFn);
                        PTHREAD_CHECK(errorHandler, "Erreur création Thread");
                    }
                    break;

                default:
                    break;
            }

            oldState[i][j] = currentState[i][j];
            
        }
        digitalWrite(columnPins[j], HIGH);
    }

}

void ThreadFn(void * args)
{   
    // Fonction qui sera executée dans un thread après l'activation d'un bouton

    //Execution de la fonction
    fnArgs *myFn = (fnArgs *) args;
    myFn->fn(myFn->param);

    printf ("Thread %ld termine\n", pthread_self());
    free(myFn);
    pthread_exit(NULL);
}

void joypiButtonMatrix(int edgeType, void (*function)(int))
{
    //Fonction qui permet de mettre en place la matrice de boutons.
    //Elle initialise la matrice et met en place le thread de scrutting

    
    pthread_t threadID;
    pthread_attr_t attrThread;
    fnArgs *myFn;
    int errorHandler;

    //Initialisation des differents objets
    wiringPiSetup();
    PrepareButtons();

    //Création du thread de scrutting
    /* thread attribute detached */
    errorHandler = pthread_attr_init(&attrThread);
    PTHREAD_CHECK(errorHandler, "Erreur configuration de l'attribut de thread");

    errorHandler = pthread_attr_setdetachstate(&attrThread, PTHREAD_CREATE_DETACHED);
    PTHREAD_CHECK(errorHandler, "Erreur configuration de l'attribut de thread");
    

    myFn = (fnArgs *) malloc(sizeof(fnArgs));
    CHECK_NOT(myFn, NULL, "Erreur Malloc");

    myFn->fn = (void *)function;
    myFn->param = edgeType;

    errorHandler = pthread_create(&threadID, &attrThread, (void *) ThreadJoypiButtonMatrixScruting, (void *) myFn);
    PTHREAD_CHECK(errorHandler, "Erreur lancement Thread");

}

void ThreadJoypiButtonMatrixScruting(void *args)
{
    //Thread de scruting
    
    fnArgs *myArgs = (fnArgs *) args;

    for (;;)
    {
        joypiButtonMatrixScruting(myArgs->param, myArgs->fn);
        usleep(50000);

    }

    free(myArgs);
    pthread_exit(NULL);
}



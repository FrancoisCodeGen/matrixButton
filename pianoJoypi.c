#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wiringPi.h>
#include "matrixButton.h"
#include "softTone.h"

#define BUZZER      1
#define N_SCALE     2


void playNote(int idButton);

//Gamme de notes de notre piano
int scale[NB_BUTTONS] = {N_SCALE*132, N_SCALE*148, N_SCALE*165, N_SCALE*176, 
                         N_SCALE*198, N_SCALE*220, N_SCALE*247, N_SCALE*264, 
                         N_SCALE*297, N_SCALE*330, N_SCALE*352, N_SCALE*396, 
                         N_SCALE*440, N_SCALE*495, N_SCALE*528, N_SCALE*594};

int main (int argc, char *argv[])
{   
    //main du pianoJoypi

    //Inititalisation du buzzer et de la matrice de boutons
    wiringPiSetup();
    softToneCreate(BUZZER); 
    joypiButtonMatrix(INT_EDGE_BOTH, &playNote);
    
    //Attente passive
    pause();

    return EXIT_SUCCESS;
}


void playNote(int idButton)
{   
    //Fonction qui est appelée à chaque fois qu'un bouton est activé

    int static etat = LOW;
    etat = !etat;

    if (etat)
    {   
        //Création d'une PWM avec une fréquence pour réaliser la note
        softToneWrite(BUZZER, scale[idButton-1]);
    }
    else
    {
        softToneWrite(BUZZER, 0);
    }

    fflush(stdout);
    printf ("Button: %d:%d\n", idButton, etat);
}
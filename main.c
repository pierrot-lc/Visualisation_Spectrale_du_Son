#include <SDL/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <fmodex/fmod.h>
#include <dirent.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>

#include "constantes.h"
#include "visualisation.h"
#include "control.h"
#include "choix_musique.h"
#include "menu.h"

int main(int argc, char *argv[])
{
    SDL_Surface *ecran = NULL;

    FMOD_SYSTEM *system;
    FMOD_SOUND *musique;
    FMOD_CHANNEL *canal;
    FMOD_RESULT resultat;//Variables


    FMOD_System_Create(&system);
    FMOD_System_Init(system, 1, FMOD_INIT_NORMAL, NULL);//Initiation FMOD


    SDL_Init(SDL_INIT_VIDEO);
    ecran = SDL_SetVideoMode(LARGEUR_FENETRE, HAUTEUR_FENETRE, 32, SDL_SWSURFACE | SDL_DOUBLEBUF);
    SDL_WM_SetIcon(IMG_Load("Visualisation Spectrale du Son.ico"), NULL);
    SDL_WM_SetCaption("Visualisation Spectrale du Son", NULL);//Initiation SDL

    if(argv[1] != NULL)//Si l'utilisateur à lancé le programme en choisissant une musique
    {
        resultat = FMOD_System_CreateSound(system, argv[1], FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &musique);//On charge la musique
        if(resultat != FMOD_OK)
        {
            fprintf(stderr, "Impossible de lire le fichier mp3\n");
            exit(EXIT_FAILURE);
        }
        FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, musique, 0, &canal);//On la lance

        control(ecran, canal, 0);//Boucle de la visualisation
        FMOD_Sound_Release(musique);
    }

    menuPrincipal(ecran);//Boucle principal

    FMOD_System_Close(system);
    FMOD_System_Release(system);//On quitte FMOD

    SDL_Quit();//On quitte la SDL
    return EXIT_SUCCESS;
}

void initialiserChemin(char *chemin)
{
    FILE *fichier = NULL;//Variable

    fichier = fopen("data/dossier.txt", "r");
    if(fichier == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier contenant l'adresses des musiques.");
        exit(EXIT_FAILURE);
    }

    fgets(chemin, 1000, fichier);//Lecture des informations du fichier

    fclose(fichier);
}

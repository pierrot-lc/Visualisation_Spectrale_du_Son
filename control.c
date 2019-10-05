#include <stdlib.h>
#include <stdio.h>
#include <SDL/SDL.h>
#include <fmodex/fmod.h>
#include <dirent.h>
#include <SDL/SDL_ttf.h>

#include "visualisation.h"
#include "constantes.h"
#include "choix_musique.h"

int control(SDL_Surface *ecran, FMOD_CHANNEL *canal, int liste)
{
    int continuer = 1, tempsActuel, tempsPrecedent, pause = 0;
    float volume = 1;

    SDL_Event event;

    FMOD_BOOL etatLecture;

    tempsPrecedent = SDL_GetTicks();
    SDL_EnableKeyRepeat(500, 100);
    while(continuer)
    {
        FMOD_Channel_IsPlaying(canal, &etatLecture);
        if(!etatLecture)//Si la musique n'est pas lu
        {
            if(!pause)//Si la musique n'est pas en pause
                continuer = 0;//On quitte
        }

        SDL_PollEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                continuer = 0;
                return 0;//On arrete (0 pour arreter la liste de lecture)
                break;
            case SDLK_SPACE:
                if(liste)
                    continuer = 0;//On arrete puis on renvoit 1 pour passer la chanson
                break;
            case SDLK_BACKSPACE:
                if(liste)
                    return 2;//2 pour la chanson précédante (liste de lecture)
                break;
            case SDLK_p:
                if(pause)//Si il y a pause
                {
                    pause = 0;
                    FMOD_Channel_SetPaused(canal, 0);//Play
                }
                else if(!pause)//Si il y a pas pause
                {
                    pause = 1;
                    FMOD_Channel_SetPaused(canal, 1);//Pause
                }
                    break;
            case SDLK_KP_PLUS://On monte le volume
                volume += 0.01;
                FMOD_Channel_SetVolume(canal, volume);
                break;
            case SDLK_KP_MINUS://On baisse le volume
                volume -= 0.01;
                FMOD_Channel_SetVolume(canal, volume);
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));//Début de l'affichage

        tempsActuel = SDL_GetTicks();
        if(tempsActuel - tempsPrecedent < DELAI_RAFRAICHISSEMENT)//Si le temps déroulé entre le dernier appel de SDL_GetTicks est < à 25 ms
        {
            SDL_Delay(DELAI_RAFRAICHISSEMENT - (tempsActuel - tempsPrecedent));//On met pause jusqu'a ce que le temps déroulé soit de 25 ms
        }
        tempsPrecedent = SDL_GetTicks();
        visualisation(ecran, canal);//On affiche la vicualisation du son
        SDL_Flip(ecran);
    }
    return 1;//1 pour passer à la chanson suivante (liste de lecture uniquement)
}

void lireListe(SDL_Surface *ecran, int numeroListe)
{
    int i, numeroMusique = 0, totalMusique = 0, continuer;
    char *positionEntree = NULL;
    char chemin[1000] = "data/listes/";//Chemin de départ pour la recherche de la liste de lecture
    char cheminMusique[100][1000];
    char test[1000] = {0};

    FILE *liste = NULL;

    FMOD_SYSTEM *system;
    FMOD_CHANNEL *canal;
    FMOD_RESULT resultat;

    struct dirent *ent;
    DIR *dir;//Variables

    FMOD_System_Create(&system);
    FMOD_System_Init(system, 1, FMOD_INIT_NORMAL, NULL);

    dir = opendir(chemin);//Initialisation

    for(i = 0; i < numeroListe + 2; i++)//On va chercher le bon fichier
        ent = readdir(dir);

    strcat(chemin, ent->d_name);

    liste = fopen(chemin, "r");//On a le bon chemin pour l'ouverture du fichier contenant les adresses des musiques
    if(liste == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture de la liste de lecture");
        exit(EXIT_FAILURE);
    }


    while(fgets(test, 1000, liste) != NULL)
        totalMusique++;//On compte le nombre de musiques

    rewind(liste);

    for(i = 0; i < totalMusique; i++)//On place toutes les adresses dans cheminMusiques
        {
            fgets(cheminMusique[i], 1000, liste);
            positionEntree = strchr(cheminMusique[i], '\n');
            if(positionEntree != NULL)
                *positionEntree = '\0';//Ne pas oublier le '\0' !
        }

    while(numeroMusique != totalMusique)//Lecture des musiques les une après les autres
    {
        FMOD_SOUND *musique;

        resultat = FMOD_System_CreateSound(system, cheminMusique[numeroMusique], FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &musique);
        if(resultat != FMOD_OK)
        {
            fprintf(stderr, "Impossible de lire le fichier mp3 (%s)\n", cheminMusique[numeroMusique]);
            exit(EXIT_FAILURE);
        }
        FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, musique, 0, &canal);

        continuer = control(ecran, canal, 1);//On récupère la valeur renvoyée par control pour savoir quoi faire
        FMOD_Sound_Release(musique);

        if(continuer == 0)//On arrete
            numeroMusique = totalMusique;
        else if(continuer == 2)//Si on doit revenir une musique en arrière
        {
            if(numeroMusique != 0)//Si on est pas à la premiere musique on peut enlevé un numéro
                numeroMusique--;
        }
        else//Ou sinon on ajoute + 1 au numéro de la musique
            numeroMusique++;
    }
    FMOD_System_Close(system);
    FMOD_System_Release(system);
}

int selectionnerListe(SDL_Surface *ecran)
{

    int continuer = 1, limite = 0, positionFleche = 0, menu = 0, resultat = 0;
    long pos = 0;
    char chemin[1000] = "data/listes/";

    SDL_Event event;

    struct dirent *ent;
    DIR* dir = NULL;//Variables

    SDL_EnableKeyRepeat(500, 100);

    dir = opendir(chemin);
    if(dir == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier des listes de lectures");
        exit(-1);
    }
    // Initialisations terminées

    limite = afficherDir(ecran, dir, pos, positionFleche, chemin);//On commence par l'affichage du dossier
    SDL_Flip(ecran);

    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT://On quitte le programme
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE://On revient au menu
                continuer = 0;
                menu = 1;
                break;
            case SDLK_DOWN:
                if(limite >= pos + 1)//Doit pas être > limite
                    pos++;
                if(positionFleche != 12 && positionFleche != limite)
                    positionFleche++;

                afficherDir(ecran, dir, pos, positionFleche, chemin);
                SDL_Flip(ecran);//On descend de un et on actualise l'affichage du dossier
                break;
            case SDLK_UP:
                if(pos - 1 >= 0)//pos ne peut etre < 0
                    pos--;
                if(positionFleche != 0)
                    positionFleche--;

                afficherDir(ecran, dir, pos, positionFleche, chemin);
                SDL_Flip(ecran);//On monte de un et on actualise l'affichage du dossier
                break;
            case SDLK_RETURN:
                seekdir(dir, pos + 2);//on se place dans le dossier à pos + 2(le '.' et le '..')
                ent = readdir(dir);
                resultat = isMP3(ent->d_name, chemin);
                if(resultat == 3)//Si c'est une liste de lecture
                    continuer = 0;
                else
                {
                    fprintf(stderr, "Erreur lors de l'ouverture de la liste de lecture");
                    exit(EXIT_FAILURE);
                }
                break;
            default:
                pos = isLetter(ecran, &event, dir);//Si une autre touche est appuyée on vérifie si c'est une lettre et si la lettre est trouvée on déplace jusuq'à la lettre
                positionFleche = 5;//on réinitialise à 5
                if(!pos)
                    positionFleche = 0;
                if(pos <= 12)
                    positionFleche = pos;
                afficherDir(ecran, dir, pos, positionFleche, chemin);
                SDL_Flip(ecran);
                break;
            }
            break;
        default:
            break;
        }
    }
    if(!menu)//Si on a pas appuyé sur ECHAP pour revenir dans le menu
    {
        return pos + 1;
    }

    return 0;//Sinon on revient au menu principal
    closedir(dir);
}

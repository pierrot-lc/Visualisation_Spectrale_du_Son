#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <SDL/SDL_ttf.h>
#include <SDL/SDL_image.h>
#include <fmodex/fmod.h>
#include <dirent.h>
#include <ctype.h>

#include "choix_musique.h"
#include "constantes.h"
#include "main.h"
#include "control.h"

void choix_musique(SDL_Surface *ecran)
{
    int continuer = 1, limite = 0, positionFleche = 0, menu = 0;
    long pos = 0;
    char chemin[1000];

    SDL_Event event;

    struct dirent *ent;
    DIR* dir = NULL;

    FMOD_SYSTEM *system;
    FMOD_SOUND *musique;
    FMOD_CHANNEL *canal;//Variables ..

    FMOD_System_Create(&system);
    FMOD_System_Init(system, 1, FMOD_INIT_NORMAL, NULL);//Initialisation FMODex

    initialiserChemin(chemin);

    SDL_EnableKeyRepeat(500, 100);

    dir = opendir(chemin);
    if(dir == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier des musiques");
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
                if(isMP3(ent->d_name, chemin) == 1)//Si c'est une musique
                {
                    strcat(chemin, ent->d_name);//On termine la boucle, on fini le chemin et la musique est lancée
                    continuer = 0;
                }
                else
                {
                    strcat(chemin, ent->d_name);//Sinon on ajoute le nom dans le chemin
                    ajouterSlash(chemin);//On ajoute un '/' pour le prochain nom à rajouter
                    closedir(dir);
                    dir = opendir(chemin);//On ouvre le nouveau dossier
                    if(dir == NULL)//Si c'était un fichier ou si il y a eu des erreurs
                    {
                        fprintf(stderr, "Erreur lors de l'ouverture du dossier %s", ent->d_name);
                        exit(EXIT_FAILURE);
                    }
                    pos = 0;//Réinitialisation des variables
                    positionFleche = 0;
                    limite = afficherDir(ecran, dir, pos, positionFleche, chemin);
                    SDL_Flip(ecran);//Actualisation de l'affichage
                }
                break;
            case SDLK_BACKSPACE:
                pos = 0;
                positionFleche = 0;
                closedir(dir);
                dossierPrecedent(chemin);//On enlève un dossier dans le chemin
                dir = opendir(chemin);//On ouvre le nouveau dossier
                if(dir == NULL)
                    exit(EXIT_FAILURE);
                limite = afficherDir(ecran, dir, pos, positionFleche, chemin);
                SDL_Flip(ecran);//Actualisation de l'affichage
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
        FMOD_System_CreateSound(system, chemin, FMOD_SOFTWARE | FMOD_2D | FMOD_CREATESTREAM, 0, &musique);
        FMOD_System_PlaySound(system, FMOD_CHANNEL_FREE, musique, 0, &canal);//Création de la musique et lecture

        control(ecran, canal, 0);
        FMOD_Sound_Release(musique);
    }

    closedir(dir);
    FMOD_System_Close(system);
    FMOD_System_Release(system);//On quitte tout
}

int afficherDir(SDL_Surface *ecran, DIR *dir, long pos, int posFleche, char *chemin)//Affiche le dossier à l'écran
{
    int resultat = 0, compteur = -1, i = 0;

    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoire = {0, 0, 0};
    SDL_Surface *texte = NULL, *fleche = NULL, *mp3 = NULL, *dossier = NULL, *exe = NULL, *playlist = NULL;
    SDL_Rect positionTexte, positionFleche;

    TTF_Font *police = NULL;

    struct dirent *ent;//Variables..

    positionTexte.x = 5;
    positionTexte.y = 0;
    positionFleche.x = 5;
    positionFleche.y = posFleche * (HAUTEUR_TEXTE + 10);

    fleche = IMG_Load("data/fleche.png");
    mp3 = IMG_Load("data/mp3.png");
    dossier = IMG_Load("data/dossier.png");
    playlist = IMG_Load("data/playlist.png");
    exe = IMG_Load("data/exe.png");//Initialisation des variables

    police = TTF_OpenFont("data/Limousines.ttf", 20);

    if(TTF_Init() == -1)//Initialisation de la TTF
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la TTF : %s\n", TTF_GetError);
        exit(EXIT_FAILURE);
    }

    rewinddir(dir);//Initialisation dirent

    ent = readdir(dir);// Le '.'
    ent = readdir(dir);// Le '..'

    for(i = 0; i < (pos - 12 - (posFleche - 12) ); i++)//On se place dans le dossier en fonction de la pos dans le dossier et de la pos de la flèche
        ent = readdir(dir);


    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));//On commence l'affichage du dossier
    while ((ent = readdir(dir))!= NULL)//Lecture du dossier
    {
        compteur++;
        positionTexte.x = 5 + fleche->w;//Réinitialisation de la position du texte
        texte = TTF_RenderText_Shaded(police, ent->d_name, couleurBlanche, couleurNoire);//Attribution de ent->d_name avec TTF

        resultat = isMP3(ent->d_name, chemin);//On regarde de quel type est le fichier traité
        switch(resultat)//On colle l'incon correspondant
        {
        case 0://Dossier
            SDL_BlitSurface(dossier, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x + dossier->w;
            break;
        case 1://Musique
            SDL_BlitSurface(mp3, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x + mp3->w;
            break;
        case 2://Fichier
            SDL_BlitSurface(exe, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x + exe->w;
            break;
        case 3://Liste de lecture
            SDL_BlitSurface(playlist, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x + playlist->w;
            break;
        }

        SDL_BlitSurface(texte, NULL, ecran, &positionTexte);//Puis on colle le texte
        positionTexte.y = positionTexte.y + HAUTEUR_TEXTE + 10;//On décale la position.y puis on recommence
    }
    SDL_BlitSurface(fleche, NULL, ecran, &positionFleche);//On colle la flèche en fonction de la posFlèche

    TTF_CloseFont(police);
    TTF_Quit();
    return compteur;//Compteur de fichers et dossiers pour déterminer la limite de pos
}

int isMP3(char *chaine, char *cheminBase)//Permet de savoir si c'est une musique ou un dossier
{
    char chemin[1000] = {0};
    int nombreLettres;

    DIR *dir;//Variables

    sprintf(chemin, cheminBase);
    strcat(chemin, chaine);

    dir = opendir(chemin);//Initialisations

    if(dir != NULL)
    {
        closedir(dir);
        return 0;//Dossier
    }
    else
    {
        closedir(dir);
        if(strchr(chaine, '.') == NULL)
            return 2; //Fichier quelconque
    }

    nombreLettres = strlen(chaine);

    while(chaine[nombreLettres] != '.')
        nombreLettres--;

    chaine[nombreLettres + 1] = toupper(chaine[nombreLettres + 1]);//On passe les lettres en majuscules
    chaine[nombreLettres + 2] = toupper(chaine[nombreLettres + 2]);
    chaine[nombreLettres + 3] = toupper(chaine[nombreLettres + 3]);
    chaine[nombreLettres + 4] = toupper(chaine[nombreLettres + 4]);

    if( (chaine[nombreLettres + 1] == 'M' && chaine[nombreLettres + 2] == 'P' && chaine[nombreLettres + 3] == '3') ||//Fichier .mp3
        (chaine[nombreLettres + 1] == 'F' && chaine[nombreLettres + 2] == 'L' && chaine[nombreLettres + 3] == 'A' && chaine[nombreLettres + 4] == 'C') ||//Fichier .flac
        (chaine[nombreLettres + 1] == 'W' && chaine[nombreLettres + 2] == 'A' && chaine[nombreLettres + 3] == 'V') )//Fichier .wav
        return 1;//Fichier musique
    else if(chaine[nombreLettres + 1] == 'L' && chaine[nombreLettres + 2] == 'L' && chaine[nombreLettres + 3] == 'V')
        return 3;//Liste de lecture
    else
    {
        return 2;//Fichier quelconque
    }
}

void ajouterSlash(char *chemin)
{
    int compteur = 0;
    while(chemin[compteur] != '\0')//On se place à la fin du chemin
        compteur++;

    chemin[compteur] = '/';//Place un '/' à la fin
    chemin[compteur + 1] = '\0';//Ajoute le '\0'
}

void dossierPrecedent(char *chemin)
{
    int compteur = 0;
    while(chemin[compteur] != '\0')//Fin du chemin
        compteur++;
    if(chemin[compteur - 1] != '/')
        exit(EXIT_FAILURE);

    compteur -= 2;//On se place juste avant le dernier '/'
    while(chemin[compteur] != '/')//Puis on va jusqu'au '/' précedant
        compteur--;
    chemin[compteur + 1] = '\0';//On coupe la chaine juste après le '/' trouvé
}

int deplacerCaractere(DIR *dir, char caractere, long *posBase)//On modifie la pos jusqu'a la lettre trouvée
{
    char chaine[100] = {0};
    long pos = 0;

    struct dirent *ent;//Variables

    rewinddir(dir);//Initialisation du DIR
    ent = readdir(dir);
    ent = readdir(dir);

    while( (ent = readdir(dir)) != NULL )//On lit tout le dossier
    {
        pos++;
        sprintf(chaine, ent->d_name);
        chaine[0] = toupper(chaine[0]);

        if(chaine[0] == caractere)//Si le caractere a été trouvé
        {
            pos--;
            *posBase = pos;//On modifie avec la nouvelle pos
            return 1;
        }
    }

    rewinddir(dir);

    return 0;
}

long isLetter(SDL_Surface *ecran, SDL_Event *event, DIR *dir)
{
    int resultat;
    long pos;
    switch(event->key.keysym.sym)//Si une lettre a été appuyée
    {
    case SDLK_a:
        resultat = deplacerCaractere(dir, 'Q', &pos);//On regarde si on peut se déplacer et on modifie les valeures de pos et positionFleche
        if(!resultat)
            pos = 0;
        break;
    case SDLK_b:
        resultat = deplacerCaractere(dir, 'B', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_c:
        resultat = deplacerCaractere(dir, 'C', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_d:
        resultat = deplacerCaractere(dir, 'D', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_e:
        resultat = deplacerCaractere(dir, 'E', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_f:
        resultat = deplacerCaractere(dir, 'F', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_g:
        resultat = deplacerCaractere(dir, 'G', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_h:
        resultat = deplacerCaractere(dir, 'H', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_i:
        resultat = deplacerCaractere(dir, 'I', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_j:
        resultat = deplacerCaractere(dir, 'J', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_k:
        resultat = deplacerCaractere(dir, 'K', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_l:
        resultat = deplacerCaractere(dir, 'L', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_SEMICOLON:
        resultat = deplacerCaractere(dir, 'M', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_n:
        resultat = deplacerCaractere(dir, 'N', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_o:
        resultat = deplacerCaractere(dir, 'O', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_p:
        resultat = deplacerCaractere(dir, 'P', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_q:
        resultat = deplacerCaractere(dir, 'A', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_r:
        resultat = deplacerCaractere(dir, 'R', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_s:
        resultat = deplacerCaractere(dir, 'S', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_t:
        resultat = deplacerCaractere(dir, 'T', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_u:
        resultat = deplacerCaractere(dir, 'U', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_v:
        resultat = deplacerCaractere(dir, 'V', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_w:
        resultat = deplacerCaractere(dir, 'Z', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_x:
        resultat = deplacerCaractere(dir, 'X', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_y:
        resultat = deplacerCaractere(dir, 'Y', &pos);
        if(!resultat)
            pos = 0;
        break;
    case SDLK_z:
        resultat = deplacerCaractere(dir, 'W', &pos);
        if(!resultat)
            pos = 0;
        break;
    default:
        pos = 0;
        break;
    }
    if(!resultat)//Si aucune lettre n'a été trouvé
        return 0;


    return pos;//On retourne la nouvelle pos
}

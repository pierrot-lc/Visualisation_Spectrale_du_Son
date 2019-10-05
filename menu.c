#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include <fmodex/fmod.h>
#include <dirent.h>
#include <string.h>

#include "main.h"
#include "menu.h"
#include "choix_musique.h"
#include "control.h"
#include "constantes.h"

void menuPrincipal(SDL_Surface *ecran)
{
    char choixMusique[] = "1. Choisir une musique";
    char liste[] = "2. Lire une liste de lecture";
    char options[] = "3. Options";

    int continuer = 1, numeroListe = 0;

    SDL_Surface *choix = NULL, *option = NULL, *lecture = NULL;
    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoire = {0, 0, 0};
    SDL_Event event;
    SDL_Rect position;

    TTF_Font *police = NULL;//Variables

    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la TTF : %s\n", TTF_GetError);
        exit(EXIT_FAILURE);
    }
    police = TTF_OpenFont("data/Limousines.ttf", 40);

    choix = TTF_RenderText_Shaded(police, choixMusique, couleurBlanche, couleurNoire);
    lecture = TTF_RenderText_Shaded(police, liste, couleurBlanche, couleurNoire);
    option = TTF_RenderText_Shaded(police, options, couleurBlanche, couleurNoire);
    position.x = ecran->w/2 - choix->w/2 - option->w/2;//Milieu de l'écran


    while(continuer)
    {
        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));//Début de l'affichage
        position.y = 110;//Milleu de l'écran

        SDL_BlitSurface(choix, NULL, ecran, &position);
        position.y += 50;
        SDL_BlitSurface(lecture, NULL, ecran, &position);
        position.y += 50;
        SDL_BlitSurface(option, NULL, ecran, &position);
        SDL_Flip(ecran);//Actualisation de l'écran

        SDL_WaitEvent(&event);
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
                break;
            case SDLK_KP1:
                choix_musique(ecran);//Choisir une musique à écouter
                break;
            case SDLK_KP2:
                numeroListe = selectionnerListe(ecran);//Choisir une liste de lecture (renvoi 0 si l'utilisateur veut annulé)
                if(numeroListe != 0)
                    lireListe(ecran, numeroListe);
                break;
            case SDLK_KP3:
                menuOptions(ecran);//Menu des options
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }

    TTF_CloseFont(police);
    TTF_Quit();//On quitte TTF
}

void menuOptions(SDL_Surface *ecran)
{
    int continuer = 1, resultat = 0;
    char chemin[] = "1. Changer le dossier par défaut";
    char creerListe[] = "2. Créer une liste de lecture";
    char regarderListe[] = "3. Supprimer une liste de lecture";
    char cheminListe[1000] = "data/listes/";

    TTF_Font *police = NULL;

    SDL_Event event;
    SDL_Surface *chem = NULL, *creeList = NULL, *regarderList;
    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoire = {0, 0, 0};
    SDL_Rect position;//Variables

    if(TTF_Init() == -1)
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la TTF : %s\n", TTF_GetError);
        exit(EXIT_FAILURE);
    }
    police = TTF_OpenFont("data/Limousines.ttf", 40);

    chem = TTF_RenderText_Shaded(police, chemin, couleurBlanche, couleurNoire);
    creeList = TTF_RenderText_Shaded(police, creerListe, couleurBlanche, couleurNoire);
    regarderList = TTF_RenderText_Shaded(police, regarderListe, couleurBlanche, couleurNoire);

    position.x = 10;
    position.y = 110;
    //Initialisations

    SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));//Début de l'affichage
    SDL_BlitSurface(chem, NULL, ecran, &position);
    position.y = position.y + creeList->h + 10;
    SDL_BlitSurface(creeList, NULL, ecran, &position);
    position.y = position.y + creeList->h + 10;
    SDL_BlitSurface(regarderList, NULL, ecran, &position);
    SDL_Flip(ecran);//Fin de l'affichage

    while(continuer)
    {
        SDL_WaitEvent(&event);
        switch(event.type)
        {
        case SDL_QUIT:
            exit(EXIT_SUCCESS);
            break;
        case SDL_KEYDOWN:
            switch(event.key.keysym.sym)
            {
            case SDLK_ESCAPE:
                continuer = 0;//Menu principal
                break;
            case SDLK_KP1:
                dossierDefaut(ecran);//Changer le dossier par défaut
                continuer = 0;
                break;
            case SDLK_KP2://Créer une nouvelle liste de lecture
                resultat = nomListe(ecran, cheminListe);//Nom de la nouvelle liste (renvoi 0 pour revenir au menu)
                if(resultat)//Si l'utilisateur n'a pas voulu annulé
                    selectionnerMusique(ecran, cheminListe);//Selections des musiques

                continuer = 0;
                break;
            case SDLK_KP3://Supprimer une liste de lecture
                supprimerListe(ecran);
                continuer = 0;
                break;
            default:
                break;
            }
            break;
        default:
            break;
        }
    }
    TTF_CloseFont(police);
    TTF_Quit();
}

void dossierDefaut(SDL_Surface *ecran)
{
    char chemin[1000] = {0};
    int continuer = 1, limite, positionFleche = 0;
    long pos = 0;

    SDL_Event event;

    DIR *dir;
    struct dirent *ent;//Variables

    chemin[0] = 'C';
    chemin[1] = ':';
    chemin[2] = '/';
    chemin[3] = '\0';//Dossier C:/ par défaut

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
            case SDLK_SPACE:
                nouveauDossierDefaut(chemin);//On écrit la place du nouveau dossier par défaut
                continuer = 0;
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

    closedir(dir);//On quitte tout
}

void nouveauDossierDefaut(char *chemin)
{
    FILE *fichier = NULL;

    fichier = fopen("data/dossier.txt", "w");//On ouvre et efface le contenu du fichier
    if(fichier == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier contenant l'adresse du dossier principal");
        exit(EXIT_FAILURE);
    }

    fprintf(fichier, chemin);//Ecriture du chemin du dossier par défaut dans le fichier

    fclose(fichier);
}

int nomListe(SDL_Surface *ecran, char *chemin)
{
    int continuer = 1, nombreCaracteres = 0;
    char carac[2] = {0};
    char afficher[] = "Nom de votre liste de lecture :";
    char nom[100] = {0};

    SDL_Surface *nomList = NULL, *affiche = NULL;
    SDL_Event event;
    SDL_Color couleurBlanche = {255, 255, 255}, couleurNoire = {0, 0, 0};
    SDL_Rect position;

    TTF_Font *police = NULL;//Variables

    police = TTF_OpenFont("data/Limousines.ttf", 20);

    if(TTF_Init() == -1)//Initialisation de la TTF
    {
        fprintf(stderr, "Erreur lors de l'initialisation de la TTF : %s\n", TTF_GetError);
        exit(EXIT_FAILURE);
    }

    position.x = 10;
    position.y = 150;
    affiche = TTF_RenderText_Shaded(police, afficher, couleurBlanche, couleurNoire);
    SDL_EnableKeyRepeat(500, 100);

    nom[0] = '_';
    nom[1] = '\0';
    carac[1] = '\0';//Initialisations

    while(continuer)
    {
        nomList = TTF_RenderText_Shaded(police, nom, couleurBlanche, couleurNoire);//Début de l'affichage

        SDL_FillRect(ecran, NULL, SDL_MapRGB(ecran->format, 0, 0, 0));
        position.y = 150;
        SDL_BlitSurface(affiche, NULL, ecran, &position);
        position.y = position.y + affiche->h + 10;
        SDL_BlitSurface(nomList, NULL, ecran, &position);
        SDL_Flip(ecran);//Fin de l'affichage

        SDL_WaitEvent(&event);
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
                return 0;//On retourne au menu principal
                break;
            case SDLK_RETURN:
                nombreCaracteres = strlen(nom);
                nom[nombreCaracteres - 1] = '.';
                nom[nombreCaracteres] = 'l';
                nom[nombreCaracteres + 1] = 'l';
                nom[nombreCaracteres + 2] = 'v';
                nom[nombreCaracteres + 3] = '\0';//On ajoute .llv à la fin du fichier
                strcat(chemin, nom);//Le chemin de la liste de lecture est complet

                continuer = 0;
                break;
            case SDLK_BACKSPACE:
                nombreCaracteres = strlen(nom);
                if(nombreCaracteres != 1)
                {
                    nom[nombreCaracteres - 2] = '_';
                    nom[nombreCaracteres - 1] = '\0';//On enlève un caractere
                }
                break;
            default:
                carac[0] = caractere(&event);//On récupère le caractere tapé par l'utilisateur (revoit '0' si ce n'est pas une lettre)
                if(carac[0] != 0)//Si c'est une lettre
                {
                    nombreCaracteres = strlen(nom);
                    nom[nombreCaracteres - 1] = '\0';//On enlève le '_'
                    strcat(nom, carac);//Ajoute le nouveau caractere
                    nombreCaracteres = strlen(nom);
                    nom[nombreCaracteres] = '_';//Ajoute de nouveau le '_'
                    nom[nombreCaracteres + 1] = '\0';
                }
                break;
            }
            break;
        }
    }

    TTF_CloseFont(police);
    TTF_Quit();//On quitte TTF

    return 1;//Tout c'est passé
}

void selectionnerMusique(SDL_Surface *ecran, char cheminListe[1000])
{
    int continuer = 1, limite = 0, positionFleche = 0, i = 0, nombreCaracteres;
    long pos = 0;
    char chemin[1000], cheminMusique[1000], cheminsToutesMusiques[100][1000] = {0};
    char entree[2] = {0};

    SDL_Event event;

    struct dirent *ent;
    DIR* dir = NULL;

    FILE *fichier = NULL;//Variables ..

    initialiserChemin(chemin);
    entree[0] = '\n';
    entree[1] = '\0';

    fichier = fopen(cheminListe, "w+");
    if(fichier == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du fichier liste de lecture (%s)", cheminListe);
        exit(EXIT_FAILURE);
    }

    SDL_EnableKeyRepeat(500, 100);

    dir = opendir(chemin);
    if(dir == NULL)
    {
        fprintf(stderr, "Erreur lors de l'ouverture du dossier des musiques");
        exit(-1);
    }
    // Initialisations terminées

    limite = afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);//On commence par l'affichage du dossier
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
                break;
            case SDLK_DOWN:
                if(limite >= pos + 1)//Doit pas être > limite
                    pos++;
                if(positionFleche != 12 && positionFleche != limite)
                    positionFleche++;

                afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
                SDL_Flip(ecran);//On descend de un et on actualise l'affichage du dossier
                break;
            case SDLK_UP:
                if(pos - 1 >= 0)//pos ne peut etre < 0
                    pos--;
                if(positionFleche != 0)
                    positionFleche--;

                afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
                SDL_Flip(ecran);//On monte de un et on actualise l'affichage du dossier
                break;
            case SDLK_RETURN:
                seekdir(dir, pos + 2);//on se place dans le dossier à pos + 2(le '.' et le '..')
                ent = readdir(dir);
                if(isMP3(ent->d_name, chemin) == 1)//Si c'est une musique
                {
                    sprintf(cheminMusique, chemin);
                    strcat(cheminMusique, ent->d_name);//Le chemin est complété

                    sprintf(cheminsToutesMusiques[i], cheminMusique);

                    fprintf(fichier, cheminsToutesMusiques[i]);
                    fprintf(fichier, entree);//On ajoute l'adresse de la musique dans le fichier
                    i++;

                    afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
                    SDL_Flip(ecran);
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
                    limite = afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
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
                limite = afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
                SDL_Flip(ecran);//Actualisation de l'affichage
                break;
            default:
                pos = isLetter(ecran, &event, dir);//Si une autre touche est appuyée on vérifie si c'est une lettre et si la lettre est trouvée on déplace jusuq'à la lettre
                positionFleche = 5;//on réinitialise à 5
                if(!pos)
                    positionFleche = 0;
                if(pos <= 12)
                    positionFleche = pos;
                afficherDirListe(ecran, dir, pos, positionFleche, chemin, cheminsToutesMusiques);
                SDL_Flip(ecran);
                break;
            }
            break;
        default:
            break;
        }
    }
    fclose(fichier);
    closedir(dir);
}

int afficherDirListe(SDL_Surface *ecran, DIR *dir, long pos, int posFleche, char *chemin, char cheminToutesMusiques[][1000])//Affiche le dossier à l'écran
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

        resultat = isMP3Liste(ent->d_name, chemin, cheminToutesMusiques);//On regarde de quel type est le fichier traité
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
        case 4://Fichier musique inscrit dans la liste de lecture actuelle
            SDL_BlitSurface(mp3, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x + mp3->w + texte->w + 2;
            SDL_BlitSurface(playlist, NULL, ecran, &positionTexte);
            positionTexte.x = positionTexte.x - texte->w - 2;
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

int isMP3Liste(char *chaine, char *cheminBase, char cheminToutesMusiques[][1000])//Permet de savoir si c'est une musique ou un dossier
{
    char chemin[1000] = {0};
    char *positionEntree = NULL;
    int nombreLettres, continuer = 1, i = 0, y = 0;

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

    nombreLettres = strlen(chemin);

    while(chemin[nombreLettres] != '.')
        nombreLettres--;

    chemin[nombreLettres + 1] = toupper(chemin[nombreLettres + 1]);//On passe les lettres en majuscules
    chemin[nombreLettres + 2] = toupper(chemin[nombreLettres + 2]);
    chemin[nombreLettres + 3] = toupper(chemin[nombreLettres + 3]);
    chemin[nombreLettres + 4] = toupper(chemin[nombreLettres + 4]);

    if( (chemin[nombreLettres + 1] == 'M' && chemin[nombreLettres + 2] == 'P' && chemin[nombreLettres + 3] == '3') ||//Fichier .mp3
        (chemin[nombreLettres + 1] == 'F' && chemin[nombreLettres + 2] == 'L' && chemin[nombreLettres + 3] == 'A' && chemin[nombreLettres + 4] == 'C') ||//Fichier .flac
        (chemin[nombreLettres + 1] == 'W' && chemin[nombreLettres + 2] == 'A' && chemin[nombreLettres + 3] == 'V') )//Fichier .wav
        {
            for(i = 0; i < 100; i++)
            {
                continuer = 1;
                y = 1;
                while(chemin[y] != '\0')
                {
                    if(chemin[y] != cheminToutesMusiques[i][y])
                        continuer = 0;
                    y++;
                }
                if(continuer)
                    return 4;//Fichier musique inscrit dans la liste de lecture
            }
            return 1;//Fichier musique
        }
    else if(chaine[nombreLettres + 1] == 'L' && chaine[nombreLettres + 2] == 'L' && chaine[nombreLettres + 3] == 'V')
        return 3;//Liste de lecture
    else
        return 2;//Fichier quelconque
}

char caractere(SDL_Event *event)
{
    char character = 0;
    switch(event->key.keysym.sym)//On regarde quelle lettre a été appuyée par l'utilisateur
    {
    case SDLK_SPACE:
        character = ' ';
        return character;
    case SDLK_a:
        character = 'Q';
        return character;
        break;
    case SDLK_b:
        character = 'B';
        return character;
        break;
    case SDLK_c:
        character = 'C';
        return character;
        break;
    case SDLK_d:
        character = 'D';
        return character;
        break;
    case SDLK_e:
        character = 'E';
        return character;
        break;
    case SDLK_f:
        character = 'F';
        return character;
        break;
    case SDLK_g:
        character = 'G';
        return character;
        break;
    case SDLK_h:
        character = 'H';
        return character;
        break;
    case SDLK_i:
        character = 'I';
        return character;
        break;
    case SDLK_j:
        character = 'J';
        return character;
        break;
    case SDLK_k:
        character = 'K';
        return character;
        break;
    case SDLK_l:
        character = 'L';
        return character;
        break;
    case SDLK_SEMICOLON:
        character = 'M';
        return character;
        break;
    case SDLK_n:
        character = 'N';
        return character;
        break;
    case SDLK_o:
        character = 'O';
        return character;
        break;
    case SDLK_p:
        character = 'P';
        return character;
        break;
    case SDLK_q:
        character = 'A';
        return character;
        break;
    case SDLK_r:
        character = 'R';
        return character;
        break;
    case SDLK_s:
        character = 'S';
        return character;
        break;
    case SDLK_t:
        character = 'T';
        return character;
        break;
    case SDLK_u:
        character = 'U';
        return character;
        break;
    case SDLK_v:
        character = 'V';
        return character;
        break;
    case SDLK_w:
        character = 'Z';
        return character;
        break;
    case SDLK_x:
        character = 'X';
        return character;
        break;
    case SDLK_y:
        character = 'Y';
        return character;
    break;
    case SDLK_z:
        character = 'W';
        return character;
        break;
    default://Si ce n'est pas une lettre, on fait rien
        return 0;
        break;
    }
}

void supprimerListe(SDL_Surface *ecran)
{
    int continuer = 1, limite = 0, positionFleche = 0, resultat = 0;
    long pos = 0;
    char chemin[1000] = "data/listes/";
    char cheminListe[1000] = {0};

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
                {
                    sprintf(cheminListe, chemin);
                    strcat(cheminListe, ent->d_name);//On colle le chemin complet du fichier
                    remove(cheminListe);//On le supprime

                    if(pos != 0)
                        pos--;
                    if(positionFleche != 0)
                        positionFleche--;
                    limite = afficherDir(ecran, dir, pos, positionFleche, chemin);
                    SDL_Flip(ecran);//Puis on actualise
                }
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
    closedir(dir);
}

#ifndef CHOIX_MUSIQUE_H_INCLUDED
#define CHOIX_MUSIQUE_H_INCLUDED

#ifndef WIN32
    #include <sys/types.h>
#endif

#include<dirent.h>

void choix_musique(SDL_Surface *ecran);
int afficherDir(SDL_Surface *ecran, DIR *dir, long pos, int posFleche, char *chemin);
int isMP3(char *chaine, char *chemin);
void ajouterSlash(char *chemin);
void dossierPrecedent(char *chemin);
int deplacerCaractere(DIR *dir, char caractere, long *posBase);
long isLetter(SDL_Surface *ecran, SDL_Event *event, DIR *dir);

#endif // CHOIX_MUSIQUE_H_INCLUDED

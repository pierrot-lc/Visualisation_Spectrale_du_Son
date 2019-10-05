#ifndef MENU_H_INCLUDED
#define MENU_H_INCLUDED

#ifndef WIN32
    #include <sys/types.h>
#endif

void menuPrincipal(SDL_Surface *ecran);
void menuOptions(SDL_Surface *ecran);
void dossierDefaut(SDL_Surface *ecran);
void nouveauDossierDefaut(char *chemin);
char caractere(SDL_Event *event);
int nomListe(SDL_Surface *ecran, char *chemin);
void selectionnerMusique(SDL_Surface *ecran, char cheminListe[1000]);
int afficherDirListe(SDL_Surface *ecran, DIR *dir, long pos, int posFleche, char *chemin, char cheminToutesMusiques[][1000]);
int isMP3Liste(char *chaine, char *cheminBase, char cheminToutesMusiques[][1000]);
void supprimerListe(SDL_Surface *ecran);

#endif // MENU_H_INCLUDED

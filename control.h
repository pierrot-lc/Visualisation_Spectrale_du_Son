#ifndef CONTROL_H_INCLUDED
#define CONTROL_H_INCLUDED

#ifndef WIN32
    #include <sys/types.h>
#endif

void control(SDL_Surface *ecran, FMOD_CHANNEL *canal, int liste);
void lireListe(SDL_Surface *ecran, int numeroListe);
int selectionnerListe(SDL_Surface *ecran);

#endif // CONTROL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <SDL/SDL.h>
#include <fmodex/fmod.h>

#include "constantes.h"
#include "visualisation.h"

void visualisation(SDL_Surface *ecran, FMOD_CHANNEL *canal)
{
    int i, j, hauteurBarre;
    float spectre[TAILLE_SPECTRE];
    FMOD_RESULT resultat;//Variables

    resultat = FMOD_Channel_GetSpectrum(canal, spectre, TAILLE_SPECTRE, 0, FMOD_DSP_FFT_WINDOW_RECT);//On récupère le spectre de la musique en cours de lecture
    if(resultat != FMOD_OK)
    {
        fprintf(stderr, "Erreur lors de la tentative de récupération du spectre de la musique en cours");
        exit(EXIT_FAILURE);
    }

    SDL_LockSurface(ecran);//On bloque l'écran pour le modifier

    for(i = 0 ; i < LARGEUR_FENETRE - 1; i++)//- 1 à cause d'un bug
    {
        hauteurBarre = spectre[i] * 15 * HAUTEUR_FENETRE;//On agrandi l'intensité de la barre

        if(hauteurBarre > HAUTEUR_FENETRE)
            hauteurBarre = HAUTEUR_FENETRE;

        for(j =  HAUTEUR_FENETRE - hauteurBarre; j < HAUTEUR_FENETRE; j++)//On déssine une colone
        {
            setPixel(ecran, i, j, SDL_MapRGB(ecran->format, 255 - (j / RATIO), j / RATIO, 0));
        }
    }
    SDL_UnlockSurface(ecran);//On débloque l'écran
}

void setPixel(SDL_Surface *surface, int x, int y, Uint32 pixel)
{
    int bpp = surface->format->BytesPerPixel;

    Uint8 *p = (Uint8 *)surface->pixels + y * surface->pitch + x * bpp;

    switch(bpp) {
    case 1:
        *p = pixel;
        break;

    case 2:
        *(Uint16 *)p = pixel;
        break;

    case 3:
        if(SDL_BYTEORDER == SDL_BIG_ENDIAN) {
            p[0] = (pixel >> 16) & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = pixel & 0xff;
        } else {
            p[0] = pixel & 0xff;
            p[1] = (pixel >> 8) & 0xff;
            p[2] = (pixel >> 16) & 0xff;
        }
        break;

    case 4:
        *(Uint32 *)p = pixel;
        break;
    }
}

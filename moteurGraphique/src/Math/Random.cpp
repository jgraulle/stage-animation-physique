/*
 * Random.cpp
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Random.h"

#include <stdlib.h>
#include <time.h>

void Random::init() {
	// initialisation du generateur de nombre aleatoire
	srand(time(NULL));
}

// retourne un entier comprit entre 0 et RAND_MAX
int Random::getInt() {
	return rand();
}

// retourne un entier comprit entre debut et fin
int Random::getInt(int debut, int fin) {
	return debut + rand()%(fin-debut+1);
}

// retourne un entier comprit entre 0 et 255
u8 Random::getu8() {
	return rand()|0xFF;
}

// retourne un flottant comprit 0.0f, et 1.0f
f32 Random::getf32() {
	return rand()/(RAND_MAX+1.0f);
}

// retourne un flottant comprit entre debut et fin
f32 Random::getf32(f32 debut, f32 fin) {
	return debut + rand()/(RAND_MAX+1.0f)*(fin-debut);
}

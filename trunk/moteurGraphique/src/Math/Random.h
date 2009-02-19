/*
 * Random.h
 *
 *  Created on: 6 févr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef RANDOM_H_
#define RANDOM_H_

#include "../Type.h"

class Random {
public :
	static void init();

	// retourne un entier comprit entre 0 et RAND_MAX
	static int getInt();

	// retourne un entier comprit entre debut et fin
	static int getInt(int debut, int fin);

	// retourne un entier comprit entre 0 et 255
	static u8 getu8();

	// retourne un flottant comprit entre 0.0f, et 1.0f
	static f32 getf32();

	// retourne un flottant comprit entre debut et fin
	static f32 getf32(f32 debut, f32 fin);
};

#endif /* RANDOM_H_ */

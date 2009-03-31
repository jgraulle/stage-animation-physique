/*
 * Perso.h
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef PERSO_H_
#define PERSO_H_

#include <MoteurGraphique.h>
#include <bvh.h>

class Perso: public Objet3D {
public:

	// constructeur
	Perso(const string & bvhFileName, Material material, Transform transform = Transform());

	// destructeur
	virtual ~Perso();

	// affichage
	virtual void display() const;
	virtual void display(int joinId) const;

	// fonction de mise a jour de l'objet
	virtual void update(f32 elapsed);

private:
	string bvhFileName;
	MOTION * motion;
	MFRAME * frame;
	f32 tempsAnim;
	int numFrame;
	f32 tempsParFrame;
};

#endif /* PERSO_H_ */

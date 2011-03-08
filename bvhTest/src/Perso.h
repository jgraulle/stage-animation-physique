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
	virtual const Vector3 display(int joinId, const Transform & transParent) const;

	// fonction de mise a jour de l'objet
	virtual void update(f32 elapsed);

	// passer a la frame suivante manuellement
	virtual void nextFrame();
	virtual void previousFrame();

	// changer le mode d'affichage
	virtual void changeTransformationRoot() {transformationRoot = !transformationRoot;}
	virtual void changeTranslationChildren() {translationChildren = !translationChildren;}

private:
	// convertion d'angle d'euler de la bvh vers quaternion
	static Quaternion bvhToQuater (f32 r[3], int bindings[3]);

	string bvhFileName;
	MOTION * motion;
	MFRAME * frame;
	int numFrame;
	int nbrFrame;
	f32 tempsCourant;
	f32 vitesse;
	f32 tempsParFrame;
	bool transformationRoot;
	bool translationChildren;
};

#endif /* PERSO_H_ */

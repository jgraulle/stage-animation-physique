/*
 * MotionSave.h
 *
 *  Created on: 7 mai 2009
 *      Author: jeremie
 */

#ifndef MOTIONSAVE_H_
#define MOTIONSAVE_H_

#include <list>
#include <MoteurGraphique.h>
#include "RagDoll.h"

using namespace std;

class MotionSave : public Objet3D {
public:
	// constructeur
	MotionSave(RagDoll * ragDoll, f32 timeBetweenSave);

	// destructeur
	virtual ~MotionSave();

	// verifie s'il y a besoin de sauvegarder cette frame et le fait si besoin
	virtual void update(f32 elapsed);

	// affichage
	virtual void display() const;

	// sauvegarde l'animation au format bvh
	virtual void bvhSave(char * bvhFileName);

	struct Point {
		const char * name;
		RagDoll::BodyPart bodyPart;
		SkeletonMesh::Extremite extremite;
	};

	static const int POINTS_COUNT;
	static const int POINT_ROOT;
	static const Point POINTS[];

	// parcours de la hierarchie des membres
	static int bodyPartToPoint(RagDoll::BodyPart part, SkeletonMesh::Extremite extremite);
	static int getFatherOf(int point);
	static const list<int> & getChildrenOf(int point);

private:
	typedef vector<Transform> Frame;
	typedef list<Frame *> Mouvement;
	typedef vector<Transform> PositionEdition;
	typedef pair<Vector3, vector<Quaternion> *> TransformHierarchie;
	typedef list<TransformHierarchie> TransformsHierarchie;

	// parcours de la hierarchie des membres
	static bool initBodyPartToPoint();
	static int _bodyPartToPoint[RagDoll::BODYPART_COUNT][2];
	static bool initPointsHierarchie();
	static list<int> pointsHierarchie[];

	TransformHierarchie computeTransform(Frame*);

	Vector3 displayAbsolueRecurcif(int numPoint) const;
	void displayHierarchieRecurcif(int numPoint, Transform globalFather) const;

	virtual string vector3ToBvh(const Vector3 & v);
	virtual string quaternionToBvh(const Quaternion & q);

	virtual void bvhSaveHierarchy(ostream & os, list<int> & sequencePoints);
	virtual void bvhSaveHierarchyRecurcif(ostream & os, int point, int nbrIndent, const Vector3 posParent, list<int> & sequencePoints);
	virtual void bvhSaveMouvement(ostream & os, list<int> & sequencePoints);

	Mouvement mouvement;			// sauvegarde du mouvement
	PositionEdition positionEdition;// sauvegarde de la position d'edition
	RagDoll * ragDoll;				// ragDoll a sauvegrader
	f32 timeBetweenSave;			// temps entre 2 sauvegardes
	f32 timeSincePreviousSave;		// temps ecoule depuis la precedente sauvegarde

	TransformsHierarchie transformsHierarchie;
};

#endif /* MOTIONSAVE_H_ */

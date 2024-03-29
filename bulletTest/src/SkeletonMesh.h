/*
 * SkeletonMesh.h
 *
 *  Created on: 2 avril 2009
 *      Author: jeremie GRAULLE
 */

#ifndef SKELETON_MESH_H_
#define SKELETON_MESH_H_

#include <MoteurGraphique.h>
#include <bvh.h>

using namespace std;

class SkeletonMesh {
public:

	enum Extremite {DEBUT = 0, FIN = 1};

	struct Os {
		Vector3 debut, fin;
		int joint;
		Os(const Vector3 & debut = Vector3::ZERO, const Vector3 & fin = Vector3::ZERO, int joint = -1) : debut(debut), fin(fin), joint(joint) {}
	};

	// constructeur
	// orientationEdition est utilise afin de faire une rotation du squelette en position d'�dition.
	// cela permet d'oriente le squelette toujours dans la meme direction
	SkeletonMesh(const string & bvhFileName, Quaternion orientationEdition, f32 scale, bool transformationRoot = true, bool translationChildren = true);

	// destructeur
	virtual ~SkeletonMesh();

	virtual const string & getBvhFileName() const {return bvhFileName;}
	virtual f32 getTempsParFrame() const {return tempsParFrame;}
	virtual f32 getDureeTotalAnimation() const {return dureeTotalAnimation;}
	virtual int getNbrTotalFrames() const {return nbrTotalFrames;}
	virtual int getNbrJoints() const {return nbrJoints;}

	// acceder a la position et la la transformation locale d'une articulation
	virtual bool isOsPosition(int numFrame, int joinId) const;
	virtual const Os * getOs(int numFrame, int joinId) const;
	virtual const Vector3 & getOsPosition(int numFrame, int joinId, Extremite extremite) const;
	virtual const Transform & getJointsTransf(int numFrame, int joinId) const;

	// acceder a la position d'un os dans la position d'edition
	virtual const Vector3 & getOsPosEdition(int joinId, Extremite extremite) const;
	virtual bool isOsPosEdition(int joinId) const {return osPosEdition[joinId]!=NULL;}

	// acceder au nom d'une articulation a partir de sont id
	virtual const string & getJointName(int joinId) const;

	// acceder a l'identifiant d'une articulation a partir de son nom (attention au doublon seul le permier est enregistre)
	virtual int getJointIndex(const string & name) const;

private:
	// calculer la position d'edition des os : parcours recursivif du skelete
	const Vector3 calculOsPosEdition(MFRAME * frame, int joinId, const Vector3 & accumulateur);

	// calculer la position de chaque os dans la frame courante : parcours recursivif du skelete
	const Vector3 calculJoinPos(int numFrame, MFRAME * frame, int joinId, const Transform &);

	// fonction de hash pour les string
	struct HashString {size_t operator()(const std::string& x) const {return hash<const char*>()(x.c_str());}};

	// convertion d'angle d'euler de la bvh vers quaternion
	static Quaternion bvhToQuater (f32 r[3], int bindings[3]);

	string bvhFileName;
	f32 tempsParFrame;
	f32 dureeTotalAnimation;
	int nbrTotalFrames;
	int nbrJoints;
	bool transformationRoot;
	bool translationChildren;

	vector<vector<Os*> > osPos;
	vector<vector<Transform> > jointsTransf;
	vector<Os*> osPosEdition;
	typedef hash_map<string, int, HashString> NameIndex;
	NameIndex jointsName;
	vector<string> jointsNameById;

	int rootId;
	Quaternion orientationEdition;
	f32 scale;
};

#endif /* SKELETON_MESH_H_ */

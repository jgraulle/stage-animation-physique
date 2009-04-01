/*
 * Perso.h
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef SKELETON_H_
#define SKELETON_H_

#include <MoteurGraphique.h>
#include <bvh.h>

using namespace std;

class Skeleton: public Objet3D {
public:

	struct Os {
		const Vector3 debut, fin;
		int joint;
		Os(const Vector3 & debut, const Vector3 & fin, int joint) : debut(debut), fin(fin), joint(joint) {}
	};

	// constructeur
	Skeleton(const string & bvhFileName, Material material, Transform transform, Quaternion orientationEdition, f32 scale);

	// destructeur
	virtual ~Skeleton();

	// affichage
	virtual void display() const;

	// fonction de mise a jour de l'objet
	virtual void update(f32 elapsed);

	virtual const Vector3 & getJointPosition(int joinId) const;
	virtual const char * getJointName(int joinId) const;
	virtual int getJointIndex(const string & name) const;

private:
	void jointPosEdition(int joinId, const Vector3 & accumulateur);
	void updateJoin(int joinId, const Transform &);

	// fonction de hash pour les string
	struct HashString {size_t operator()(const std::string& x) const {return hash<const char*>()(x.c_str());}};

	string bvhFileName;
	MOTION * motion;
	MFRAME * frame;
	f32 tempsAnim;
	int numFrame;
	f32 tempsParFrame;
	vector<Vector3> jointsPos;
	vector<Os*> jointsPosEdition;
	typedef hash_map<string, int, HashString> NameIndex;
	NameIndex jointsName;
	int nbrJoints;
	int rootId;
	Quaternion orientationEdition;
	f32 scale;
};

#endif /* SKELETON_H_ */

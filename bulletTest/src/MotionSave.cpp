/*
 * MotionSave.cpp
 *
 *  Created on: 7 mai 2009
 *      Author: jeremie
 */

#include "MotionSave.h"

#include <iostream>
#include <fstream>

using namespace std;

const int MotionSave::POINT_ROOT = 0;
const MotionSave::Point MotionSave::POINTS[] = {
	{"Pelvis", RagDoll::BODYPART_PELVIS, SkeletonMesh::DEBUT},
	{"Spine", RagDoll::BODYPART_SPINE, SkeletonMesh::DEBUT},
	{"Head", RagDoll::BODYPART_HEAD, SkeletonMesh::DEBUT},
	{"HeadEnd", RagDoll::BODYPART_HEAD, SkeletonMesh::FIN},

	{"LeftShoulder", RagDoll::BODYPART_LEFT_UPPER_ARM, SkeletonMesh::DEBUT},
	{"LeftElbow", RagDoll::BODYPART_LEFT_LOWER_ARM, SkeletonMesh::DEBUT},
	{"LeftHand", RagDoll::BODYPART_LEFT_LOWER_ARM, SkeletonMesh::FIN},
	{"RightShoulder", RagDoll::BODYPART_RIGHT_UPPER_ARM, SkeletonMesh::DEBUT},
	{"RightElbow", RagDoll::BODYPART_RIGHT_LOWER_ARM, SkeletonMesh::DEBUT},
	{"RightHand", RagDoll::BODYPART_RIGHT_LOWER_ARM, SkeletonMesh::FIN},

	{"LeftHip", RagDoll::BODYPART_LEFT_UPPER_LEG, SkeletonMesh::DEBUT},
	{"LeftKnee", RagDoll::BODYPART_LEFT_LOWER_LEG, SkeletonMesh::DEBUT},
	{"LeftFoot", RagDoll::BODYPART_LEFT_LOWER_LEG, SkeletonMesh::FIN},
	{"RightHip", RagDoll::BODYPART_RIGHT_UPPER_LEG, SkeletonMesh::DEBUT},
	{"RightKnee", RagDoll::BODYPART_RIGHT_LOWER_LEG, SkeletonMesh::DEBUT},
	{"LeftFoot", RagDoll::BODYPART_RIGHT_LOWER_LEG, SkeletonMesh::FIN},
};
const int MotionSave::POINTS_COUNT = sizeof(MotionSave::POINTS)/sizeof(Point);

int MotionSave::_bodyPartToPoint[RagDoll::BODYPART_COUNT][2];
bool MotionSave::initBodyPartToPoint() {
	// initialise chaque part
	for (int part = 0; part < RagDoll::BODYPART_COUNT; part++) {
		_bodyPartToPoint[part][SkeletonMesh::FIN] = -1;
		_bodyPartToPoint[part][SkeletonMesh::DEBUT] = -1;
	}
	// pour chaque point
	for (int point=0; point<POINTS_COUNT; point++) {
		// l'ajouter dans la liste de son pere
		_bodyPartToPoint[POINTS[point].bodyPart][POINTS[point].extremite] = point;
	}
/*	cout << "MotionSave::initBodyPartToPoint" << endl;
	for (int part = 0; part < RagDoll::BODYPART_COUNT; part++) {
		cout << "part=" << part << " pointDebut=" << _bodyPartToPoint[part][SkeletonMesh::DEBUT] << " pointFin="<< _bodyPartToPoint[part][SkeletonMesh::FIN] << endl;
	}
	cout << "oki" << endl;
*/	return true;
}
int MotionSave::bodyPartToPoint(RagDoll::BodyPart part, SkeletonMesh::Extremite extremite) {
	static bool init = initBodyPartToPoint();
	return _bodyPartToPoint[part][extremite];
}

int MotionSave::getFatherOf(int point) {
	// acceder au membre auquel appartient ce point
	RagDoll::BodyPart part = POINTS[point].bodyPart;
	// si c'est le point est l'extremite fin
	if (POINTS[point].extremite==SkeletonMesh::FIN)
		// alors son pere est l'extremite debut
		return bodyPartToPoint(part, SkeletonMesh::DEBUT);
	// si non
	// else {
		// alors son pere est le point attache au membre pere
		// acceder au joint pere
		RagDoll::Joint jointPere = RagDoll::bodyPartToJointPere(part);
		// acceder au membre pere
		RagDoll::BodyPart partPere = RagDoll::jointToBodyPart(jointPere, RagDoll::PERE);
		// acceder au point pere
		return bodyPartToPoint(partPere, SkeletonMesh::DEBUT);
	//}
}

list<int> MotionSave::pointsHierarchie[MotionSave::POINTS_COUNT];
bool MotionSave::initPointsHierarchie() {
	// pour chaque point sauf pour le root
	for (int numPoint=1; numPoint<POINTS_COUNT; numPoint++) {
		// l'ajouter dans la liste de son pere
		pointsHierarchie[getFatherOf(numPoint)].push_back(numPoint);
	}
/*	cout << "MotionSave::initPointsHierarchie" << endl;
	for (int point=0; point<POINTS_COUNT; point++) {
		cout << "point=" << point << " children=";
		for (list<int>::const_iterator it = pointsHierarchie[point].begin(); it!=pointsHierarchie[point].end(); it++)
			cout << *it << ", ";
		cout << endl;
	}
	cout << "oki" << endl;
*/	return true;
}
const list<int> & MotionSave::getChildrenOf(int point) {
	static bool init = initPointsHierarchie();
	return pointsHierarchie[point];
}

string MotionSave::vector3ToBvh(const Vector3 & v) {
	ostringstream os;
	os << fixed << v.x << " " << v.y << " " << v.z;
	return os.str();
}
string MotionSave::quaternionToBvh(const Quaternion & q) {
	Matrix3 m;
	q.ToRotationMatrix(m);
	float x, y, z;
	m.ToEulerAnglesZXY(z, x, y);
	ostringstream os;
	os << fixed << z * 180.0f / M_PI << " " << x * 180.0f / M_PI << " " << y * 180.0f / M_PI;
	return os.str();
}

MotionSave::MotionSave(RagDoll * ragDoll, f32 timeBetweenSave)
: Objet3D(Material(NULL), NULL, ragDoll->getTransform()), ragDoll(ragDoll), timeBetweenSave(timeBetweenSave), timeSincePreviousSave(0.0f) {
	// sauvegarder la position d'edition
	positionEdition.resize(POINTS_COUNT);
	for (int point=0; point<POINTS_COUNT; point++) {
		positionEdition[point] = ragDoll->getLocalTransformPart(POINTS[point].bodyPart, POINTS[point].extremite);
	}
}

MotionSave::~MotionSave() {
	// liberer la memoire occupe par le mouvement
	for(	Mouvement::iterator it = mouvement.begin() ;
			it != mouvement.end() ;
			it++) {
		delete (*it);
		*it=NULL;
	}
	// liberer la memoire occupe par la TransformsHierarchie
	for(	TransformsHierarchie::iterator it = transformsHierarchie.begin() ;
			it != transformsHierarchie.end() ;
			it++) {
		delete (it->second);
		it->second=NULL;
	}
}

// verifie s'il y a besoin de sauvegarder cette frame et le fait si besoin
void MotionSave::update(f32 elapsed) {
	timeSincePreviousSave += elapsed;
	if (timeSincePreviousSave>timeBetweenSave) {
		// creer une nouvelle frame vide
		Frame * frame = new Frame(POINTS_COUNT);
		// remplir la frame des transformation de chaque point
		for (int numPoint=0; numPoint<POINTS_COUNT; numPoint++) {
			(*frame)[numPoint] = ragDoll->getLocalTransformPart(POINTS[numPoint].bodyPart, POINTS[numPoint].extremite);
		}
		// ajouter la frame au sauvegrade
		mouvement.push_back(frame);
		timeSincePreviousSave = 0.0f;
		// calculer les transform au format hierarchique et la sauvegarder
		transformsHierarchie.push_back(computeTransform(frame));
	}
}

// sauvegarde l'animation au format bvh
// le squelette est recopier depuis une autre bvh
void MotionSave::bvhSave(char * bvhFileName) {
	ofstream bvhFile(bvhFileName);
	list<int> sequencePoints;
	bvhSaveHierarchy(bvhFile, sequencePoints);
	bvhSaveMouvement(bvhFile, sequencePoints);
	bvhFile.close();
}

void MotionSave::bvhSaveHierarchy(ostream & os, list<int> & sequencePoints) {
	os << "HIERARCHY" << endl;
	// ecriture du root
	os << "ROOT " << POINTS[POINT_ROOT].name << endl;
	os << "{" << endl;
	os << "\t" << "OFFSET " << fixed << positionEdition[POINT_ROOT].getPosition().x << " " << positionEdition[POINT_ROOT].getPosition().y << " " << positionEdition[POINT_ROOT].getPosition().z << endl;
	os << "\t" << "CHANNELS 6 Xposition Yposition Zposition Zrotation Xrotation Yrotation" << endl;
	sequencePoints.push_back(POINT_ROOT);
	// pour chaque fils du root
	for (list<int>::const_iterator it = getChildrenOf(POINT_ROOT).begin(); it != getChildrenOf(POINT_ROOT).end(); it++) {
		// ajouter la hierarchy de ce fils
		bvhSaveHierarchyRecurcif(os, *it, 1, positionEdition[POINT_ROOT].getPosition(), sequencePoints);
	}
	os << "}" << endl;
}
void MotionSave::bvhSaveHierarchyRecurcif(ostream & os, int point, int nbrIndent, const Vector3 posParent, list<int> & sequencePoints) {
	ostringstream temp;
	for (int i=0; i<nbrIndent; i++)
		temp << "\t";
	string ident = temp.str();

	Vector3 position = positionEdition[point].getPosition() - posParent;

	// si le point est une feuille dans la hierarchie des points
	if (getChildrenOf(point).size()==0) {
		// ajouter l'element en tant que "End Site"
		os << ident << "End Site " << endl;
		os << ident << "{" << endl;
		os << ident << "\tOFFSET " << vector3ToBvh(position) << endl;
		os << ident << "}" << endl;
	} else {
		// ajouter l'element en tant que "JOINT"
		os << ident << "JOINT " << POINTS[point].name << endl;
		os << ident << "{" << endl;
		os << ident << "\tOFFSET " << vector3ToBvh(position) << endl;
		os << ident << "\tCHANNELS 3 Zrotation Xrotation Yrotation" << endl;
		sequencePoints.push_back(point);

		// pour chaque fils
		for (list<int>::const_iterator it = getChildrenOf(point).begin(); it != getChildrenOf(point).end(); it++) {
			// ajouter la hierarchy de ce fils
			bvhSaveHierarchyRecurcif(os, *it, nbrIndent+1, positionEdition[point].getPosition(), sequencePoints);
		}
		os << ident << "}" << endl;
	}
}

void MotionSave::bvhSaveMouvement(ostream & os, list<int> & sequencePoints) {
	os << "MOTION" << endl;
	os << "Frames: " << mouvement.size() << endl;
	os << "Frame Time: " << fixed << timeBetweenSave << endl;
	// calculer le changement de repere pour passe de chaque fils a leurs pere dans la position d'edition
	Transform pereToFilsEdit[POINTS_COUNT];
	for (int point=1; point<POINTS_COUNT; point++) {
		pereToFilsEdit[point] = Transform(positionEdition[getFatherOf(point)].getPosition() - positionEdition[point].getPosition());
	}
	// pour chaque image
	for (TransformsHierarchie::const_iterator it = transformsHierarchie.begin(); it!=transformsHierarchie.end(); it++) {
		// enregistrer la position du root
		os << vector3ToBvh(it->first);
		// pour chaque point
		for (list<int>::const_iterator numPoint=sequencePoints.begin(); numPoint!=sequencePoints.end(); numPoint++) {
			// si le point n'est pas une extremite
			if (getChildrenOf(*numPoint).size()!=0) {
				// enregistrer sa rotation
				Quaternion q = (*it->second)[*numPoint];
				os << " " << quaternionToBvh(q);
			}
		}
		os << endl;
	}
}

MotionSave::TransformHierarchie MotionSave::computeTransform(Frame *  frame) {
	vector<Quaternion> * orient = new vector<Quaternion>(POINTS_COUNT);
	// enregistrer la transform du root
	Vector3 vRoot = (*frame)[POINT_ROOT].getPosition();
	Quaternion qRoot = (*frame)[POINT_ROOT].getOrientation() * positionEdition[POINT_ROOT].getOrientation().Inverse();
	(*orient)[POINT_ROOT] = qRoot;
	// pour chaque point
	for (int numPoint=1; numPoint<POINTS_COUNT; numPoint++) {
		// si le point n'est pas une extremite
		if (getChildrenOf(numPoint).size()!=0) {
			// enregistrer sa rotation
			Transform tmpFather = positionEdition[getFatherOf(numPoint)] * (*frame)[getFatherOf(numPoint)].inverse();
			Transform tmpChild = positionEdition[numPoint] * (*frame)[numPoint].inverse();
			Transform localChild = tmpFather * tmpChild.inverse();
			(*orient)[numPoint] = localChild.getOrientation();
		}
	}
	return make_pair(vRoot, orient);
}

void MotionSave::display(void) const {
/*	if (transformsHierarchie.size()!=0) {
		Disable lumiere(GL_LIGHTING);
		Disable texture(GL_TEXTURE_2D);
		Disable profondeur(GL_DEPTH_TEST);
		glBegin(GL_LINES);
		// afficher les positions enregistrees
		displayAbsolueRecurcif(POINT_ROOT);
		// afficher la hierarchie
		Transform globalRoot = Transform(transformsHierarchie.back().first, (*transformsHierarchie.back().second)[POINT_ROOT]);
		displayHierarchieRecurcif(POINT_ROOT, globalRoot);
		glEnd();
		glColor4f(1.0f, 1.0f, 1.0f, 1.f);
	}
*/
}

Vector3 MotionSave::displayAbsolueRecurcif(int numPoint) const {
	// calculer la position du point courant
	Vector3 position = (*mouvement.back())[numPoint].getPosition();
	// s'il a des fils
	if (getChildrenOf(numPoint).size()!=0) {
		// calculer la moyenne des positions de ces enfants en les affichants
		Vector3 moyenne = Vector3::ZERO;
		for (list<int>::const_iterator it=getChildrenOf(numPoint).begin(); it!=getChildrenOf(numPoint).end(); it++) {
			moyenne += displayAbsolueRecurcif(*it);
		}
		moyenne /= getChildrenOf(numPoint).size();
		// afficher une ligne entre le pere et la moyenne de ces fils
		glColor4f(0.5f, 0.5f, 0.5f, 1.0f);
		glVertex3fv(position);
		glVertex3fv(moyenne);
	}
	return position;
}

void MotionSave::displayHierarchieRecurcif(int numPoint, Transform globalFather) const {
	// s'il a des fils
	if (getChildrenOf(numPoint).size()!=0) {
		// calculer la moyenne des positions de ces enfants en les affichants
		Vector3 moyenne = Vector3::ZERO;
		for (list<int>::const_iterator it=getChildrenOf(numPoint).begin(); it!=getChildrenOf(numPoint).end(); it++) {
			Vector3 pereToFilsEdit = positionEdition[*it].getPosition() - positionEdition[numPoint].getPosition();
			Transform localChild = Transform(pereToFilsEdit) * Transform((*transformsHierarchie.back().second)[*it]);
			Transform globalChild = globalFather * localChild;
			moyenne += globalChild.getPosition();
			displayHierarchieRecurcif(*it, globalChild);
		}
		moyenne /= getChildrenOf(numPoint).size();
		if (moyenne!=Vector3::ZERO) {
			glColor4f(1.0f, 1.0f, 1.0f, 1.0f);
			glVertex3fv(globalFather.getPosition());
			glVertex3fv(moyenne);
		}
	}
	// afficher un repere correspondant au point courant
	Transform t = globalFather;
	glColor4f(1.0f, 0.0f, 0.0f, 0.5f);
	glVertex3fv(t.getPosition());
	glVertex3fv(t * Vector3::UNIT_X);
	glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
	glVertex3fv(t.getPosition());
	glVertex3fv(t * Vector3::UNIT_Y);
	glColor4f(0.0f, 0.0f, 1.0f, 0.5f);
	glVertex3fv(t.getPosition());
	glVertex3fv(t * Vector3::UNIT_Z);
}

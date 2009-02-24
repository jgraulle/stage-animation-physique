#ifndef _MOCAPMANIPULATOR_H_
#define _MOCAPMANIPULATOR_H_

#include <list>
#include "MocapData.h"
#include "SkeletalData.h"
#include "Joint.h"
#include "Point3D.h"
#include "Mat4f.h"

class MocapManipulator {

    private:
        // ATTRIBUTS
        MocapData * mocapData;
        SkeletalData * skeletalData;

  	    // Renvoie la position d'un joint par son nom à partir d'un joint de reference
	    void getMatTransformation(int numeroFrame, Joint::JointId jointId, bool getEndSite, Mat4f &mat);
	    // Applique les transformations géométriques relatives à un joint
	    void appliquerTransformation(int numeroFrame, Joint::JointId jointId, bool getEndSite);
	    void appliquerTransformationInverse(int numeroFrame, Joint::JointId jointId);

        // Renvoie la chaine des joints d'un joint vers la racine
        void getChaineFromRootToJoint(Joint::JointId id, Joint::ListJointId &resultat);

    public:
        // CONSTRUCTEUR
        MocapManipulator(MocapData * mocapData, SkeletalData * skeletalData);
        ~MocapManipulator();

        // METHODES

	    // Renvoie la position d'un Joint par son nom dans une frame
	    Point3D * getPositionJointByNameInFrame(int numeroFrame, string nomJoint, bool getEndSite = false, float*matRot = NULL, bool withtoutScale = false);

	    // Renvoie la matrice de transformation d'un Joint
        void getJointMatriceTransformation(int numeroFrame, Joint::JointId jointId, Mat4f &mat, bool getEndSite = false, bool withtoutScale = false);

	    // Renvoie la matrice de transformation inverse d'un Joint
	    void getInverseMatTransformation(int numeroFrame, Joint::JointId jointId, Mat4f &mat);


};

#endif

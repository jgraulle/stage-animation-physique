#include "MocapManipulator.h"
#include <list>
#include <GL/gl.h>

MocapManipulator::MocapManipulator(MocapData * mocapData, SkeletalData * skeletalData) {
    this->mocapData = mocapData;
    this->skeletalData = skeletalData;

}

MocapManipulator::~MocapManipulator() {
    
}

void MocapManipulator::getChaineFromRootToJoint(Joint::JointId id, Joint::ListJointId &resultat) {
    // Le résultat
    //Joint::ListJointId resultat;
    
    // On recupere le joint de départ
    Joint * currentJoint = skeletalData->getJointById(id);
    
    // On remonte jusqu'à la racine
    while (currentJoint != NULL) {
        resultat.push_front(currentJoint->getId());
        currentJoint = currentJoint->getParentJoint();
    }
    
    // On renvoie le résultat
    //return resultat;
}
    
//////////////////////////////////////////////////
// PROC:   getPositionTerminauxAnimation()
// DOES:   renvoie la position d'un joint par son nom dans une frame (on part de la racine)
//		   EndSite indique si on doit renvoyer la position du terminal (EndSite section)
//		   ou seulement la position du joint.
//////////////////////////////////////////////////
Point3D * MocapManipulator::getPositionJointByNameInFrame(int numeroFrame, string  nomJoint, bool getEndSite, float *matRot, bool withtoutScale) {
    // Calcul de la matrice de transformation
    Mat4f mat;
    getJointMatriceTransformation(numeroFrame, skeletalData->getJointByName(nomJoint)->getId(), mat, getEndSite, withtoutScale); 
    
    // Copie de la matrice de transformation si necessaire
    if (matRot != NULL) {
        memcpy(matRot, &mat, 16*sizeof(float));   
    }
    
    // Calcul de la position
    Point3D * resultat = new Point3D(0,0,0);
    *resultat = *resultat * mat;
    return resultat;
}

void MocapManipulator::getJointMatriceTransformation(int numeroFrame, Joint::JointId jointId, Mat4f &mat, bool getEndSite, bool withtoutScale) {
	glPushMatrix();
	glLoadIdentity();

	if (!withtoutScale)
		glScalef(mocapData->getEchelle(),mocapData->getEchelle(),mocapData->getEchelle());

	// Petite correction, car en général nos mouvement sont alignés sur l'axe des X
	// Alors que le plus cohérent est sur l'axe des Y
	glRotatef(90, 0.0, 1.0, 0.0);


	getMatTransformation(numeroFrame, jointId, getEndSite, mat);
	glPopMatrix();
   
}




//////////////////////////////////////////////////
// PROC:   appliquerTransformation()
// DOES:   applique les transformations relatives à un Joint
//         (rotation et translation)
//////////////////////////////////////////////////
void MocapManipulator::appliquerTransformation(int numeroFrame, Joint::JointId jointId, bool getEndSite) {
    
    // Recupere le joint
    Joint * joint = skeletalData->getJointById(jointId);

    // Offset du joint
	glTranslatef(joint->getOffsetPos('X'),joint->getOffsetPos('Y'),joint->getOffsetPos('Z'));

	// Appliquer translation
	if (joint->isTransJoint()) {
		float x = mocapData->getTrans(numeroFrame, 'X', joint);
		float y = mocapData->getTrans(numeroFrame, 'Y', joint);
		float z = mocapData->getTrans(numeroFrame, 'Z', joint);
		glTranslatef(x,y,z);
		//nc += 3;
	}

    // Appliquer rotation
    float aZ = mocapData->getRot(numeroFrame, 'Z', joint);
	glRotatef(aZ,0,0,1);
    float aX = mocapData->getRot(numeroFrame, 'X', joint);
	glRotatef(aX,1,0,0);
    float aY = mocapData->getRot(numeroFrame, 'Y', joint);
	glRotatef(aY,0,1,0);

	// Cas particulier pour un terminal
	if (joint->isTerminal() && getEndSite) {
    	// On translate sur la fin réelle du terminal
		glTranslatef(joint->getOffsetEndPos('X'),
					 joint->getOffsetEndPos('Y'),
					 joint->getOffsetEndPos('Z'));
	}  

}

void MocapManipulator::appliquerTransformationInverse(int numeroFrame, Joint::JointId jointId) {

    // Recupere le joint
    Joint * joint = skeletalData->getJointById(jointId);
    
    // Appliquer rotation
    float aY = mocapData->getRot(numeroFrame, 'Y', joint);
	glRotatef(-aY,0,1,0);
    float aX = mocapData->getRot(numeroFrame, 'X', joint);
	glRotatef(-aX,1,0,0);
    float aZ = mocapData->getRot(numeroFrame, 'Z', joint);
	glRotatef(-aZ,0,0,1);


	// Appliquer translation
	if (joint->isTransJoint()) {
		float x = mocapData->getTrans(numeroFrame, 'X', joint);
		float y = mocapData->getTrans(numeroFrame, 'Y', joint);
		float z = mocapData->getTrans(numeroFrame, 'Z', joint);
		glTranslatef(-x,-y,-z);
	}

    // Offset du joint
	glTranslatef(-joint->getOffsetPos('X'),-joint->getOffsetPos('Y'),-joint->getOffsetPos('Z'));

}

//////////////////////////////////////////////////
// PROC:   getInverseMatTransformation()
// DOES:   Renvoie la matrice de transformation inverse d'un Joint
//////////////////////////////////////////////////
void MocapManipulator::getInverseMatTransformation(int numeroFrame, Joint::JointId jointId, Mat4f &mat) {
	
    // On recupere la chaine du joint vers la racine pour pouvoir appliquer l'ensemble des transformations successives
    Joint::ListJointId listeJointId;// = getChaineFromRootToJoint(jointId);
    getChaineFromRootToJoint(jointId, listeJointId);
    
    if (listeJointId.empty()) return;
    
    // Inversion de l'ordre de la liste (normal, on applique les transformations en sens inverse)
    listeJointId.reverse();
    
    glPushMatrix();  
    glLoadIdentity();

    // Parcours de la chaine  	
    for (Joint::ListJointId::iterator it = listeJointId.begin(); it!=listeJointId.end();it++) {

        // Applique les transformation inverse du joint courant
  	     appliquerTransformationInverse(numeroFrame, *it);        
    } 
    	
    // Finalement on calcule la position/matrice de transformation
    glGetFloatv(GL_MODELVIEW_MATRIX, &mat);
    
    glPopMatrix();
}

//////////////////////////////////////////////////
// PROC:   getPositionJointByName()
// DOES:   renvoie la position d'un joint par son nom à partir d'un joint de reference
//////////////////////////////////////////////////
void MocapManipulator::getMatTransformation(int numeroFrame, Joint::JointId jointId, bool getEndSite, Mat4f &mat)
{
    // On recupere la chaine du joint vers la racine pour pouvoir appliquer l'ensemble des transformations successives
    Joint::ListJointId listeJointId;// = getChaineFromRootToJoint(jointId);
    getChaineFromRootToJoint(jointId, listeJointId);
    if (listeJointId.empty()) return;

    glPushMatrix();
    for (Joint::ListJointId::iterator it = listeJointId.begin(); it!=listeJointId.end(); it++) {
    
        // On effectue la transformation
        appliquerTransformation(numeroFrame, *it, getEndSite); 
                    
    }     
    
    // Finalement on calcule la position/matrice de transformation
    glGetFloatv(GL_MODELVIEW_MATRIX, &mat);
    
    glPopMatrix();   
    
    
}

/*
Point3D * MocapManipulator::getPositionJointByName(int numeroFrame, string nomJoint, Joint * referenceJoint, bool getEndSite, float *matRot) {

	glPushMatrix();

	// Le resultat
	Point3D * resultat = NULL;

	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();
    
	// On applique les transformation du noeud
	//referenceJoint->appliquerTransformation(frameData);
    appliquerTransformation(numeroFrame, referenceJoint);

	// Si le nom du joint correspond, on arrete la recherche
	if (referenceJoint->getName() == nomJoint) {
		
		// Cas particulier pour un terminal
		if (referenceJoint->isTerminal() && getEndSite) {
			// On translate sur la fin réelle du terminal
			glTranslatef(referenceJoint->getOffsetEndPos('X'),
						 referenceJoint->getOffsetEndPos('Y'),
						 referenceJoint->getOffsetEndPos('Z'));
		}


	    
	    if (matRot!=NULL) {
            glGetFloatv(GL_MODELVIEW_MATRIX, matRot); 
            resultat = new Point3D(0,0,0);
            resultat->multVecteurMatrice(matRot);
        } else {
            Mat4f mat;
            glGetFloatv(GL_MODELVIEW_MATRIX, &mat);
            resultat = new Point3D(0,0,0);
            *resultat = *resultat * mat;            
        }
	    
	} else {

		// Sinon c'est un noeud
		// on parcours ses enfants pour recommencer le processus
		list<Joint*>::iterator it = listeCourante->begin();
		while (it != listeCourante->end()) {
			Joint * unenfant = *(it++);
			Point3D * result = getPositionJointByName(numeroFrame, nomJoint, unenfant, getEndSite, matRot);
			if (result) {
				resultat = result;
				break;
			}
		}
	}

	glPopMatrix();


	// Renvoie le resultat
	return resultat;


}
*/

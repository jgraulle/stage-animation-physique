#include "SkeletalData.h"

SkeletalData::SkeletalData(Joint * racine) {
    //On enregistre la racine
    root = racine;

    // Création de la liste des joints indexé par leur nom
    majListeJoints(root);
}

SkeletalData::SkeletalData(const SkeletalData &copie) {
    this->root = new Joint(*copie.root);
    //this->listeJointsByName = copie.listeJointsByName;
    //this->listeJointsById = copie.listeJointsById;
    majListeJoints(root); // Au lieu de recopier la liste des Joint par leur id, on la regénère 
                          // car les Joints peuvent/(doivent?) changer d'id
}

SkeletalData::~SkeletalData() {
    delete root; 
    
}

SkeletalData::JointListIndexedByName SkeletalData::getListeJointsByName() {
    return listeJointsByName;
}

SkeletalData::JointListIndexedById SkeletalData::getListeJointsById() {
    return listeJointsById;
}

void SkeletalData::majListeJoints(Joint * referenceJoint) {

    // Ajoute le joint de référence dans la liste
    listeJointsByName[referenceJoint->getName()] = referenceJoint;
    listeJointsById[referenceJoint->getId()] = referenceJoint;


	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();
	// on parcours ses enfants pour recommencer le processus	
	for (list<Joint*>::iterator it = listeCourante->begin(); it != listeCourante->end(); it++) {
		// Recupération de l'enfant
		Joint * unEnfant = *it;
		
		// Appel récursif
		majListeJoints(unEnfant);
	}
    
}

Joint * SkeletalData::getJointByName(string nomJoint) {

    // On renvoie le joint en le recherchant par son nom dans la liste des joints
    return listeJointsByName[nomJoint];
    
}

Joint * SkeletalData::getJointById(int idJoint) {
    
    return listeJointsById[idJoint];
}

Joint * SkeletalData::getRootSqueleton() {
    return root;   
}

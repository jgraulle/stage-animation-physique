#include "MocapData.h"

#include <stdlib.h>

MocapData::MocapData(int nframes, int nchannels, float dureeFrame) {

    // Stocke le nombre de frame et de channels pour l'animation enti�re
    this->nframes = nframes;
    this->nchannels = nchannels;
    this->dureeFrame = dureeFrame;
    this->drawScale = 1;

    // Valeur par defaut
    freeIndex = 0; // On commence au d�but du tableau



    // Utilise les donn�es pass�es en param�tres ou alors
    // Alloue un espace m�moire suffisant pour stocker l'ensemble des param�tres
    this->data = (float *) malloc(nframes*nchannels*sizeof(float));
    //if (data) {
    //    memcpy(this->data, data, nframes*nchannels*sizeof(float));
    //}
    //this->data = data;
}

MocapData::MocapData(const MocapData &copie) {

    //MocapData(copie.nframes, copie.nchannels, copie.dureeFrame, copie.data);

    this->nframes = copie.nframes;
    this->nchannels = copie.nchannels;
    this->dureeFrame = copie.dureeFrame;
    this->freeIndex = copie.freeIndex;
    this->drawScale = copie.drawScale;

    this->listeIndex = copie.listeIndex;

    // Utilise les donn�es pass�es en param�tres ou alors
    // Alloue un espace m�moire suffisant pour stocker l'ensemble des param�tres
    this->data = (float *) malloc(copie.nframes*copie.nchannels*sizeof(float));
    memcpy(this->data, copie.data, copie.nframes*copie.nchannels*sizeof(float));
}

MocapData::~MocapData() {
    if (data)
        free(data);
}

int MocapData::getOffsetRotChannel(Joint * joint, char axe) {
   int debut=0;
   if (joint->isTransJoint()) {
		debut = 3;
   }
   switch (axe) {
        case 'X': return debut+0;
        case 'Y': return debut+1;
        case 'Z': return debut+2;
        default: return 0; // Aucune verif si l'axe est invalide (rapidit�)
   }
}

int MocapData::getOffsetTransChannel(char axe) {
   switch (axe) {
        case 'X': return 0;
        case 'Y': return 1;
        case 'Z': return 2;
        default: return 0; // Aucune verif si l'axe est invalide (rapidit�)
   }
}


void MocapData::setNewFrameNumber(int nframes) {
    data = (float *) realloc(data, nframes*nchannels*sizeof(float));
    this->nframes = nframes;
}

// D�coupe l'animation
void MocapData::cropData(int numeroFrameDebut, int numeroFrameFin) {
 	// Test de validit�
	if ((numeroFrameDebut >= numeroFrameFin) || (numeroFrameDebut <0) || (numeroFrameFin>=getNbFrames()))
		return;

	// On d�place le pointeur data
	data += numeroFrameDebut*getNChannels();

	// On d�place la fin des donn�es
	nframes = numeroFrameFin-numeroFrameDebut;
}


int MocapData::getNbFrames() {
    return nframes;
}

int MocapData::getNChannels() {
    return nchannels;
}

float MocapData::getDureeFrame() {
    return dureeFrame;
}

void MocapData::setEchelle(float echelle) {
    drawScale = echelle;
}

float MocapData::getEchelle() {
    return drawScale;
}


void MocapData::addJoint(Joint * joint) {
    // Si le joint n'existe pas d�j�
    if (listeIndex.find(joint->getId()) == listeIndex.end()) {
        // On enregistre l'index du joint
        listeIndex[joint->getId()] = freeIndex;


        // On d�cale le prochain index libre de la taille occup�e par le joint
        // C'est � dire le nombre de channels du joints (rotation + eventuelle translation, 3 ou 6 en g�n�ral)
        freeIndex += joint->getNbChannels();
        /*
        if (joint->isTransJoint()) {
            freeIndex+=6;
        } else {
            freeIndex+=3;
        }
        */
    }


}


MocapData::JointData MocapData::getJointData(int numeroFrame, Joint * joint) {

    // Cr�ation du conteneur
    JointData jointData;

    // Translations
    if (joint->isTransJoint()) {
        jointData.transX = getTrans(numeroFrame, 'X', joint);
        jointData.transY = getTrans(numeroFrame, 'Y', joint);
        jointData.transZ = getTrans(numeroFrame, 'Z', joint);
    }

    // Rotations
    jointData.rotX = getRot(numeroFrame, 'X', joint);
    jointData.rotY = getRot(numeroFrame, 'Y', joint);
    jointData.rotZ = getRot(numeroFrame, 'Z', joint);


    return jointData;
}

void MocapData::setJointData(const MocapData::JointData &jointData, int numeroFrame, Joint * joint) {

    // Translations
    if (joint->isTransJoint()) {
        setTransXYZ(jointData.transX, jointData.transY, jointData.transZ, numeroFrame, joint);
    }

    // Rotations
    setRotXYZ(jointData.rotX, jointData.rotY, jointData.rotZ, numeroFrame, joint);

}

float MocapData::getRot(int numeroFrame, char axe, Joint * joint) {
   //return data[numeroFrame*getNChannels() + joint->getOffsetRotChannel(axe)];
   return data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetRotChannel(joint,axe)];
}


float MocapData::getTrans(int numeroFrame, char axe, Joint * joint) {
   return data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetTransChannel(axe)];
}

void MocapData::setRot(float valeur, int numeroFrame, char axe, Joint * joint) {
   data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetRotChannel(joint,axe)] = valeur;
}

void MocapData::setRotXYZ(float valeurX, float valeurY, float valeurZ, int numeroFrame, Joint * joint) {
    setRot(valeurX, numeroFrame, 'X', joint);
    setRot(valeurY, numeroFrame, 'Y', joint);
    setRot(valeurZ, numeroFrame, 'Z', joint);
}

void MocapData::setTrans(float valeur, int numeroFrame, char axe, Joint * joint) {
   data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetTransChannel(axe)] = valeur;
}

void MocapData::setTransXYZ(float valeurX, float valeurY, float valeurZ, int numeroFrame, Joint * joint) {
    setTrans(valeurX, numeroFrame, 'X', joint);
    setTrans(valeurY, numeroFrame, 'Y', joint);
    setTrans(valeurZ, numeroFrame, 'Z', joint);
}

void MocapData::addRot(float valeur, int numeroFrame, char axe, Joint * joint) {
   data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetRotChannel(joint,axe)] += valeur;
}


void MocapData::addTrans(float valeur, int numeroFrame, char axe, Joint * joint) {
   data[numeroFrame*getNChannels() + listeIndex[joint->getId()] + getOffsetTransChannel(axe)] += valeur;
}

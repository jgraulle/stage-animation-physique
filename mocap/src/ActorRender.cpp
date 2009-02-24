#include "ActorRender.h"

ActorRender::ActorRender(MocapData *mocapData, SkeletalData *skeletalData, SkinData *skinData)
{
    this->mocapData = mocapData;
    this->skeletalData = skeletalData;
    this->skinData = skinData;
    
    canOptimizeFlag = true;
    
    // Valeurs par defaut
    //echelleRendu = 1;
}


ActorRender::~ActorRender(){
    
}


void ActorRender::appliquerTransformation(int numeroFrame, Joint * joint) {

    // Offset du joint
	glTranslatef(joint->getOffsetPos('X'),joint->getOffsetPos('Y'),joint->getOffsetPos('Z'));

	// Appliquer translation
	if (joint->isTransJoint()) {
		float x = mocapData->getTrans(numeroFrame, 'X', joint);
		float y = mocapData->getTrans(numeroFrame, 'Y', joint);
		float z = mocapData->getTrans(numeroFrame, 'Z', joint);
		glTranslatef(x,y,z);
	}

    // Appliquer rotation
    float aZ = mocapData->getRot(numeroFrame, 'Z', joint);
	glRotatef(aZ,0,0,1);
    float aX = mocapData->getRot(numeroFrame, 'X', joint);
	glRotatef(aX,1,0,0);
    float aY = mocapData->getRot(numeroFrame, 'Y', joint);
	glRotatef(aY,0,1,0);
}

void ActorRender::setOptimizeFlag(bool canOptimizeFlag) {
    this->canOptimizeFlag = canOptimizeFlag;
}

/*
void ActorRender::setEchelle(float echelleRendu) {
    this->echelleRendu = echelleRendu;
} 
*/

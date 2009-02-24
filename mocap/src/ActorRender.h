#ifndef _ACTORRENDER_H_
#define _ACTORRENDER_H_

#include <list>
#include <math.h>
#include <GL/glut.h>
#include "Point3D.h"
#include "MocapData.h"
#include "SkeletalData.h"
#include "SkinData.h"
#include "Joint.h"

using namespace std;

#define sphereSize 0.7

class Bvh;

class ActorRender {

protected:
    MocapData *mocapData;
    SkeletalData *skeletalData;
    SkinData *skinData;
    //float echelleRendu;


    bool canOptimizeFlag; // Indique si l'actor render peut effectuer des opérations
                      // d'optimisation en considérant que l'animation n'est pas
                      // modifiée au cours du temps

    virtual void drawJoint(int numeroFrame, Joint * joint)  = 0;
    void appliquerTransformation(int numeroFrame, Joint * joint);

public:
    ActorRender(MocapData *mocapData, SkeletalData *skeletalData, SkinData *skinData);
    virtual ~ActorRender();

    virtual  void drawFrame(int numeroFrame) = 0;

    void setOptimizeFlag(bool canOptimizeFlag);
    bool canOptimize() { return canOptimizeFlag; };

    //void setEchelle(float echelleRendu);


};


#endif

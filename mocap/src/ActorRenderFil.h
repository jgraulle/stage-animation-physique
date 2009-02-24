#ifndef _ACTORRENDERFIL_H_
#define _ACTORRENDERFIL_H_


#include "ActorRender.h"

using namespace std;


class Bvh;

class ActorRenderFil: public ActorRender {

private:

    
    void drawJoint(int numeroFrame, Joint * joint);

    
public:
    ActorRenderFil(MocapData *mocapData, SkeletalData *skeletalData, SkinData *skinData);
    ~ActorRenderFil();
    void drawFrame(int numeroFrame);
    
};


#endif

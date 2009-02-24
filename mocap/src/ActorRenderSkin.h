#ifndef _ACTORRENDERSKIN_H_
#define _ACTORRENDERSKIN_H_


#include <map>
#include "ActorRender.h"
#include "Joint.h"
#include "SkinData.h"
#include "MocapManipulator.h"
#include "Point3D.h"
#include "Mat4f.h"

using namespace std;

class Bvh;

class ActorRenderSkin: public ActorRender {

private:
    // ATTRIBUTS

    MocapManipulator * mocapManipulator;


    // Liste des id des textures
    typedef map<const string, GLuint> TextureIdByTextureName;
    TextureIdByTextureName listeTextureId;

    // La liste des matrices de transformation des joints
    //typedef map<const Joint::JointId, Mat4f> MatJointIndexedByJointId;   // On n'utilise plus les map ici car moins rapide
    typedef Mat4f* MatJointIndexedByJointId;
    MatJointIndexedByJointId inverseMatListAtRefPos;
    MatJointIndexedByJointId matListAtFrameN;

    // La liste de l'ensemble des matrices de transformations des joints
    typedef MatJointIndexedByJointId* MatListIndexedByFrame;
    MatListIndexedByFrame matListAtFrame;


    typedef map<const SkinData::Vertex*, Point3D> VertexListCorrectedAtFrameN;
    VertexListCorrectedAtFrameN * vertexListCorrectedAtFrame;

    // La derniere frame rendu (optimisation)
    int lastFrameRendered;


    // METHODES
    void drawJoint(int numeroFrame, Joint * joint);

    void computeInverseMatAtFrameN(int numeroFrame, MocapManipulator * mocapManip, MatJointIndexedByJointId &inverseMatListAtFrameN);
    void computeMatAtFrameN(int numeroFrame, MocapManipulator * mocapManip, MatJointIndexedByJointId &matListAtFrameN);

    void drawSkinnedVertex(const SkinData::Vertex &vertex, const int numeroFrame);

    void loadTextures(TextureIdByTextureName &listeTextureId);
public:
    // CONSTRUCTEURS
    ActorRenderSkin(MocapData *mocapData, SkeletalData * skeletalData, SkinData * skinData);
    ~ActorRenderSkin();


    // METHODES
    void drawFrame(int numeroFrame);


};


#endif

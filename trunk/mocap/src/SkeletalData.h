#ifndef _SKELETALDATA_H_
#define _SKELETALDATA_H_

#include <map>
#include "Joint.h"

class SkeletalData {
    public:
        typedef map<const string, Joint*, less<string> > JointListIndexedByName;
        typedef map<const int, Joint*> JointListIndexedById;
              
        
    private:
        // ATTRIBUTS
        JointListIndexedByName listeJointsByName;     // la liste des joints par leur nom 
        JointListIndexedById listeJointsById;     // la liste des joints par leur id

        Joint* root;
        
        // METHODES
        void majListeJoints(Joint * referenceJoint);    
             
    public:
  
        
        // CONSTRUCTEUR
        SkeletalData(Joint * racine);
        SkeletalData(const SkeletalData &copie);
        ~SkeletalData();
            
            
        // METHODES
        Joint * getRootSqueleton(); 
        Joint * getJointByName(string nomJoint);
        Joint * getJointById(int idJoint);
        JointListIndexedByName getListeJointsByName();
        JointListIndexedById getListeJointsById();
    
};

#endif

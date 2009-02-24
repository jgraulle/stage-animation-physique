#ifndef _MOCAPLOADER_H_
#define _MOCAPLOADER_H_

#include <algorithm>
#include <string>
#include "Parser.h"
#include "MocapData.h"
#include "SkeletalData.h"
#include "SkinData.h"
#include "Joint.h"


class MocapLoader {
protected:
    // ATTRIBUTS
    MocapData * mocapData;
    SkeletalData * skeletalData;
    SkinData * skinData; // NULL si aucune info de skin
    
    // CONSTRUCTEUR
    MocapLoader();

     
public :
    // DESTRUCTEUR
    virtual ~MocapLoader() = 0;   
       
    // METHODES
    // Pour la lecture
    virtual bool load(string nomFichier) = 0; 
    MocapData * getMocapData();
    SkeletalData * getSkeletalData();
    SkinData * getSkinData();
    
    // Pour l'écriture
    virtual bool write(const string nomFichier) = 0;
    void setMocapData(MocapData * mocapData);
    void setSkeletalData(SkeletalData * skeletalData);
    void setSkinData(SkinData * skinData);
    
    static MocapLoader * getInstanceOfAppriopriateLoader(string nomFichier);
};

#endif

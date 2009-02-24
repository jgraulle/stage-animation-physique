#ifndef _SKINLOADER_H_
#define _SKINLOADER_H_

#include <algorithm>
#include <map>
#include <string>
#include <math.h>
#include "MocapLoader.h"


/* 
    CHARGE SEULEMENT LES .SMD
    VOIR http://developer.valvesoftware.com/wiki/SMD_file_format
*/

class SkinLoader : public MocapLoader {
protected:
    // ATTRIBUTS
    
    // La liste des joints par leur ID
    typedef map<const int , Joint* > JointListIndexedByID;
    JointListIndexedByID listeJointsById;
    
    void convertEulerZYXtoZXY(float rotXOld, float rotYOld, float rotZOld, float &rotX, float &rotY, float &rotZ, bool isRacine = false);

      
public :
    // CONSTRUCTEUR
    SkinLoader();
    // DESTRUCTEUR
    ~SkinLoader();   
       
    // METHODES
    // Pour la lecture
    bool load(string nomFichier); 
    bool load(string nomFichierSkin, string nomFichierAnim); 
        
    //MocapData * getMocapData();
    //SkinData * getSkinData();
    //SkeletalData * getSkeletalData();

    // Pour l'écriture
    bool write(const string nomFichier);
    

};

#endif

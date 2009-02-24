#ifndef _MOCAPDATA_H_
#define _MOCAPDATA_H_

#include <string.h>
#include <map>
#include "Joint.h"

class MocapData {
    
public:
    class JointData {
        public:
            // Attributs
            float rotX, rotY, rotZ, transX, transY, transZ;
          
    };
      
private:
    // ATTRIBUTS
    
    // Les données du mouvements
    float *data;  
    // Prochain index libre dans le tableau des données
    int freeIndex;

    // Le nombre de channels	    
    int nchannels;
          
    // Le nombre de frames totales    
    int nframes;
    
    // La durée en seconde d'une frame
    float dureeFrame;
    
    // Echelle de rendu
    float drawScale;  
     
    // La liste des index des joints pour les situer dans la table des données
    typedef map<const Joint::JointId, int> IndexListIndexedByJointId;
    IndexListIndexedByJointId listeIndex;     // la liste des joints par leur id (petite optimisation)
    
    // Methode renvoyant les offset de stockage de Translation et de Rotation
    int getOffsetRotChannel(Joint * joint, char axe);
    int getOffsetTransChannel(char axe);
        	

public:
    // CONSTRUCTEURS
    MocapData(int nframes, int nchannels, float dureeFrame);
    MocapData(const MocapData &copie);
    ~MocapData();
    
    
    // METHODES
    
    void setNewFrameNumber(int nframes); // Augmente l'espace disponible
    
    // Découpe l'animation
    void cropData(int numeroFrameDebut, int numeroFrameFin);
    
    //float getRawData(int numeroFrame, int offset);
    //void setRawData(float valeur, int numeroFrame, int offset);
    
    int getNbFrames();
    int getNChannels();
    float getDureeFrame();
 
	void setEchelle(float echelle);			// Fixe l'echelle de rendu de l'animation
	float getEchelle();						// Renvoie l'echelle de rendu de l'animation
      
    // Réserve un emplacement de taille adéquate pour le joint  
    void addJoint(Joint * joint);
    
    JointData getJointData(int numeroFrame, Joint * joint);
    void setJointData(const JointData &jointData, int numeroFrame, Joint * joint);
    float getRot(int numeroFrame, char axe, Joint * joint);
    float getTrans(int numeroFrame, char axe, Joint * joint);     
    void setRot(float valeur, int numeroFrame, char axe, Joint * joint);
    void setRotXYZ(float valeurX, float valeurY, float valeurZ, int numeroFrame, Joint * joint);
    void setTrans(float valeur, int numeroFrame, char axe, Joint * joint);    
    void setTransXYZ(float valeurX, float valeurY, float valeurZ, int numeroFrame, Joint * joint);
    void addRot(float valeur, int numeroFrame, char axe, Joint * joint);
    void addTrans(float valeur, int numeroFrame, char axe, Joint * joint);    
            
            
    	
};

#endif

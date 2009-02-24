#ifndef _BVHLOADER_H_
#define _BVHLOADER_H_

#include <list>
#include "MocapLoader.h"


class BvhLoader : public MocapLoader  {
private:
    // ATTRIBUTS

    int totalChannels;          // Le nombre total de channels (pour la réservation mémoire)
    list<Joint *> listeJoints;
    
    // METHODES PRIVEES
    Joint * readJointsFromParser(string currentJointName, Parser * parser);
    void writeJointHeader(Joint * joint, FILE* fp, int decalage);
    void writeOneFrame(FILE* fp, int numeroFrame, Joint * joint);
    
public :
    // CONSTRUCTEURS
    BvhLoader();
    ~BvhLoader();    
    
    // METHODES
    // Pour la lecture
    bool load(string nomFichier); 

    // Pour l'écriture
    bool write(const string nomFichier);

    
};

#endif

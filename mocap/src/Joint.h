#ifndef _JOINT_H_
#define _JOINT_H_

#include <list>
#include "Parser.h"

using namespace std;


class Joint {
private:
    
	// ATTRIBUTS
	int id;                     // Id du joint
	string name;                // Nom
	float offset[3];            // Placement du joint
	int nchannels;              // #DOFs 
	string chtype[6];           // DOF 
	Joint * parent;             // Joint parent, NULL si aucun
	bool transJoint;            // Joint de translation?
	list<Joint*> *enfants;		// Liste de pointeurs sur les enfants
	bool endSite;               // endsite?
	float endLength[3];         // placement du endsite
public:
    // TYPE
    typedef int JointId;
    typedef list<JointId> ListJointId;

	// CONSTRUCTEUR
	Joint(const Joint &copie);
    Joint(string nomJoint, int nchannels, bool transJoint, float offset[3], string chtype[6], int id = -1);
    ~Joint();
    
	// METHODES
	void addChildJoint(Joint * joint);     // Ajoute un joint fils 
	void addEndSite(float endLength[3]);   // Ajoute un terminal au joint (a priori il n'y en aura qu'un seul)
	JointId getId();              // Renvoie l'ID du joint
	Joint * getParentJoint();     // Renvoie le joint Père, NULL si aucun
	int getOffsetTransChannel(char axe);
	int getOffsetRotChannel(char axe);
	float getOffsetPos(char axe);	 // renvoie l'offset défini en dur dans le Bvh entre 2 joints
	void setOffsetPos(float valeur, char axe);
	float getOffsetEndPos(char axe); // Idem mais pour un joint terminal
	void setOffsetEndPos(float valeur, char axe);
	list<Joint*> *getListeEnfants(); // Renvoie la liste des enfants du joint, directement connectés
	bool isTerminal();			// Indique si le joint est un terminal
	bool isTransJoint();        // Indique si le joint possède des données de translations
	string getName();			// Renvoie le nom du joint
	int getNbChannels();		// Renvoie le nombre de paramètres du joint
	string getChannelsType();	// Renvoie la chaine de caractere de l'ensemble des channels (translation [+rotation])
};

#endif

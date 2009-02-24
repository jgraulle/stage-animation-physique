//////////////////////////////////////////////////
//				 CLASSE JOINT
//////////////////////////////////////////////////


#include "Joint.h"


//////////////////////////////////////////////////
// PROC:   Joint::Joint(string nomJoint, int nchannels, bool transJoint, float offset[3], string chtype[6], int chOffset)
// DOES:   constructeur
//////////////////////////////////////////////////
Joint::Joint(string nomJoint, int nchannels, bool transJoint, float offset[3], string chtype[6], int id) {
    // Cree la liste des enfants
    this->enfants = new list<Joint*>();
    
    // Fixe les paramètres
    this->parent = NULL;
   	this->name = nomJoint;
   	this->nchannels = nchannels;   	
   	this->transJoint = transJoint;  
    this->endSite = false;   	 	
   	memcpy(this->offset, offset, sizeof(float)*3);   	
   	//memcpy(this->endLength, endLength, sizeof(float)*3);   	
   	//memcpy(this->chtype, chtype, sizeof(string)*6);   	
   	for (int i=0; i<6; i++)
       	this->chtype[i] = chtype[i];
   	//this->chOffset = chOffset;   
       
    //sphereSize = 1;	
 	
    static int idProgressing = 1;
    // Si l'id n'a pas été spécifié, on en génére
    if (id == -1)
        this->id = idProgressing++;
    else 
        this->id = id;


   	
}

//////////////////////////////////////////////////
// PROC:   Joint::Joint(const Joint &copie)
// DOES:   constructeur par copie
//////////////////////////////////////////////////
Joint::Joint(const Joint &copie) {
    id = copie.id;
    
	name = copie.name;
	memcpy(offset,copie.offset,sizeof(float)*3);
	nchannels = copie.nchannels;
	memcpy(chtype,copie.chtype,sizeof(string)*6);
	parent = NULL;
	transJoint = copie.transJoint;  
     
	enfants = new list<Joint*>();
 	list<Joint*>::iterator it = copie.enfants->begin();
	while (it != copie.enfants->end()) {
		Joint * unEnfant = *(it++);
		addChildJoint(new Joint(*unEnfant));
		//enfants->push_back(new Joint(*unEnfant));
	}              
	//enfants = new list<Joint*>(*(copie.enfants));;
    		
	endSite = copie.endSite;   
	memcpy(endLength,copie.endLength,sizeof(float)*3);                    

}

//////////////////////////////////////////////////
// PROC:   Joint::~Joint()
// DOES:   destructeur
//////////////////////////////////////////////////
Joint::~Joint() {

    // Detruit l'ensemble des fils
	for (list<Joint*>::iterator it = enfants->begin();it != enfants->end(); it++) {
		Joint * unEnfant = *it;
		delete unEnfant;
	}   
	
	
	// Detruit la liste des enfants
	delete enfants;
	

}

//////////////////////////////////////////////////
// PROC:   getParentJoint() {
// DOES:   Renvoie le joint parent (NULL si aucun parent)
//////////////////////////////////////////////////
Joint * Joint::getParentJoint() {
    return parent;   
}

//////////////////////////////////////////////////
// PROC:   Joint::addChildJoint(Joint * joint)
// DOES:   Ajoute un joint a la liste des enfants
//////////////////////////////////////////////////
void Joint::addChildJoint(Joint * joint) {
    if (enfants != NULL) {
        enfants->push_back(joint);
    }
    joint->parent = this;
}

//////////////////////////////////////////////////
// PROC:   addEndSite(float endLength[3])
// DOES:   Ajoute un terminal au joint (a priori il n'y en aura qu'un seul)
//////////////////////////////////////////////////
void Joint::addEndSite(float endLength[3]) {
    // Copie de la position du terminal
    memcpy(this->endLength, endLength, sizeof(float)*3); 
    
    // Indique la présence du terminal
    endSite = true;      
}

//////////////////////////////////////////////////
// PROC:   getId()
// DOES:   retourne l'id du joint
//////////////////////////////////////////////////
int Joint::getId() {
    return id;   
}

//////////////////////////////////////////////////
// PROC:   Joint::getOffsetPos()
// DOES:   renvoie l'offset défini en dur dans le Bvh entre 2 joints
//////////////////////////////////////////////////
float Joint::getOffsetPos(char axe) {

	switch(axe) {
		case 'X':
			return offset[0];
			break;
		case 'Y':
			return offset[1];
			break;
		case 'Z':
			return offset[2];
			break;
		default:
			return 0;
		}
}

//////////////////////////////////////////////////
// PROC:   Joint::setOffsetPos()
// DOES:   Défini l'offset en dur dans le Bvh entre 2 joints
//////////////////////////////////////////////////
void Joint::setOffsetPos(float valeur, char axe) {
	switch(axe) {
		case 'X':
			offset[0] = valeur;
			break;
		case 'Y':
			offset[1] = valeur;
			break;
		case 'Z':
			offset[2] = valeur;
			break;
		}

}

//////////////////////////////////////////////////
// PROC:   Joint::getOffsetEndPos()
// DOES:   renvoie l'offset défini en dur dans le Bvh entre 2 joints dans le cas
//			d'un joint terminal seulement
//////////////////////////////////////////////////
float Joint::getOffsetEndPos(char axe) {
	if (endSite) {
		switch(axe) {
			case 'X':
				return endLength[0];
				break;
			case 'Y':
				return endLength[1];
				break;
			case 'Z':
				return endLength[2];
				break;
			default:
				return 0;
			}
	} else return 0;
}

//////////////////////////////////////////////////
// PROC:   Joint::SetOffsetEndPos()
// DOES:   Défini l'offset en dur dans le Bvh entre 2 joints dans le cas
//			d'un joint terminal seulement
//////////////////////////////////////////////////
void Joint::setOffsetEndPos(float valeur, char axe) {
	switch(axe) {
		case 'X':
			endLength[0] = valeur;
			break;
		case 'Y':
			endLength[1] = valeur;
			break;
		case 'Z':
			endLength[2] = valeur;
			break;
		}

}

//////////////////////////////////////////////////
// PROC:   Joint::getOffsetPosChannel()
// DOES:   renvoie l'offset dans la frame du joint de la donnée de position d'un des axe
//			En général, X en premier (Offset=0), Y en deuxieme, Z en 3ieme
//////////////////////////////////////////////////
int Joint::getOffsetTransChannel(char axe) {


	// Parcours l'ensemble des translations du joint
	if (transJoint) {
		for (int r=0; r<3; r++) {
			if (chtype[r][0] == axe) {
					return r;// + chOffset;
			}
		}
	}

	// Ne doit pas arriver?
	return 0;
}

//////////////////////////////////////////////////
// PROC:   Joint::getOffsetRotChannel()
// DOES:   renvoie l'offset dans la frame du joint de la donnée de rotation d'un des axe
//			En général, Z en premier (Offset=0), X en deuxieme, Y en 3ieme
//////////////////////////////////////////////////
int Joint::getOffsetRotChannel(char axe) {

	// Enregistre le décalage de 3 en offset si le joint à des champs de position (rare à part la racine)
	int debut=0;
	if (transJoint) {
		debut = 3;
	}
	
	// Parcours l'ensemble des rotations du joint
	for (int r=debut; r<nchannels; r++) {
		if (chtype[r][0] == axe) {
				return r;// + chOffset;
		}
	}

	// Ne doit pas arriver?
	return 0;
}

//////////////////////////////////////////////////
// PROC:   Joint::getListeEnfants()
// DOES:   Renvoie la liste des enfants
//////////////////////////////////////////////////
list<Joint*> * Joint::getListeEnfants() {
	return enfants;
}

//////////////////////////////////////////////////
// PROC:   Joint::isTerminal()
// DOES:   Indique sur le joint est un terminal
//////////////////////////////////////////////////
bool Joint::isTerminal() {
	return endSite;
}

//////////////////////////////////////////////////
// PROC:   Joint::isTransJoint()
// DOES:   Indique sur le joint possède des attributs 
//         de translation (en général, seulement le root)
//////////////////////////////////////////////////
bool Joint::isTransJoint() {
	return transJoint;
}

//////////////////////////////////////////////////
// PROC:   Joint::check_channel_names()
// DOES:   verifies that all channels have known names
//////////////////////////////////////////////////
/*
void Joint::check_channel_names()
{
     
	int startChannel = 0;
	if (transJoint)                // translations already checked
		startChannel = 3;
	for (int c=startChannel; c<nchannels; c++) {
		if (chtype[c] == "Xrotation" && 
			chtype[c] =="Yrotation" &&
			chtype[c] == "Zrotation")
			printf("Unrecognized channel type: '%s'\n",chtype[c]);
			//bvh->parseError = true;
	}
}
*/

//////////////////////////////////////////////////
// PROC:   Joint::getName()
// DOES:   renvoie le nom du joint
//////////////////////////////////////////////////
string Joint::getName() {
	return name;
}

//////////////////////////////////////////////////
// PROC:   Joint::getNbChannels()
// DOES:   renvoie le nom du joint
//////////////////////////////////////////////////
int Joint::getNbChannels() {
	return nchannels;
}

//////////////////////////////////////////////////
// PROC:   Joint::getChannelsType()
// DOES:   renvoie le type des channels sous forme de chaine de caractère
//////////////////////////////////////////////////
string Joint::getChannelsType() {

	string resultat = "";
	for (int i=0; i<getNbChannels(); i++) {
		resultat += chtype[i] + " ";
	}

	return resultat;
}

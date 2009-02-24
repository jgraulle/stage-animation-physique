////////////////////////////////////////////////
// Fichier:  Bvh.h
////////////////////////////////////////////////

#ifndef _BVH_H_
#define _BVH_H_

#include <string>
#include <list>
#include <map>
#include <float.h>
#include <math.h>


#include "Joint.h"
#include "MocapData.h"
#include "SkinLoader.h"
#include "MocapLoader.h"
#include "SkeletalData.h"
#include "MocapManipulator.h"
#include "Point3D.h"

#include "ActorRenderFil.h"
#include "ActorRenderSkin.h"

using namespace std;

#define FLT_MAX 10000;

/////////////////////////////////
//			CLASSE BVH
/////////////////////////////////
class BvhGlissementPied;
class ActorRender;

class Bvh {
private:

	// ATTRIBUTS
	//ActorRender * actorRender;             // Rendu du personnage
	MocapData * mocapData ;                // Les données de l'animation
    SkeletalData * skeletalData;           // Les donnée du squelette
    SkinData * skinData;                   // Les données pour le skin (si existant)
    MocapManipulator * mocapManipulator;

	int frameDebutSupport, frameFinSupport; // Indique les frames de début et de fin de la phase de support d'un pied
											// Mis à jour par detecterSupportPied()

	// Optimiseurs anti-glissement pour chaque pied
	BvhGlissementPied * bvhGlissementPied1, * bvhGlissementPied2;

	// Le nom d'articulation du haut des 2 jambes
	string jambeDroite;
	string jambeGauche;

	// METHODES
	void init();			  // Permet d'initialiser les variable pour une nouvelle lecture
	float getAngleOrientation(); // Renvoie l'angle d'écart selon Y par rapport à la direction de référence (tout droit)

	// Renvoie la position d'un joint par son nom à partir d'un joint de reference
	//Point3D * getPositionJointByName(int numeroFrame, string nomJoint, bool getEndSite, float *matRot);
	// Joint par son nom (recherche à la racine par defaut)
	Joint * getJointByName(string nomJoint, Joint * referenceJoint = NULL);

	// Utilisée pour la récursivité de la méthode publique getListAllJoint()
	map<string, void*> * getListAllJoint(Joint * referenceJoint);


public:
	// CONSTRUCTEURS
	Bvh();
	Bvh(const Bvh &copie);
	Bvh(string nomFichier);
	~Bvh();

    // GETTERS
	MocapData * getMocapData();
    SkeletalData * getSkeletalData();
    SkinData * getSkinData();

	// METHODES
	int read(const string fname);                      // Loade une mocap
	int write(const string fname);						// Enregistre l'animation une mocap
	//void drawFrame(int f);								// Affiche une frame
	Joint * getRootSqueleton();                         // Renvoie la racine du squelette
	int getNchannels();									// Renvoie le nombre total de paramètres de l'animation
	int getNbFrame();									// Renvoie le nombre de frame de l'animation
	float getDureeFrame();								// Renvoie le temps entre deux frame
	void setEchelle(float echelle);						// Fixe l'echelle de rendu de l'animation
	float getEchelle();						            // Renvoie l'echelle de rendu de l'animation
	void setJambeDroite(string nomJambeDroite);
	void setJambeGauche(string nomJambeGauche);
	string getJambeDroite();
	string getJambeGauche();
    bool canWalk();                                     // Vrai si les 2 jambes sont nommées

	void centrerAnimation();							// Centre l'animation en X=0, Z=0; Y=?
	void fixerAnimation();								// Fixe la racine du squelette dans son animation
	void fixerAltitudeMarche(float altitude, string nomPied); // Fixe l'alitude d'un pied sur un plan défini
	float getVitesseDeplacement(string nomPremierPied, string nomDeuxiemePied); // Renvoie la vitesse de marche à partir d'un pied en unité de déplacement par seconde
	string getEndJointName(string nomJoint);			// Renvoie le nom du joint terminal en partant d'un joint
	void clearOptimisation();							// Efface les informations d'optimisations
	void turnAroundY(float angle);						// Change l'orientation de l'animation par rotation autour de l'axe Y (la verticale)
	void rendreCyclique(string  nomPremierPied, string  nomDeuxiemePied);
	void fixerTailleSquelette(float valeur);            // Change l'echelle de l'animation et du squelette (réduction, augmentation)

	// Renvoie une stl::map d'association clé-valeur contenant l'arbre du squelette du personnage
	// clé = nom du joint, valeur = pointeur sur une valeur de type map
	// si clé = NULL, alors il s'agit d'une feuille
	map<string, void*> * getListAllJoint();

	// Renvoie la chaine entier vers un terminal à partir d'un joint
	list<Joint*> * getChaineJointVersTerminal(string nomJoint);


	// Renvoie la position d'un Joint par son nom dans une frame
	Point3D * getPositionJointByNameInFrame(int numeroFrame, string nomJoint, bool getEndSite = false, float*matRot = NULL, bool withtoutScale = false);

	// Ik par ccd sur une chaine simple (pas de dédoublement)
	void appliquerIkCcd(int numeroFrame, string nomJoint, Point3D positionVoulue, int nbIterationsMax);
	void appliquerIkCcd(int numeroFrame, list<Joint*> * chaine, Point3D positionVoulue, int nbIterationsMax);

	// Renvoie la valeur de translation d'un joint selon un axe pour une frame
	float getTransJoint(int numeroFrame, Joint * joint, char axe);
	// Renvoie la valeur de rotation d'un joint selon un axe pour une frame
	float getRotJoint(int numeroFrame, Joint * joint, char axe);
	// Fixe la valeur de rotation d'un joint selon un axe pour une frame
	void setRotJoint(float valeur, int numeroFrame, Joint * joint, char axe);

	// Fixe l'orientation de la racine en alignement selon un axe
	void alignerAnimationSensDeMarche();

	// Detecte la phase de support des pieds
	void detecterSupportPied(string nomPremierPied, string nomDeuxiemePied);

	// Indique si la phase de support du premier pied est en cours
	bool isFirstFootSupportPeriod(int frameCourante);

    // Change le premier pied de support
    void changerPremierPiedSupport();

	// Fixe l'éventuel glissement des pieds
	// Attention, on suppose qu'un appel a detecterSupportPied a été fait avant, sinon aucun effet
	void fixerGlissementPied(int numeroFrame, float xPers, float yPers, float direction, string nomHautJambe1, string nomHautJambe2);

	// Génere une mocap différente (en construction, pour tests seulement)
	//void genererMocap();

	// Interpole deux animations
	// Attention, l'animation courante est modifiée
	static Bvh * interpoler(Bvh* bvh1, Bvh * bvh2, float poids);
};


/////////////////////////////////
//	 CLASSE BVHGLISSEMENTPIED
/////////////////////////////////
class BvhGlissementPied{

private:
	// ATTRIBUTS

	// Fichier bvh parent
	Bvh * bvh;

	// Variable temporaire du processus de non glissement des pieds
	Point3D * pointPiedDebutMonde, * pointPiedDebutPers;
	Point3D *A, *B;

	// Chaine de la jambe, pour éviter de recalculer la liste des Joints constituant la jambe
	list<Joint*> * chaine;

	// Nom du joint du haut de la jambe
	string nomHautJambe;

	// Nom du pied
	string nomPied;

	// La dernière frame traitée
	int derniereFrameTraitee;


public:
	// CONSTRUCTEUR
	BvhGlissementPied(Bvh *bvh, string nomHautJambe);

	// METHODES

	// Permet de reinitialiser l'objet et les informations d'optimisation de la marche:
	// a utiliser si on déplace le personnage de façon non naturelle (exemple: téléportation)
	void init();

	// Fixe le glissement d'un pied
	void fixerGlissementPied(int numeroFrame, int frameDebutSupport, int frameFinSupport, float xPers, float yPers, float direction);

};

#endif


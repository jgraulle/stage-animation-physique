
//////////////////////////////////////////////////////////////
// Fichier:   bvh->cpp
//         simple parsing of a Bvh mocap file
// AUTHOR  Michiel van de Panne, Oct 2004,
//		   Refonte totale par J.Salut, 2006
//////////////////////////////////////////////////////////////

//////////////////////////////////////////////////
//				 CLASSE BVH
//////////////////////////////////////////////////


#include "bvh.h"

#include "misc.h"

#include <GL/gl.h>

using namespace std;

//////////////////////////////////////////////////
// PROC:   Bvh::Bvh()
// DOES:   Constructeurs
//////////////////////////////////////////////////

Bvh::Bvh()
{

}

Bvh::Bvh(const Bvh &copie) {


	mocapData = new MocapData(*copie.mocapData);
    skeletalData = new SkeletalData(*copie.skeletalData);
    if (copie.skinData) {
        skinData = new SkinData(*copie.skinData);
    } else {
        skinData = NULL;
    }
    mocapManipulator = new MocapManipulator(mocapData, skeletalData);

	//actorRender = new ActorRenderSkin(mocapData, skeletalData, skinData);

	bvhGlissementPied1 = NULL;
	bvhGlissementPied2 = NULL;
	frameDebutSupport = -1;
	frameFinSupport = -1;

	frameDebutSupport = copie.frameDebutSupport;
    frameFinSupport = copie.frameFinSupport;

    jambeDroite = copie.jambeDroite;
    jambeGauche = copie.jambeGauche;


}

Bvh::Bvh(const string nomFichier)
{


    // Lecture du fichier
	read(nomFichier);

	// Création de l'actor render
	//actorRender = new ActorRenderSkin(mocapData, skeletalData, skinData);

    // Une fois que toutes les données sont chargées
    mocapManipulator = new MocapManipulator(mocapData, skeletalData);

}

Bvh::~Bvh() {

    //delete actorRender;
    delete skinData;
    delete mocapManipulator;
    delete mocapData;
    delete skeletalData;

    if (bvhGlissementPied1 != NULL) {
        delete bvhGlissementPied1;
    }
    if (bvhGlissementPied2 != NULL) {
        delete bvhGlissementPied2;
    }

}

// Initialise l'objet pour etre pret pour une nouvelle lecture
void Bvh::init() {
    jambeDroite = "";
    jambeGauche = "";

	bvhGlissementPied1 = NULL;
	bvhGlissementPied2 = NULL;
	skinData = NULL;

	//nchannels = 0;

	//drawScale = 1;

	frameDebutSupport = -1;
	frameFinSupport = -1;

	//drawCenter[0] = drawCenter[1] = drawCenter[2] = 0;



}

// GETTERS
MocapData * Bvh::getMocapData() {
    return mocapData;
}

SkeletalData * Bvh::getSkeletalData() {
    return skeletalData;
}

SkinData * Bvh::getSkinData() {
    return skinData;
}

//////////////////////////////////////////////////
// PROC:   bvh::write(string fname)
// DOES:   ecrit l'animation en cours dans un fichier
//////////////////////////////////////////////////
int Bvh::write(const string fname)
{
    MocapLoader * mocapLoader = MocapLoader::getInstanceOfAppriopriateLoader(fname);
    mocapLoader->setMocapData(mocapData);
    mocapLoader->setSkeletalData(skeletalData);
    bool erreur = mocapLoader->write(fname);

    delete mocapLoader;
    return erreur;

}

//////////////////////////////////////////////////
// PROC:   bvh::read(string fname)
// DOES:   charge une mocap
//////////////////////////////////////////////////

int Bvh::read(const string fname)
{

	// Initialise les variable globales de l'objet pour une lecture
	init();

    // Création du loader approprié
    MocapLoader * mocapLoader = MocapLoader::getInstanceOfAppriopriateLoader(fname);

    // Chargement
    int erreur = mocapLoader->load(fname);
    mocapData = mocapLoader->getMocapData();
    skeletalData = mocapLoader->getSkeletalData();
    skinData = mocapLoader->getSkinData();

    // Efface le loader
    delete mocapLoader;

    // Renvoie l'etat du chargement
	return erreur;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getRootSqueleton()
// DOES:   // Renvoie la racine du squelette
//////////////////////////////////////////////////
Joint * Bvh::getRootSqueleton() {
    //return root;
    return skeletalData->getRootSqueleton();
}

//////////////////////////////////////////////////
// PROC:   Bvh::setJambeDroite()
// DOES:   Fixe le nom de la jambe droite
//////////////////////////////////////////////////
void Bvh::setJambeDroite(string nomJambeDroite) {
    this->jambeDroite = nomJambeDroite;
}

//////////////////////////////////////////////////
// PROC:   Bvh::setJambeGauche()
// DOES:   Fixe le nom de la jambe gauche
//////////////////////////////////////////////////
void Bvh::setJambeGauche(string nomJambeGauche) {
    this->jambeGauche = nomJambeGauche;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getJambeDroite()
// DOES:   Renvoie le nom de la jambe droite
//////////////////////////////////////////////////
string Bvh::getJambeDroite() {
    return jambeDroite;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getJambeGauche()
// DOES:   Renvoie le nom de la jambe gauche
//////////////////////////////////////////////////
string Bvh::getJambeGauche() {
    return jambeGauche;
}

//////////////////////////////////////////////////
// PROC:   Bvh::canWalk()
// DOES:   Indique si le personnage peut marcher (si les 2 jambes sont nommées en fait)
//////////////////////////////////////////////////
bool Bvh::canWalk() {
    return (jambeDroite != "" && jambeGauche != "");
}

//////////////////////////////////////////////////
// PROC:   Bvh::fixerTailleSquelette()
// DOES:   Change l'echelle de l'animation et du squelette (réduction, augmentation)
//          valeur = 1.0 => aucun effet
//                 < 1.0 => diminution
//                 > 1.0 => augmentation
//////////////////////////////////////////////////
void Bvh::fixerTailleSquelette(float valeur) {

    // Pour chaque joint de la frame
    SkeletalData::JointListIndexedByName listeJoints =  skeletalData->getListeJointsByName();
    for (SkeletalData::JointListIndexedByName::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
        Joint * jointCourant = it->second;

        // A ne faire qu'une fois: on ajuste les offset/endoffset des joints
        float tX = jointCourant->getOffsetPos('X');
        float tY = jointCourant->getOffsetPos('Y');
        float tZ = jointCourant->getOffsetPos('Z');

        // Ajustement des offset
        tX = tX * valeur;
        tY = tY * valeur;
        tZ = tZ * valeur;

        // Stockage des nouvelles valeurs
        jointCourant->setOffsetPos(tX, 'X');
        jointCourant->setOffsetPos(tY, 'Y');
        jointCourant->setOffsetPos(tZ, 'Z');


        // Enfin meme chose pour les endoffset
        if (jointCourant->isTerminal()) {
            tX = jointCourant->getOffsetEndPos('X');
            tY = jointCourant->getOffsetEndPos('Y');
            tZ = jointCourant->getOffsetEndPos('Z');

            // Ajustement des offset
            tX = tX * valeur;
            tY = tY * valeur;
            tZ = tZ * valeur;

            // Stockage des nouvelles valeurs
            jointCourant->setOffsetEndPos(tX, 'X');
            jointCourant->setOffsetEndPos(tY, 'Y');
            jointCourant->setOffsetEndPos(tZ, 'Z');
        }

    }


	for (int numeroFrame=0; numeroFrame<getNbFrame(); numeroFrame++) {
		// Pour chaque joint de la frame
		SkeletalData::JointListIndexedByName listeJoints =  skeletalData->getListeJointsByName();
		for (SkeletalData::JointListIndexedByName::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
            Joint * jointCourant = it->second;

            for (int k =1; k<3; k++){
                // La mocapData à retailler:
                // Les données de l'animation ET la posture de référence contenue dans le skinData
                MocapData * mocapData;
                switch (k) {
                    case 1: mocapData = this->mocapData; break;
                    case 2: if (skinData) {
                                if (numeroFrame<skinData->getReferencePosData()->getNbFrames()) {
                                    mocapData = skinData->getReferencePosData();
                                } else {
                                    continue;
                                }
                            } else {
                                continue;
                            }
                            break;
                    default: mocapData = NULL; return;
                }

                if (jointCourant->isTransJoint()) {
                    // On récupere les 3 paramètres
                    float tX = mocapData->getTrans(numeroFrame, 'X', jointCourant);
                    float tY = mocapData->getTrans(numeroFrame, 'Y', jointCourant);
                    float tZ = mocapData->getTrans(numeroFrame, 'Z', jointCourant);

                    // Ajustement des translations
                    tX = tX * valeur;
                    tY = tY * valeur;
                    tZ = tZ * valeur;

                    // Stockage des nouvelles valeurs
                    mocapData->setTransXYZ(tX, tY, tZ, numeroFrame, jointCourant);

                }
            }




        }
    }
    // On n'oublie pas de retailler le skin aussi (si présent)
    if (skinData)
        skinData->resizeSkin(valeur);
}

//////////////////////////////////////////////////
// PROC:   Bvh::centerAnimation()
// DOES:   centre l'animation de départ en (0,?,0)
//////////////////////////////////////////////////
void Bvh::centrerAnimation()
{
	// Détermine X et Z de la racine
	float Xroot, Zroot;
	//Xroot = data[root->getOffsetTransChannel('X')];
	//Zroot = data[root->getOffsetTransChannel('Z')];
	Xroot = mocapData->getTrans(0,'X',getRootSqueleton());
	Zroot = mocapData->getTrans(0,'Z',getRootSqueleton());

	// Decale l'animation en conséquence
	for (int f=0; f<getNbFrame(); f++) {
		//int i = f*nchannels;

		mocapData->addTrans(-Xroot, f,'X',getRootSqueleton());
		mocapData->addTrans(-Zroot, f,'Z',getRootSqueleton());
		//data[i+0] -= Xroot; // X
		//data[i+2] -= Zroot; // Z
	}
}

//////////////////////////////////////////////////
// PROC:   Bvh::fixerAnimation()
// DOES:   fixe la racine de l'animation en (0,?,0)
//			en dépit des déplacements relatifs
//////////////////////////////////////////////////
void Bvh::fixerAnimation()
{
	// Détermine X et Z de la racine
	float Xroot, Zroot;
	Xroot = mocapData->getTrans(0,'X',getRootSqueleton()) - getRootSqueleton()->getOffsetPos('X');
	Zroot = mocapData->getTrans(0,'Z',getRootSqueleton()) - getRootSqueleton()->getOffsetPos('Z');


	// Calcul de la droite de régression (méthodes des moindres carrés)
	// Le nuage de point est l'ensemble des positions de la racine au cours du temps
	// On obtiendra en sortie la direction globale de l'animation
	// Z = bo + b1 X
	float b0 = 0, b1 = 0, sommeXk = 0, sommeXkC = 0, sommeYk = 0, sommeXkYk = 0;
	for (int k=0;k<getNbFrame();k++) {
	  //float Xroot = data[k * nchannels + root->getOffsetTransChannel('X')];
   	  //float Zroot = data[k * nchannels + root->getOffsetTransChannel('Z')];
	  float Xroot = mocapData->getTrans(k, 'X', getRootSqueleton());
	  float Zroot = mocapData->getTrans(k, 'Z', getRootSqueleton());
      sommeXk += Xroot;
	  sommeYk += Zroot;
	  sommeXkC += Xroot*Xroot;
	  sommeXkYk += Xroot*Zroot;
	}
	b0 = (sommeXkC*sommeYk - sommeXk * sommeXkYk)
		/ (getNbFrame() * sommeXkC - sommeXk * sommeXk);
	b1 = (getNbFrame() * sommeXkYk - sommeXk * sommeYk)
		/ (getNbFrame() * sommeXkC - sommeXk * sommeXk);


	// Decale l'animation en conséquence
	for (int f=0; f<getNbFrame(); f++) {
		//int i = f*nchannels;

		// On calcule l'acceleration

		Point3D pi = Point3D(mocapData->getTrans(f, 'X', getRootSqueleton()),
							 0,
							 mocapData->getTrans(f, 'Z', getRootSqueleton()));
		//Point3D pip1 = Point3D(mocapData->getTrans(f+1, 'X', root),
		//					  0,
		//					  0);
		//Point3D pip2 = Point3D(mocapData->getTrans((f+1)%getNbFrame(), 'X', root),
		//					   0,
		//					   mocapData->getTrans((f+1)%getNbFrame(), 'Z', root));

        // On calcule la distance du point courant à la droite de regression
		//float d = fabs((b1 * pi.getX() - pi.getZ() + b0) / sqrt(b1*b1 + 1));
		//if (b1 * pi.getX() - pi.getZ() + b0 <0)
		//  distance = - distance;

		float a = b1;
		float b = -1;
		float c = b0;

        Point3D projetePi = Point3D((b*b*pi.getX() - a*b*pi.getZ() - a*c)
                                    / (a*a+b*b),
                                    0,
                                    (-a*b*pi.getX() + a*a*pi.getZ() - b*c)
                                    / (a*a+b*b));

		//float a = pip2.distanceVers(pip1) - pip1.distanceVers(pi);

		// On fixe la racine
		//data[i+root->getOffsetTransChannel('X')] = Xroot; // X
		//data[i+root->getOffsetTransChannel('Z')] = Zroot; // Z

		//data[i+root->getOffsetTransChannel('X')] = Xroot; // X
		//data[i+root->getOffsetTransChannel('Z')] = Zroot; // Z
		mocapData->setTrans(Xroot - projetePi.getX() + pi.getX(), f,'X',getRootSqueleton());
		mocapData->setTrans(Zroot - projetePi.getZ() + pi.getZ(), f,'Z',getRootSqueleton());
	}


}

//////////////////////////////////////////////////
// PROC:   Bvh::fixerAltitudeJoint()
// DOES:   Fixe l'altitude des pieds lors de la marche
//////////////////////////////////////////////////
void Bvh::fixerAltitudeMarche(float altitude, string  nomPied)
{

	// La plus faible altitude du pied
	float minY = FLT_MAX;

	// Pour chaque frame
	for (int f=0; f<getNbFrame(); f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPied, true, NULL, true);
		if (!posPied) return;

		// Sauvegarde la plus faible altitude du pied
		if (posPied->getY() < minY) {
			minY = posPied->getY();
		}

		delete posPied;
	}

	// Decale l'animation en conséquence
	for (int f=0; f<getNbFrame(); f++) {
		//int i = f*nchannels;

		// On fixe la racine
		//data[i+root->getOffsetTransChannel('Y')] += -minY + altitude;
		mocapData->addTrans(-minY + altitude, f, 'Y', getRootSqueleton());

	}

}

//////////////////////////////////////////////////
// PROC:   Bvh::getVitesseDeplacement()
// DOES:   Calcule la vitesse de déplacement de l'animation de marche (si celà en est une)
//		   Le principe est de mesurer l'intervalle de temps entre la pose du premier pied et la pose du deuxieme pied
//////////////////////////////////////////////////
float Bvh::getVitesseDeplacement(string  nomPremierPied, string  nomDeuxiemePied)
{
	// Détection de la phase de support
	detecterSupportPied(nomPremierPied,nomDeuxiemePied);

	// Calcul de la vitesse
	Point3D * posPiedDebut = getPositionJointByNameInFrame(frameDebutSupport, nomPremierPied, true);
	Point3D * posPiedFin = getPositionJointByNameInFrame(frameFinSupport, nomPremierPied, true);
	if (!posPiedDebut || !posPiedFin) return 0;
	Point3D resultat = posPiedDebut->distanceVers(*posPiedFin);
	float vitesse = resultat.norme() /( getDureeFrame() *(frameFinSupport - frameDebutSupport)); // V = distance / temp

	return vitesse;

}



//////////////////////////////////////////////////
// PROC:   Bvh::fixerGlissementPied()
// DOES:   Fixe le flissement de chaque pied
//		   Attention, on suppose qu'un appel a detecterSupportPied a été fait avant, sinon aucun effet
//////////////////////////////////////////////////
void Bvh::fixerGlissementPied(int numeroFrame, float xPers, float yPers, float direction, string  nomHautJambe1, string  nomHautJambe2) {


	if (bvhGlissementPied1 == NULL || bvhGlissementPied2 == NULL) {
		bvhGlissementPied1 = new BvhGlissementPied(this, nomHautJambe1);
		bvhGlissementPied2 = new BvhGlissementPied(this, nomHautJambe2);

		detecterSupportPied(getEndJointName(nomHautJambe1), getEndJointName(nomHautJambe2));
	}

	// Si aucun support n'a été trouvé, on ne fait rien
	if (frameDebutSupport == -1 ||frameFinSupport == -1) return;

	bvhGlissementPied1->fixerGlissementPied(numeroFrame, frameDebutSupport, frameFinSupport, xPers, yPers, direction);
	bvhGlissementPied2->fixerGlissementPied(numeroFrame, frameFinSupport, frameDebutSupport, xPers, yPers, direction);

}


//////////////////////////////////////////////////
// PROC:   Bvh::detecterSupportPied()
// DOES:   Le principe est de trouver l'intervalle de temps entre la pose du premier pied et la pose du deuxieme pied
//		   En sortie, 2 variable sont mise à jour
//////////////////////////////////////////////////
void Bvh::detecterSupportPied(string  nomPremierPied, string  nomDeuxiemePied) {
    /*
	// Variation d'altitude autorisée du pied durant la phase de support
	const float epsilonAltitude = 0.2;
	const float epsilonVitesse = 0.2;

	// Invalidation des résultats
	this->frameDebutSupport = -1;
	this->frameFinSupport = -1;

	// L'altitude min du pied
	float minYpied1 = FLT_MAX;
	float minYpied2 = FLT_MAX;

	// Premiere passe: recherche de l'altitude min du pied
	for (int f=0; f<getNbFrame(); f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPremierPied, true, NULL, true);
		Point3D * posPied2 = getPositionJointByNameInFrame(f, nomDeuxiemePied, true, NULL, true);

		if (!posPied || !posPied2) return;

		// Sauvegarde la plus faible altitude du pied
		if (posPied->getY() < minYpied1) {
			minYpied1 = posPied->getY();

		}
		if (posPied2->getY() < minYpied2) {
			minYpied2 = posPied2->getY();

		}

		delete posPied;
        delete posPied2;
	}


	// La frame de début du support du pied
	int frameDebutSupport = -1;
	int frameFinSupport = -1;
	bool pied1First = false;
	bool secondPeriodReady = false;
	// Deuxieme passe: recherche le debut et la fin du support du pied
	for (int f=0; f<getNbFrame(); f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPremierPied, true, NULL, true);
		Point3D * posPied2 = getPositionJointByNameInFrame(f, nomDeuxiemePied, true, NULL, true);
		if (!posPied || !posPied2) return;


		Point3D * posPiedOld = getPositionJointByNameInFrame((f+1)%getNbFrame(), nomPremierPied, true, NULL, true);
		Point3D * posPied2Old = getPositionJointByNameInFrame((f+1)%getNbFrame(), nomDeuxiemePied, true, NULL, true);

		// Lorsque que l'altitude de la phase de support est quasiment atteinte
		if (frameDebutSupport == -1) {
			if ((posPied->getY() - minYpied1 <= epsilonAltitude)
				& (fabs(posPied->getZ() - posPiedOld->getZ())<= epsilonVitesse)) {

				frameDebutSupport = f;
				pied1First = true;
			} else if ((posPied2->getY() - minYpied2 <= epsilonAltitude)
				& (fabs(posPied2->getZ() - posPied2Old->getZ())<= epsilonVitesse)) {

				frameDebutSupport = f;
				pied1First = false;
			}

		} else if ((frameDebutSupport != -1) & (!secondPeriodReady)) {


			// Selon la situation, on teste si un des 2 pieds se relève
			if ((posPied->getY() - minYpied1 > epsilonAltitude)
				& (fabs(posPied->getZ() - posPiedOld->getZ())> epsilonVitesse/2)
				& (!pied1First)) {
                secondPeriodReady = true;

			} else if ((posPied2->getY() - minYpied2 > epsilonAltitude)
				& (fabs(posPied2->getZ() - posPied2Old->getZ())> epsilonVitesse/2)
				& (pied1First)) {
                secondPeriodReady = true;
			}



        } else if (secondPeriodReady) {
            if ((posPied2->getY() - minYpied2 <= epsilonAltitude)
    			& (fabs(posPied2->getZ() - posPied2Old->getZ())<= epsilonVitesse) & pied1First){

    			frameFinSupport = f;

    		} else if ((posPied->getY() - minYpied1 <= epsilonAltitude)
    			& (fabs(posPied->getZ() - posPiedOld->getZ())<= epsilonVitesse) & !pied1First) {
                frameFinSupport = f;
            }
        }



		delete posPied;
        delete posPied2;
		delete posPiedOld;
		delete posPied2Old;
	}


	// Mise à jour des variables globales
	this->frameDebutSupport = frameDebutSupport;
	this->frameFinSupport = frameFinSupport;
    */


    /* ICIII */
    /*
	int frameDebutRecherche = getNbFrame() /4;
	int frameFinRecherche = getNbFrame()* 0.75;

	// L'altitude min du pied
	float minYpied1 = FLT_MAX;
	float minYpied2 = FLT_MAX;
    float ecartementMax = -1;
	// Premiere passe: recherche de l'altitude min du pied
	for (int f=0; f<getNbFrame(); f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPremierPied, true, NULL, true);
		Point3D * posPied2 = getPositionJointByNameInFrame(f, nomDeuxiemePied, true, NULL, true);

		if (!posPied || !posPied2) return;

		// Sauvegarde la plus faible altitude du pied
		if (posPied->getY() < minYpied1) {
			minYpied1 = posPied->getY();

		}
		if (posPied2->getY() < minYpied2) {
			minYpied2 = posPied2->getY();

		}
        if ( f>frameDebutRecherche & f<frameFinRecherche &(ecartementMax <= fabs(posPied->getZ()) + fabs(posPied2->getZ())) ) {
            ecartementMax = fabs(posPied->getZ()) + fabs(posPied2->getZ());
        }

		delete posPied;
        delete posPied2;
	}

    // On considère que le mouvement a été rendu cyclique par notre module
    // Donc forcément le début de la mocap est une phase de support du pied initiateur
    // Il suffit alors de déterminer quel est le pied initiateur pour déterminer l'ordre
    // de début/fin support
    // En effet frameDebutSupport est toujours le début du support du pied1 (qu'il soit gauche ou droite)


    const float epsilonDeplacement = 0.2;
    const float epsilonAltitude = 0.2;

    // On détermine alors à quel moment le premier pied fini sa phase
    // (pas forcément en milieu de l'animation cyclique pour une marche
    // d'une personne qui boite par exemple)
    int finSupportPremierPied=20;

	// Le premier qui va progresser le permier vers l'avant n'est pas le premier pied de support
    float progressionPied1 = 0;
    float progressionPied2 = 0;

	for (int f=0; f<getNbFrame() ; f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPremierPied, true, NULL, true);
		Point3D * posPied2 = getPositionJointByNameInFrame(f, nomDeuxiemePied, true, NULL, true);

		//if (!posPied || !posPied2) return;

		// Additionne la distance de progression
		progressionPied1 += posPied->getZ();
		progressionPied2 += posPied2->getZ();
		//progressionPied1 += posPiedSuiv->getZ() - posPied->getZ();
		//progressionPied2 += posPied2Suiv->getZ() - posPied2->getZ();

        if (f>frameDebutRecherche & f<frameFinRecherche & fabs(posPied->getZ()) + fabs(posPied2->getZ()) > ecartementMax -0.2) {

            finSupportPremierPied = f;
            break;
        }


    }




    //if (finSupportPremierPied == 0)
    //    finSupportPremierPied = (getNbFrame()-1)/2;

    // On fixe alors le début et la fin de la phase de support du pied1
    if (progressionPied1 < progressionPied2) {
        // Alors le pied2 est le premier pied de support
    	this->frameDebutSupport = finSupportPremierPied;
    	this->frameFinSupport = 1;
    } else {
        // Sinon c'est le pied 1
    	this->frameDebutSupport = 1;
    	this->frameFinSupport = finSupportPremierPied;
    }
    */

    // Bon finalement je zappe, on prend la moitié et c tout
    this->frameDebutSupport = 1;
    this->frameFinSupport = (getNbFrame()-1)/2;
}

void Bvh::rendreCyclique(string  nomPremierPied, string  nomDeuxiemePied) {

	// Etape 1: détecter un cycle de marche (pose d'un pied, puis du second)

	// Variation d'altitude autorisée du pied durant la phase de support
	const float epsilonAltitude = 0.2;
	const float epsilonVitesse = 0.05;

	// L'altitude min du pied
	float minYpied1 = FLT_MAX;
	float minYpied2 = FLT_MAX;

	// Premiere passe: recherche de l'altitude min du pied
	for (int f=0; f<getNbFrame(); f++) {
		// Recupere la position du pied
		Point3D * posPied = getPositionJointByNameInFrame(f, nomPremierPied, true);
		Point3D * posPied2 = getPositionJointByNameInFrame(f, nomDeuxiemePied, true);
		if (!posPied || !posPied2) return;

		// Sauvegarde la plus faible altitude du pied
		if (posPied->getY() < minYpied1) {
			minYpied1 = posPied->getY();

		}
		if (posPied2->getY() < minYpied2) {
			minYpied2 = posPied2->getY();

		}

		delete posPied;
        delete posPied2;
	}


	int numeroFrame = 1; // On part à la frame 1 pour pouvoir calculer la vitesse
	bool pied1Last = true; // On considere à tort ou a raison que le pied1 touche le sol en premier
	int tabFrameCle[4]; // Tableau contenant les indexs des frames clés
	int indexTabFrameCle = 0; // Variable d'index du tableau ci-dessus

	// Pour les 4 frames clés que l'on veut trouver:
	for (int k=0; k<4; k++) {
		// On recherche la pose d'un des 2 pieds
		while (numeroFrame < getNbFrame()) {
			// Recupere la position du pied
			Point3D * posPied = getPositionJointByNameInFrame(numeroFrame, nomPremierPied, true);
			Point3D * posPiedOld = getPositionJointByNameInFrame(numeroFrame-1, nomPremierPied, true);
			Point3D * posPied2 = getPositionJointByNameInFrame(numeroFrame, nomDeuxiemePied, true);
			Point3D * posPied2Old = getPositionJointByNameInFrame(numeroFrame-1, nomDeuxiemePied, true);


			// Lorsque que l'altitude de la phase de support est quasiment atteinte
			// et vitesse quasi nulle
			if ((posPied->getY() - minYpied1 <= epsilonAltitude) & (posPied->getY() - minYpied1>=0)
				& (posPied->distanceVers(*posPiedOld)<= epsilonVitesse) & (!pied1Last) ) {
				frameDebutSupport = numeroFrame;
				tabFrameCle[indexTabFrameCle] = numeroFrame;
				pied1Last = true;
				break;
			} else if ((posPied2->getY() - minYpied2 <= epsilonAltitude) & (posPied2->getY() - minYpied2>=0)
				& (posPied2->distanceVers(*posPied2Old)<= epsilonVitesse) & (pied1Last) ) {
				frameDebutSupport = numeroFrame;
				tabFrameCle[indexTabFrameCle] = numeroFrame;
				pied1Last = false;
				break;
			}
			delete posPied;
            delete posPied2;
            delete posPiedOld;
            delete posPied2Old;

			// Frame suivante
			numeroFrame++;
		}

		indexTabFrameCle++;
		numeroFrame++;

		// On attends qu'un des deux pieds se relève
		while (numeroFrame < getNbFrame()) {
			// Recupere la position du pied
			Point3D * posPied = getPositionJointByNameInFrame(numeroFrame, nomPremierPied, true);
			Point3D * posPiedOld = getPositionJointByNameInFrame(numeroFrame-1, nomPremierPied, true);
			Point3D * posPied2 = getPositionJointByNameInFrame(numeroFrame, nomDeuxiemePied, true);
			Point3D * posPied2Old = getPositionJointByNameInFrame(numeroFrame-1, nomDeuxiemePied, true);


			// Selon la situation, on teste si un des 2 pieds se relève
			if ((posPied->getY() - minYpied1 > epsilonAltitude)
				& (posPied->distanceVers(*posPiedOld)> epsilonVitesse)
				& (!pied1Last)) {

				break;
			} else if ((posPied2->getY() - minYpied2 > epsilonAltitude)
				& (posPied2->distanceVers(*posPied2Old)> epsilonVitesse)
				& (pied1Last)) {
				break;
			}
			delete posPied;
            delete posPied2;
            delete posPiedOld;
            delete posPied2Old;

			// Frame suivante
			numeroFrame++;
		}
		numeroFrame++;

	}



	// A ce stade, notre cycle de marche non corrigé se situe entre les frames
	// tabFrameCle[1] et tabFrameCle[3]. La frame tabFrameCle[2] correspond à
	// la pose du pied intermédiaire (autre que celui de départ)
	// On ignore volontairement tabFrameCle[0] car la première pose du pied
	// peut correspondre au passage d'un état statique à la marche
	// ce qui donne un mauvais résultat pour produire un cycle


	// Etape 2: Extraire le cycle non corrigé de l'animation initiale
	alignerAnimationSensDeMarche();
	fixerAnimation();

    mocapData->cropData(tabFrameCle[1]+1, tabFrameCle[3]-1);


	// Etape 3: mélange entre le début et la fin de l'animation pour assurer
	// la transition cyclique

	// On commence le mélange sur une portion égale a 40% de la fin de l'animation
	int frameDebutMelange = (int)(0.6 * getNbFrame());

	// Mélange degressif de la fin vers frameDebutMelange
	for (numeroFrame=getNbFrame()-1; numeroFrame>=frameDebutMelange; numeroFrame--) {
		// Poids du mélange
		float poids = 0.8*(float)(numeroFrame-frameDebutMelange) / (getNbFrame()-frameDebutMelange-1.);
		//float poids = (float)(numeroFrame*numeroFrame-frameDebutMelange*frameDebutMelange)
		//			/ ((nframes-1.)*(nframes-1.) -frameDebutMelange*frameDebutMelange);

		// Pour chaque paramètre de la frame courante
		/*
		for (int i=0; i<getNchannels(); i++) {
			//int indexCourant = numeroFrame*nchannels+i;
			// Avant d'effectuer le mélange, on vérifie qu'il ne s'agit
			// pas de 2 angles <-90° et >90° dont le mélange donnerait une mauvaise valeur
			// Exemple: -179 + 179° donnerait 0° alors que le résultat doit être 180°
			//float donnee1 = data[indexCourant];
			float donnee1 = mocapData->getRawData(numeroFrame, i );

			//float donnee2 = data[(indexCourant + nchannels)%(nframes*nchannels)];
			//float donnee2 = data[((numeroFrame+1)%nframes)*nchannels+i];

			float donnee2 = mocapData->getRawData((numeroFrame+1)%getNbFrame(), i);
			if ( ((donnee1<-90)&&(donnee2>90)) || ((donnee1>90)&&(donnee2<-90)) ) {


				if (fabs(donnee1)> fabs(donnee2)) {
					if (donnee1>0)
						donnee1 -=360;
					else
						donnee1 +=360;
				} else {
					if (donnee2>0)
						donnee2 -=360;
					else
						donnee2 +=360;
				}


			}

			// On effectue le mélange avec la frame qui la succede
			//data[indexCourant] = ((1.-poids) * donnee1 + poids * donnee2 );
			mocapData->setRawData((1.-poids) * donnee1 + (poids * donnee2), numeroFrame, i);

			// On s'assure que la valeur en sortie est dans [-180;180]
			//data[indexCourant] = modulo(data[indexCourant],-180,180);
			mocapData->setRawData( modulo( mocapData->getRawData(numeroFrame, i ) ,-180,180), numeroFrame, i);
		}
		*/
		// Pour chaque joint de la frame
		SkeletalData::JointListIndexedByName listeJoints =  skeletalData->getListeJointsByName();
		for (SkeletalData::JointListIndexedByName::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
            Joint * jointCourant = it->second;

            char axes[] = {'X', 'Y', 'Z'};

            // - Mélange des 3 translations si elle existent
            if (jointCourant->isTransJoint()) {

                for (int k=0; k<3; k++) {
                    float trans1 = mocapData->getTrans(numeroFrame, axes[k], jointCourant);
                    float trans2 = mocapData->getTrans((numeroFrame+1)%getNbFrame(), axes[k], jointCourant);

                    // Mélange
                    mocapData->setTrans((1.-poids) * trans1 + poids * trans2, numeroFrame, axes[k], jointCourant);
                }
            }

            // - Mélange des 3 rotations
            for (int k=0; k<3; k++) {
                float rot1 = mocapData->getRot(numeroFrame, axes[k], jointCourant);
                float rot2 = mocapData->getRot((numeroFrame+1)%getNbFrame(), axes[k], jointCourant);

                // Avant d'effectuer le mélange, on vérifie qu'il ne s'agit
    			// pas de 2 angles <-90° et >90° dont le mélange donnerait une mauvaise valeur
    			// Exemple: -179 + 179° donnerait 0° alors que le résultat doit être 180°
    			if ( ((rot1<-90)&&(rot2>90)) || ((rot1>90)&&(rot2<-90)) ) {


    				if (fabs(rot1)> fabs(rot2)) {
    					if (rot1>0)
    						rot1 -=360;
    					else
    						rot1 +=360;
    				} else {
    					if (rot2>0)
    						rot2 -=360;
    					else
    						rot2 +=360;
    				}


    			}

    			// On effectue le mélange avec la frame qui suit
    			float rotMelange = (1.-poids) * rot1 + (poids * rot2);

    			// Stockage de la valeur
    			mocapData->setRot( modulo(rotMelange, -180,180) , numeroFrame, axes[k], jointCourant);

            }
        }

	}

}

// Restreint l'animation entre deux frames
/*
void Bvh::cropAnimation(int frame1, int frame2) {

	// Test de validité
	if ((frame1 >= frame2) || (frame1 <0) || (frame2>=nframes))
		return;

	// On déplace le pointeur data
	data += frame1*nchannels;

	// On déplace la fin des données
	nframes = frame2-frame1;

}
*/

//////////////////////////////////////////////////
// PROC:   Bvh::getAngleOrientation()
// DOES:   Renvoie la rotation globale de l'animation
//	       par rapport à l'axe Y
//////////////////////////////////////////////////
float Bvh::getAngleOrientation()
{
	//return data[root->getOffsetRotChannel('Y')];
    return mocapData->getRot(0,'Y',getRootSqueleton());
}

//////////////////////////////////////////////////
// PROC:   Bvh::turnAroundY()
// DOES:   change l'orientation de l'animation par rotation autour de l'axe Y (la verticale)
//////////////////////////////////////////////////
void Bvh::turnAroundY(float angle) {
	// On fixe le départ de l'animation en (0,?,0);
	centrerAnimation();

	// Correction de la trajectoire
	for (int f=0; f<getNbFrame(); f++) {

		// Etape 0: On recalcule les 3 angles d'euler de la racine en ayant effectué
		// auparavant une rotation sur l'axe Y (la rotation devient P'=YXZY' P)

		// Transformation effective d'un point par la matrice de vue d'opengl
		// pour récuperer les coordonnées du point du terminal
		// Ensemble des transformations
		//float rotZ = getRotJoint(f, root,  'Z');
		//float rotX = getRotJoint(f, root,  'X');
		//float rotY = getRotJoint(f, root,  'Y');
	    float rotZ = mocapData->getRot(f, 'Z', getRootSqueleton());
	    float rotX = mocapData->getRot(f, 'X', getRootSqueleton());
	    float rotY = mocapData->getRot(f, 'Y', getRootSqueleton());

		glPushMatrix();
		glLoadIdentity();
		glRotatef(-angle * 180./M_PI, 0., 1., 0.);
		glRotatef(rotZ, 0., 0., 1.);
		glRotatef(rotX, 1., 0., 0.);
		glRotatef(rotY, 0., 1., 0.);

		// Recupere la matrice de rotation associé à la vue courante
		GLfloat *buf2 = new GLfloat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX, buf2);

		// Recupère les 3 nouveau angles d'euler associés
		float a = asinf(buf2[6]);
		float b = atan2(buf2[8], buf2[10]);
		float c = atan2(-buf2[4], buf2[5]);

		// Libération mémoire et restauration
		delete buf2;
		glPopMatrix();

		/*
		float cosX = cosf(rotX);
		float sinX = sinf(rotX);
		float cosY = cosf(rotY);
		float sinY = sinf(rotY);
		float cosZ = cosf(rotZ);
		float sinZ = sinf(rotZ);
		float cosA = cosf(angle);
		float sinA = sinf(angle);


		float xp = asinf(sinX*cosA + cosX*sinZ*sinA);
		float zp = atanf((cosX*sinZ*cosA-sinX*sinA)/(cosX*cosZ));
		float yp = atanf( (cosY*cosZ*sinA+sinY*(cosX*cosA-sinX*sinZ*sinA))
						 / (sinY*cosZ*sinA-cosY*(cosX*cosA-sinX*sinZ*sinA)));
		*/
		/*
		float xp = asinf(buf[2][1]);
		float zp = atanf((buf[0][1])/(buf[1][1]));
		float yp = atanf(- buf[2][0] / buf[2][2]);
*/

		// Etape 1: On corrige l'orientation de la racine
		setRotJoint(a * 180./M_PI, f, getRootSqueleton(), 'X');
		//setRotJoint(b * 180./M_PI, f, root, 'Y');
		//setRotJoint(c * 180./M_PI, f, root, 'Z');

		mocapData->setRot(a * 180./M_PI, f, 'X', getRootSqueleton());
		mocapData->setRot(b * 180./M_PI, f, 'Y', getRootSqueleton());
		mocapData->setRot(c * 180./M_PI, f, 'Z', getRootSqueleton());

		//data[f * nchannels + root->getOffsetRotChannel('X')] = a * 180./M_PI;
		//data[f * nchannels + root->getOffsetRotChannel('Y')] = b * 180./M_PI;
		//data[f * nchannels + root->getOffsetRotChannel('Z')] = c * 180./M_PI;


		// Etape 2: On corrige la position de la racine

		//float Xroot = data[f * nchannels + root->getOffsetTransChannel('X')];
		//float Zroot = data[f * nchannels + root->getOffsetTransChannel('Z')];
		float Xroot = mocapData->getTrans(f, 'X', getRootSqueleton());
		float Zroot = mocapData->getTrans(f, 'Z', getRootSqueleton());
		// Translation pour départ en (0,0)
		//Xroot -= XrootInitO;
		//Zroot -= ZrootInitO;
		// Rotation des coordonnées de la racine dans le plan XZ (axe Y)

		//data[f * nchannels + root->getOffsetTransChannel('X')] =
		//	(Xroot * cosf(angle) - Zroot*sinf(angle));
		//data[f * nchannels + root->getOffsetTransChannel('Z')] =
		//	(Xroot * sinf(angle) + Zroot*cosf(angle));
        mocapData->setTrans(Xroot * cosf(angle) - Zroot*sinf(angle), f, 'X', getRootSqueleton());
        mocapData->setTrans(Xroot * sinf(angle) + Zroot*cosf(angle), f, 'Z', getRootSqueleton());


	}

}
//////////////////////////////////////////////////
// PROC:   Bvh::alignerAnimationSensDeMarche()
// DOES:   Fixe l'orientation de la racine en alignement selon un axe
//////////////////////////////////////////////////
void Bvh::alignerAnimationSensDeMarche()
{
	// Calcul de la droite de régression (méthodes des moindres carrés)
	// Le nuage de point est l'ensemble des positions de la racine au cours du temps
	// On obtiendra en sortie la direction globale de l'animation
	// Z = bo + b1 X
	float b0 = 0, b1 = 0, sommeXk = 0, sommeXkC = 0, sommeYk = 0, sommeXkYk = 0;
	for (int k=0;k<getNbFrame();k++) {
	  //float Xroot = data[k * nchannels + root->getOffsetTransChannel('X')];
   	  //float Zroot = data[k * nchannels + root->getOffsetTransChannel('Z')];
	  float Xroot = mocapData->getTrans(k, 'X', getRootSqueleton());
	  float Zroot = mocapData->getTrans(k, 'Z', getRootSqueleton());
      sommeXk += Xroot;
	  sommeYk += Zroot;
	  sommeXkC += Xroot*Xroot;
	  sommeXkYk += Xroot*Zroot;
	}
	b0 = (sommeXkC*sommeYk - sommeXk * sommeXkYk)
		/ (getNbFrame() * sommeXkC - sommeXk * sommeXk);
	b1 = (getNbFrame() * sommeXkYk - sommeXk * sommeYk)
		/ (getNbFrame() * sommeXkC - sommeXk * sommeXk);

	// Recherche de l'angle de la trajectoire sur la totalité du chemin

	// On recupère la positon de la racine en début et en fin de parcours
	//float XrootInitO = data[root->getOffsetTransChannel('X')];
    float XrootInitO = mocapData->getTrans(0, 'X', getRootSqueleton());
	//float ZrootInitO = data[root->getOffsetTransChannel('Z')];

    //float XrootTermO = data[(nframes-1) * nchannels + root->getOffsetTransChannel('X')];
    float XrootTermO = mocapData->getTrans(getNbFrame()-1, 'X', getRootSqueleton());
	//float ZrootTermO = (nframes-1) * nchannels + root->getOffsetTransChannel('Z')];

	// On calcule la position en début et en fin de parcours sur la droite de régression
	float XrootInit = XrootInitO;
	//float ZrootInit = b0 + b1*XrootInit;
	float XrootTerm = XrootTermO;
	//float ZrootTerm = b0 + b1*XrootTerm;

	// Angle de la trajectoire par rapport à l'axe X
	//float angle = acosf(fabs(ZrootInit - ZrootTerm)
	//			/ sqrt((ZrootTerm - ZrootInit) * (ZrootTerm - ZrootInit)
	//				+ (XrootTerm - XrootInit) * (XrootTerm - XrootInit) ));
	float angle =  - atanf(b1);


	// On lève l'indétermination de la valeur renvoyé par la fonction tangente inverse
	// en vérifiant que la progression sur l'axe X se fera dans le sens croissant
	if (XrootInit > XrootTerm)
		angle = M_PI + angle;

	//angle = modulo(angle, -M_PI, M_PI);

	// Modification de la trajectoire
	turnAroundY(angle);


}

//////////////////////////////////////////////////
// PROC:   Bvh::setScale()
// DOES:   Change l'echelle de visualisation
//////////////////////////////////////////////////

void Bvh::setEchelle(float echelle)
{
	//drawScale = echelle;
	if (mocapData!=NULL) {
    	//actorRender->setEchelle(echelle);
    	mocapData->setEchelle(echelle);
    	if (skinData != NULL)
            skinData->getReferencePosData()->setEchelle(echelle);
    }
}

float Bvh::getEchelle()
{
	return mocapData->getEchelle();
}


//////////////////////////////////////////////////
// PROC:   Bvh::getDureeFrame()
// DOES:   Renvoie le temps entre deux frame
//////////////////////////////////////////////////
float Bvh::getDureeFrame()
{
	return mocapData->getDureeFrame();
}

//////////////////////////////////////////////////
// PROC:   Bvh::drawFrame(int f)
// DOES:   draws the given frame number
//////////////////////////////////////////////////
/*
void Bvh::drawFrame(int f)
{
    if (actorRender != NULL)
        actorRender->drawFrame(f);

}
*/

//////////////////////////////////////////////////
// PROC:   Bvh::printFrame(int f)
// DOES:   print contents of the given frame
//////////////////////////////////////////////////
/*
void Bvh::printFrame(int f)
{

	int indx = nchannels*f;
	for (int c=0; c<nchannels; c++)
		printf("%f ",data[indx++]);
	printf("\n");

}
*/

//////////////////////////////////////////////////
// PROC:   Bvh::getNbFrame()()
// DOES:   Renvoie le nombre total de frames de l'animation
//////////////////////////////////////////////////
int Bvh::getNbFrame() {
    return mocapData->getNbFrames();
	//return nframes;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getNchannels()
// DOES:   Renvoie le nombre total de joint
//////////////////////////////////////////////////
int Bvh::getNchannels() {
	return mocapData->getNChannels();
}

//////////////////////////////////////////////////
// PROC:   Bvh::getTransJoint()
// DOES:   renvoie la valeur de translation d'un joint selon un axe pour une frame
//////////////////////////////////////////////////
float Bvh::getTransJoint(int numeroFrame, Joint * joint, char axe) {

    return mocapData->getTrans(numeroFrame, axe, joint);
	//return data[numeroFrame * nchannels + joint->getOffsetRotChannel(axe)];
}

//////////////////////////////////////////////////
// PROC:   Bvh::getRotJoint()
// DOES:   renvoie la valeur de rotation d'un joint selon un axe pour une frame
//////////////////////////////////////////////////
float Bvh::getRotJoint(int numeroFrame, Joint * joint, char axe) {

    return mocapData->getRot(numeroFrame, axe, joint);
	//return data[numeroFrame * nchannels + joint->getOffsetRotChannel(axe)];
}

//////////////////////////////////////////////////
// PROC:   Bvh::setRotJoint()
// DOES:   fixe la valeur de rotation d'un joint selon un axe pour une frame
//////////////////////////////////////////////////
void Bvh::setRotJoint(float valeur, int numeroFrame, Joint * joint, char axe) {

    mocapData->setRot(valeur, numeroFrame, axe, joint);
	//data[numeroFrame * nchannels + joint->getOffsetRotChannel(axe)] = valeur;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getPositionTerminauxAnimation()
// DOES:   Renvoie la liste complete des positions des terminaux d'une frame
//////////////////////////////////////////////////
/*
list<Point3D*>  * Bvh::getPositionTerminauxAnimation(int numeroFrame) {
	glPushMatrix();
	glLoadIdentity();
	glScalef(drawScale,drawScale,drawScale);
    // Ici on tient compte de l'orientation de l'animation et on la corrige
	glRotatef(getAngleOrientation() , 0.0, 1.0, 0.0);

	float *frameData = &data[numeroFrame*nchannels];

	list<Point3D*> * liste = getPositionTerminaux(frameData, root);
	glPopMatrix();
	return liste;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getPositionTerminaux
// DOES:   Renvoie la liste complete des positions des terminaux à partir d'un joint de référence
//////////////////////////////////////////////////
list<Point3D*> * Bvh::getPositionTerminaux(float* frameData, Joint * referenceJoint) {

	glPushMatrix();

	// Pour sauver les résultats
	list<Point3D> *listeResult = new list<Point3D>();

	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();

	// On applique les transformation du noeud
	referenceJoint->appliquerTransformation(frameData);

	// Si aucun enfant, alors c'est un terminal)
	if (referenceJoint->isTerminal()) {

		// On translate sur la fin réelle du terminal
		glTranslatef(referenceJoint->getOffsetEndPos('X'),
					referenceJoint->getOffsetEndPos('Y'),
					referenceJoint->getOffsetEndPos('Z'));


		// Transformation effective d'un point par la matrice de vue d'opengl
		// pour récuperer les coordonnées du point du terminal
		Point3D p = Point3D(0,0,0);
		GLfloat * buf = new GLfloat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,buf);
		Point3D result = p->multVecteurMatrice(buf);

		// Verification
		glPushMatrix();
		glLoadIdentity();
		glTranslatef(result->getX(),result->getY(),result->getZ());
		//glColor3f(0,1,1);
		//glutSolidSphere(sphereSize,20,10);
		glPopMatrix();


		// On ajoute la position du terminal à la liste
		listeResult->push_back(result);

	} else {

		// Sinon c'est un noeud
		// on parcours ses enfants pour recommencer le processus
		list<Joint*>::iterator it = listeCourante->begin();
		while (it != listeCourante->end()) {
			Joint * unenfant = *(it++);
			listeResult->merge(*getPositionTerminaux(frameData, unenfant));
		}
	}

	glPopMatrix();
	return listeResult;

		return NULL;
}
*/

//////////////////////////////////////////////////
// PROC:   Bvh::getPositionTerminauxAnimation()
// DOES:   renvoie la position d'un joint par son nom dans une frame (on part de la racine)
//		   EndSite indique si on doit renvoyer la position du terminal (EndSite section)
//		   ou seulement la position du joint.
//////////////////////////////////////////////////
Point3D * Bvh::getPositionJointByNameInFrame(int numeroFrame, string nomJoint, bool getEndSite, float *matRot, bool withtoutScale) {
    return mocapManipulator->getPositionJointByNameInFrame(numeroFrame, nomJoint, getEndSite, matRot, withtoutScale);

}
//////////////////////////////////////////////////
// PROC:   Bvh::getPositionJointByName()
// DOES:   renvoie la position d'un joint par son nom à partir d'un joint de reference
//////////////////////////////////////////////////

//Point3D * Bvh::getPositionJointByName(int numeroFrame, string nomJoint, bool getEndSite, float *matRot) {

    //return mocapManipulator->getPositionJointByName(numeroFrame, nomJoint, getEndSite, matRot);
    /*
	glPushMatrix();

	// Le resultat
	Point3D * resultat = NULL;

	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();

	// On applique les transformation du noeud
	//referenceJoint->appliquerTransformation(frameData);
    appliquerTransformation(numeroFrame, referenceJoint);

	// Si le nom du joint correspond, on arrete la recherche
	if (referenceJoint->getName() == nomJoint) {

		// Cas particulier pour un terminal
		if (referenceJoint->isTerminal() && getEndSite) {
			// On translate sur la fin réelle du terminal
			glTranslatef(referenceJoint->getOffsetEndPos('X'),
						referenceJoint->getOffsetEndPos('Y'),
						referenceJoint->getOffsetEndPos('Z'));
		}

		// Transformation effective d'un point par la matrice de vue d'opengl
		// pour récuperer les coordonnées du point du terminal
		Point3D ptemp = Point3D(0,0,0);
		GLfloat * buf = new GLfloat[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,buf);
		ptemp.multVecteurMatrice(buf);

		// Sauvegarde la matrice de rotation trouvée si nécessaire
		if (matRot !=  NULL) {
			memcpy(matRot, buf, sizeof(GLfloat)*16);
		}
		delete buf;

		// On renvoie le résultat
		resultat = new Point3D(ptemp);

	} else {

		// Sinon c'est un noeud
		// on parcours ses enfants pour recommencer le processus
		list<Joint*>::iterator it = listeCourante->begin();
		while (it != listeCourante->end()) {
			Joint * unenfant = *(it++);
			Point3D * result = getPositionJointByName(numeroFrame, nomJoint, unenfant, getEndSite, matRot);
			if (result) {
				resultat = result;
				break;
			}
		}
	}

	glPopMatrix();


	// Renvoie le resultat
	return resultat;

    */
//}

//////////////////////////////////////////////////
// PROC:   Bvh::getJointByName()
//////////////////////////////////////////////////
Joint * Bvh::getJointByName(string nomJoint, Joint * referenceJoint) {

    return skeletalData->getJointByName(nomJoint);

}




//////////////////////////////////////////////////
// PROC:   Bvh::clearOptimisation()
// DOES:   Efface les informations d'optimisations
//////////////////////////////////////////////////
void Bvh::clearOptimisation() {

	// Reinitialise les anti-glisseurs de pieds
	if (bvhGlissementPied1 && bvhGlissementPied2) {
		bvhGlissementPied1->init();
		bvhGlissementPied2->init();
	}
}

//////////////////////////////////////////////////
// PROC:   Bvh::getEndJointName()
// DOES:   Renvoie le nom du joint terminal en partant d'un joint d'une chaine
//////////////////////////////////////////////////
string Bvh::getEndJointName(string nomJoint) {
	Joint * terminal = getJointByName(nomJoint);

	// Ici on considére que la chaine est simple (sans dédoublement)
	// Recupere le terminal de la chaine et compte la longueur de la chaine
	// On enregistre par la meme occasion la chaine dans une liste pour un
	// parcours en sens inverse ensuite
	//list<Joint*> * chaine = new list<Joint*>();
	while (!terminal->isTerminal()) {
		list<Joint*> * listeCourante = terminal->getListeEnfants();
		terminal = *(listeCourante->begin());
	}

	return terminal->getName();
}


//////////////////////////////////////////////////
// PROC:   Bvh::getChaineJointVersTerminal()
// DOES:   Renvoie la chaine entier vers un terminal à partir d'un joint
//////////////////////////////////////////////////
list<Joint*> * Bvh::getChaineJointVersTerminal(string nomJoint) {

	// Valeur par defaut du terminal
	Joint * terminal = getJointByName(nomJoint);

	// Ici on considére que la chaine est simple (sans dédoublement)
	// Recupere le terminal de la chaine et compte la longueur de la chaine
	// On enregistre par la meme occasion la chaine dans une liste pour un
	// parcours en sens inverse ensuite
	list<Joint*> * chaine = new list<Joint*>();
	while (!terminal->isTerminal()) {
		chaine->push_front(terminal);
		list<Joint*> * listeCourante = terminal->getListeEnfants();
		terminal = *(listeCourante->begin());
	}
	chaine->push_front(terminal);

	// Retourne le résultat
	return chaine;

}

//////////////////////////////////////////////////
// PROC:   Bvh::majListeJoints(JointListIndexedByName & listeJoints, Joint * referenceJoint)
// DOES:   Met à jour / construit la liste des joint "listeJoints"
//////////////////////////////////////////////////
/*
void Bvh::majListeJoints(JointListIndexedByName & listeJoints, Joint * referenceJoint) {

    // Ajoute le joint de référence dans la liste
    listeJoints[referenceJoint->getName()] = referenceJoint;

	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();
	// on parcours ses enfants pour recommencer le processus
	for (list<Joint*>::iterator it = listeCourante->begin(); it != listeCourante->end(); it++) {
		// Recupération de l'enfant
		Joint * unEnfant = *it;

		// Insertion de l'enfant dans la liste
		//listeJoints[unEnfant->getName()] = unEnfant;

		// Appel récursif
		majListeJoints(listeJoints, unEnfant);
	}

}

//////////////////////////////////////////////////
// PROC:   Bvh::majListeJoints()
// DOES:   Met à jour / construit la liste des joint "listeJoints"
//////////////////////////////////////////////////
Bvh::JointListIndexedByName Bvh::majListeJoints() {

    // Vide la liste
    //listeJoints.clear();

    JointListIndexedByName listeJoints;

    // Commence la mise à jour par la racine
    majListeJoints(listeJoints, root);

    return listeJoints;
}
*/
//////////////////////////////////////////////////
// PROC:   Bvh::getListAllJoint(Joint * referenceJoint)
// DOES:   Utilisée pour la récursivité de la méthode publique getListAllJoint()
//////////////////////////////////////////////////
map<string, void*> * Bvh::getListAllJoint(Joint * referenceJoint) {
	// Création de la liste résultat
	map<string, void*>  * listeResult = new map<string, void*>();


	// Cas particulier de la feuille
	if (referenceJoint->isTerminal()) {
		(*listeResult)["terminal"] =  NULL;
		return listeResult;
	}

	// Récupere la liste des enfants du joint de départ (la référence passée en paramètre)
	list<Joint*> * listeCourante = referenceJoint->getListeEnfants();
	// on parcours ses enfants pour recommencer le processus
	for (list<Joint*>::iterator it = listeCourante->begin(); it != listeCourante->end(); it++) {
		// Recupération de l'enfant
		Joint * unenfant = *it;

		// Appel de la fonction sur le sous branche de l'enfant
		// et recupération de la liste des noeud
		map<string, void*> * listeNoeud = getListAllJoint(unenfant);

		// Insertion de cette branche dans l'arbre résultat
		(*listeResult)[unenfant->getName()] = listeNoeud;

	}
	//listeResult.push_back(listeNoeud);
	return listeResult;
}

//////////////////////////////////////////////////
// PROC:   Bvh::getListAllJoint()
// DOES:   Renvoie une stl::map d'association clé-valeur contenant l'arbre du squelette du personnage
//			clé = nom du joint, valeur = pointeur sur une valeur de type map
//			si clé = NULL, alors il s'agit d'une feuille
//////////////////////////////////////////////////
map<string, void*> * Bvh::getListAllJoint() {

	// On lance l'appel récursif sur la liste des joints
	// On n'oublie pas d'inclure la racine
	map<string, void*>  * listeResult = new map<string, void*>();
	(*listeResult)[getRootSqueleton()->getName()] = getListAllJoint(getRootSqueleton());
	return listeResult;

}


//////////////////////////////////////////////////
// PROC:   Bvh::appliquerIkCcd()
// DOES:   Ik par ccd sur une chaine simple (pas de dédoublement)
//		   Fournir le numéro de la frame à traiter, le nom du joint désignant le début de la chaine,
//		   la position voulue pour la fin de cette chaine, et le nombre d'itération maximum
//		   (pour borner l'algorithme)
//////////////////////////////////////////////////
void Bvh::appliquerIkCcd(int numeroFrame, string nomJoint, Point3D positionVoulue, int nbIterationsMax) {
	list<Joint*> * chaine = getChaineJointVersTerminal(nomJoint);
	appliquerIkCcd(numeroFrame, chaine, positionVoulue, nbIterationsMax);
}

void Bvh::appliquerIkCcd(int numeroFrame, list<Joint*> * chaine, Point3D positionVoulue, int nbIterationsMax) {
	// Constantes de l'algortihme
	const float epsilon = 0.05;
	const float alpha = 1;
	const float w0 = 1;

	if (!chaine) {
		return;
	}

	// On récupère le terminal de la chaine
	Joint * terminal = *(chaine->begin());
	//chaine->pop_front();

    // Les matrices de rotations
	float * matRotTerm = new float[16];
	float * matRotDepart = new float[16];

	// On repete la passe d'optimisation autant de fois que nécessaire
	for (int precision=0; precision<nbIterationsMax; precision++) {



		// Une passe: parcours entier de la chaine et optimisation de chaque rotation
		// du terminal vers la base
		//for(list<Joint*>::iterator it = ++chaine->begin(); it != chaine->end(); it++) {
		for(list<Joint*>::iterator it = --chaine->end(); it != chaine->begin(); it--) { // de la base vers le terminal
			// Recupere le joint courant
			Joint * curseur = *(it);

			// Calcul de la positon du terminal et recupération de la matrice de rotation
			// Pour plus de précision il faut déplacer ce bloc dans la boucle de parcours de la chaine (fait)

			Point3D * posTerm = getPositionJointByNameInFrame(numeroFrame, terminal->getName(), true, matRotTerm);

			// On arrete l'optimisation lorsqu'on est pres du but
			if (posTerm->distanceVers(positionVoulue)<epsilon) {
				//printf("break\n");
				return;
			}


			// Position de la racine en cours et matrice de rotation
			Point3D * posDepart = getPositionJointByNameInFrame(numeroFrame, curseur->getName(), true, matRotDepart);

			// Calcul de Pic: vecteur de distance entre départ et terminal
			Point3D Pic = Point3D((posTerm->getX() - posDepart->getX()),
									(posTerm->getY() - posDepart->getY()),
									(posTerm->getZ() - posDepart->getZ()));

			// Calcul de Pid: vecteur de distance entre départ et la position voulue
			Point3D Pid = Point3D((positionVoulue.getX() - posDepart->getX()),
									(positionVoulue.getY() - posDepart->getY()),
									(positionVoulue.getZ() - posDepart->getZ()));



			// Mise à jour des paramètres des coefficients k
			float ro = min(Pid.norme(), Pic.norme()) / max(Pid.norme(), Pic.norme());
			float wp = alpha*(1+ro);


			// Orientation du terminal (uc) et orientation voulue en fin d'optimisation (ud)
			// On applique la matrice de vue du terminal à X Y Z
			// puis on soustraint la position du terminal, pour obtenir le nouveau repere X' Y' Z' bien orienté
			// selon les 3 degrés de liberté du joint
			Point3D axe1 = Point3D(1,0,0);
			axe1.multVecteurMatrice(matRotTerm);
			Point3D axe2 = Point3D(0,1,0);
			axe2.multVecteurMatrice(matRotTerm);
			Point3D axe3 = Point3D(0,0,1);
			axe3.multVecteurMatrice(matRotTerm);
			Point3D uc[3] = {axe1, axe2, axe3};
			uc[0] = uc[0].add(posTerm->inverse());
			uc[1] = uc[1].add(posTerm->inverse());
			uc[2] = uc[2].add(posTerm->inverse());

			// On garde la meme orientation pour la position voulue
			Point3D ud[3] = {uc[0],uc[1],uc[2]};


			// Calcul des 3 axes (on applique la matrice de vue de départ à X Y Z)
			// puis on soustraint la position de départ, pour obtenir le nouveau repere X' Y' Z' bien orienté
			// selon les 3 degrés de liberté du joint
			Point3D axed1 = Point3D(1,0,0);
			axed1.multVecteurMatrice(matRotDepart);
			axed1 = axed1.add(posDepart->inverse());

			Point3D axed2 = Point3D(0,1,0);
			axed2.multVecteurMatrice(matRotDepart);
			axed2 = axed2.add(posDepart->inverse());

			Point3D axed3 = Point3D(0,0,1);
			axed3.multVecteurMatrice(matRotDepart);
			axed3 = axed3.add(posDepart->inverse());


			// K2
			// NOTE: le calcul de K2 ne dépend pas de l'axe courant (voir dans la boucle suivante)
			float k2 = 0;
			for (int j = 0; j<3; j++) {
				k2 += ud[j].produitf32(uc[j]);
			}
			k2 *= w0;
			k2 += wp * Pid.produitf32(Pic);

			// Determination de l'axe
			char axeLetter[] = {'X', 'Y', 'Z'};
			for (int axe = 0; axe <3; axe++) {

				// Optimisation du joint courant sur l'axe courant
				Point3D axis;

				switch (axe) {
					case 0: axis = axed1; break;
					case 1: axis = axed2; break;
					case 2: axis = axed3; break;
				}

				// K1
				float k1 = 0;
				for (int j = 0; j<3; j++) {
					k1 += (ud[j].produitf32(axis)) * (uc[j].produitf32(axis));
				}
				k1 *= w0;
				k1 += wp * Pid.produitf32(axis) * Pic.produitf32(axis);



				//K3
				float k3 = 0;
				Point3D temp =Point3D(0,0,0);
				for (int j = 0; j<3; j++) {
					temp = temp.add(uc[j].produitVectoriel(ud[j]));
				}
				temp = temp.mult(w0);
				temp = temp.add((Pic.produitVectoriel(Pid)).mult(wp));
				k3 = axis.produitf32(temp);

				//TETA
				//Le deplacement de l'angle optimisé
				float teta;
				if (k2-k1 != 0) {
					teta = atanf(k3/(k2-k1)) *57.29;//* 180/M_PI;
					//teta[axe-1] = atan2(k3, k2-k1);
				} else {
					// Normalement impossible, dans le doute, on ne change pas l'angle courant
					teta = 0;
				}

				//TEST: on a bien a = 0
				//float a = (k1-k2)*sinf(teta[axe-1])+k3*cosf(teta[axe-1]);
				//printf("test: %f\n", a);

				// Applique le changement de rotation sur le joint
				/*
				float rot;
				switch (axe) {
					case 1: rot= getRotJoint(numeroFrame, curseur, 'X');
							setRotJoint(fmod(teta + rot+180,360)-180, numeroFrame, curseur, 'X');
							break;
					case 2: rot= getRotJoint(numeroFrame, curseur, 'Y');
							setRotJoint(fmod(teta + rot+180,360)-180, numeroFrame, curseur, 'Y');
							break;
					case 3: rot= getRotJoint(numeroFrame, curseur, 'Z');
							setRotJoint(fmod(teta + rot+180,360)-180, numeroFrame, curseur, 'Z');
							break;
				}
				*/

				float rot= getRotJoint(numeroFrame, curseur, axeLetter[axe]);
				setRotJoint(fmod(teta + rot+180,360)-180, numeroFrame, curseur, axeLetter[axe]);




			}

			// Liberation mémoire
			/*
			delete matRotTerm;
			delete matRotDepart;
			delete posDepart;
			delete posTerm;
*/
		}


	}

	delete matRotTerm;
	delete matRotDepart;

	// Destruction de chaine temporaire
	//delete chaine;
}



/* Ancienne version pas du tout optimisée */
/*
void Bvh::appliquerIkCcd(int numeroFrame, string nomJoint, Point3D positionVoulue, int nbIterationsMax) {
	// Constantes de l'algortihme
	const float epsilon = 0.01;
	const float alpha = 1;
	const float w0 = 1;


	// Longueur de la chaine et terminal (avant calcul)
	//int longueurChaine = 0;
	Joint * terminal = getJointByName(nomJoint);

	// Ici on considére que la chaine est simple (sans dédoublement)
	// Recupere le terminal de la chaine et compte la longueur de la chaine
	// On enregistre par la meme occasion la chaine dans une liste pour un
	// parcours en sens inverse ensuite
	list<Joint*> * chaine = new list<Joint*>();
	while (!terminal->isTerminal()) {
		chaine->push_front(terminal);
		list<Joint*> * listeCourante = terminal->getListeEnfants();
		terminal = *(listeCourante->begin());
		//longueurChaine++;
	}
	//chaine->push_front(terminal); // => A DEBUGUER LE TRAITEMENT DU TERMINAL


	// On repete la passe d'optimisation autant de fois que nécessaire
	for (int precision=0; precision<nbIterationsMax; precision++) {

		// Une passe: parcours entier de la chaine et optimisation de chaque rotation
		// du terminal vers la base
		for(list<Joint*>::iterator it = chaine->begin(); it != chaine->end(); it++) {
			// Recupere le joint courant
			Joint * curseur = *(it);

			for (int axe = 1; axe <=3; axe++) {
				// Determination de l'axe

				// Calcul de la positon du terminal et recupération de la matrice de rotation
				float * matRotTerm = new float[16];
				Point3D * posTerm = getPositionJointByNameInFrame(numeroFrame, terminal->getName(), true, matRotTerm);

				// Optimisation: on arrete l'optimisation lorsqu'on est pres du but
				if (posTerm->distanceVers(positionVoulue)<epsilon) {
					//printf("break\n");
					return;
				}

				// Position de la racine en cours et matrice de rotation
				float * matRotDepart = new float[16];
				Point3D * posDepart = getPositionJointByNameInFrame(numeroFrame, curseur->getName(), false, matRotDepart);

				// Calcul de Pic: vecteur de distance entre départ et terminal
				Point3D Pic = Point3D((posTerm->getX() - posDepart->getX()),
										(posTerm->getY() - posDepart->getY()),
										(posTerm->getZ() - posDepart->getZ()));

				// Calcul de Pid: vecteur de distance entre départ et la position voulue
				Point3D Pid = Point3D((positionVoulue.getX() - posDepart->getX()),
										(positionVoulue.getY() - posDepart->getY()),
										(positionVoulue.getZ() - posDepart->getZ()));



				// Mise à jour des paramètres des coefficients k
				float ro = min(Pid.norme(), Pic.norme()) / max(Pid.norme(), Pic.norme());
				float wp = alpha*(1+ro);


				// Orientation du terminal (uc) et orientation voulue en fin d'optimisation (ud)
				// On applique la matrice de vue du terminal à X Y Z
				// puis on soustraint la position du terminal, pour obtenir le nouveau repere X' Y' Z' bien orienté
				// selon les 3 degrés de liberté du joint
				Point3D axe1 = Point3D(1,0,0);
				axe1.multVecteurMatrice(matRotTerm);
				Point3D axe2 = Point3D(0,1,0);
				axe2.multVecteurMatrice(matRotTerm);
				Point3D axe3 = Point3D(0,0,1);
				axe3.multVecteurMatrice(matRotTerm);
				Point3D uc[3] = {axe1, axe2, axe3};
				uc[0] = uc[0].add(posTerm->inverse());
				uc[1] = uc[1].add(posTerm->inverse());
				uc[2] = uc[2].add(posTerm->inverse());

				// On garde la meme orientation pour la position voulue
				Point3D ud[3] = {uc[0],uc[1],uc[2]};

				// Optimisation du joint courant sur l'axe courant
				Point3D axis;

				// Calcul des 3 axes (on applique la matrice de vue de départ a X Y Z
				// puis on soustraint la position de départ, pour obtenir le nouveau repere X' Y' Z' bien orienté
				// selon les 3 degrés de liberté du joint
				Point3D axed1 = Point3D(1,0,0);
				axed1.multVecteurMatrice(matRotDepart);
				axed1 = axed1.add(posDepart->inverse());

				Point3D axed2 = Point3D(0,1,0);
				axed2.multVecteurMatrice(matRotDepart);
				axed2 = axed2.add(posDepart->inverse());

				Point3D axed3 = Point3D(0,0,1);
				axed3.multVecteurMatrice(matRotDepart);
				axed3 = axed3.add(posDepart->inverse());


				switch (axe) {
					case 1: axis = axed1; break;
					case 2: axis = axed2; break;
					case 3: axis = axed3; break;
				}

				// K1
				float k1 = 0;
				for (int j = 0; j<3; j++) {
					k1 += (ud[j].produitf32(axis)) * (uc[j].produitf32(axis));
				}
				k1 *= w0;
				k1 += wp * Pid.produitf32(axis) * Pic.produitf32(axis);

				//K2
				float k2 = 0;
				for (j = 0; j<3; j++) {
					k2 += ud[j].produitf32(uc[j]);
				}
				k2 *= w0;
				k2 += wp * Pid.produitf32(Pic);

				//K3
				float k3 = 0;
				Point3D temp =Point3D(0,0,0);
				for (j = 0; j<3; j++) {
					temp = temp.add(uc[j].produitVectoriel(ud[j]));
				}
				temp = temp.mult(w0);
				temp = temp.add((Pic.produitVectoriel(Pid)).mult(wp));
				k3 = axis.produitf32(temp);

				//TETA
				//Le deplacement de l'angle optimisé
				float teta;
				if (k2-k1 != 0) {
					teta = atanf(k3/(k2-k1));
					//teta[axe-1] = atan2(k3, k2-k1);
				} else {
					// Normalement impossible
					teta = 0;
				}

				//TEST: on a bien a = 0
				//float a = (k1-k2)*sinf(teta[axe-1])+k3*cosf(teta[axe-1]);
				//printf("test: %f\n", a);

				// Applique le changement de rotation sur le joint
				float rot;
				switch (axe) {
					case 1: rot= getRotJoint(numeroFrame, curseur, 'X');
							setRotJoint(teta + rot, numeroFrame, curseur, 'X');
							break;
					case 2: rot= getRotJoint(numeroFrame, curseur, 'Y');
							setRotJoint(teta + rot, numeroFrame, curseur, 'Y');
							break;
					case 3: rot= getRotJoint(numeroFrame, curseur, 'Z');
							setRotJoint(teta + rot, numeroFrame, curseur, 'Z');
							break;
				}




				// On passe au joint suivant
				//list<Joint*> * listeCourante = curseur->getListeEnfants();
				//curseur = *(listeCourante->begin());

				// Liberation mémoire
				delete matRotTerm;
				delete matRotDepart;
				delete posDepart;
				delete posTerm;
			}

		}

	}

	// Destruction de chaine temporaire
	delete chaine;
}
*/
				/*
				float rotX = getRotJoint(numeroFrame, curseur, 'X');
				float rotY = getRotJoint(numeroFrame, curseur, 'Y');
				float rotZ = getRotJoint(numeroFrame, curseur, 'Z');
				setRotJoint(teta[0] + rotX, numeroFrame, curseur, 'X');
				setRotJoint(teta[1] + rotY, numeroFrame, curseur, 'Y');
				setRotJoint(teta[2] + rotZ, numeroFrame, curseur, 'Z');
				*/
				/*
				setRotJoint(0, numeroFrame, curseur, 'X');
				setRotJoint(0, numeroFrame, curseur, 'Y');
				setRotJoint(0, numeroFrame, curseur, 'Z');
				*/

//////////////////////////////////////////////////
// PROC:   Bvh::interpoler()
// DOES:   Interpolation entre deux animations
//		   Aucun retimage effectué
//////////////////////////////////////////////////
Bvh::Bvh * Bvh::interpoler(Bvh* bvh1, Bvh * bvh2, float poids) {

    // Déclaration de l'animation finale
    Bvh * bvhResult;

    // On part de la copie de la mocap la plus petite
    if (bvh1->getNbFrame() > bvh2->getNbFrame()) {
        bvhResult = new Bvh(*bvh1);
    } else {
        bvhResult = new Bvh(*bvh2);
    }

    //bvhResult->fixerTailleSquelette(

    // Calcul et stockage de la longueur totale de la mocap résultante
    //int longueurTotale = (int)(poids * bvh1->getNbFrame() + (1-poids) * bvh2->getNbFrame());
    int longueurTotale = bvh1->getNbFrame()>bvh2->getNbFrame()?bvh1->getNbFrame():bvh2->getNbFrame();
    bvhResult->getMocapData()->setNewFrameNumber(longueurTotale);

    // RETIMING + Interp
    // (bon pour l'instant on considère que la durée des frames est la même des 2 cotés)


	// Pour chaque joint de l'animation
	SkeletalData::JointListIndexedByName listeJointsBvh1 =  bvh1->getSkeletalData()->getListeJointsByName();
	SkeletalData::JointListIndexedByName listeJointsBvh2 =  bvh2->getSkeletalData()->getListeJointsByName();
	for (SkeletalData::JointListIndexedByName::iterator it = listeJointsBvh1.begin(); it != listeJointsBvh1.end(); it++) {
        // On récupere le joint courant
        Joint * jointCourantBvh1 = it->second;

        // On récupere le joint associé dans l'anim2
        Joint * jointCourantBvh2 = bvh2->getSkeletalData()->getJointByName(jointCourantBvh1->getName());
        if (jointCourantBvh2 == NULL) return NULL;

        // On récupere le joint concerné (le joint résultat)
        // On n'effectue aucune vérification de présence du joint, car l'animation étant produite à partir
        // d'une des deux anims à interpoler, le joint existe FORCEMENT
        Joint * jointResult = bvhResult->getSkeletalData()->getJointByName(jointCourantBvh1->getName());


        // Au passage, on retaille son offset/endoffset
        char axes[] = {'X', 'Y', 'Z'};
        for (int i=0; i<3; i++) {
            jointResult->setOffsetPos( poids * jointCourantBvh1->getOffsetPos(axes[i])
                                + (1.-poids) * jointCourantBvh2->getOffsetPos(axes[i]),
                                    axes[i]);

            if (jointResult->isTerminal()) {
                jointResult->setOffsetEndPos( poids * jointCourantBvh1->getOffsetEndPos(axes[i])
                                    + (1.-poids) * jointCourantBvh2->getOffsetEndPos(axes[i]),
                                        axes[i]);

            }

        }


        // Pour chaque frame de l'animation résultante
        for (int numeroFrame=0; numeroFrame<longueurTotale; numeroFrame++) {

            // On récupere les données de l'anim1 et 2 en respectant les poids du mélange
            MocapData::JointData jointData1 =
                bvh1->getMocapData()->getJointData(numeroFrame * (bvh1->getNbFrame()-1) / (longueurTotale-1), jointCourantBvh1);

            MocapData::JointData jointData2 =
                bvh2->getMocapData()->getJointData(numeroFrame * (bvh2->getNbFrame()-1) / (longueurTotale-1), jointCourantBvh2);

            // On effectue le mélange
            MocapData::JointData jointDataResult;

            // Mélange des rotations
            float rotXYZ1[] = {0, 0, 0};
            float rotXYZ2[] = {0, 0, 0};

            for (int i=0; i<3; i++) {
                float rot1 = 0, rot2 = 0;
                switch (i) {
                    case 0: rot1 = jointData1.rotX;
                            rot2 = jointData2.rotX;
                            break;
                    case 1: rot1 = jointData1.rotY;
                            rot2 = jointData2.rotY;
                            break;
                    case 2: rot1 = jointData1.rotZ;
                            rot2 = jointData2.rotZ;
                            break;
                    default:
                            break;
                }

    			if ( ((rot1<-90)&&(rot2>90)) || ((rot1>90)&&(rot2<-90)) ) {


    				if (fabs(rot1)> fabs(rot2)) {
    					if (rot1>0)
    						rot1 -=360;
    					else
    						rot1 +=360;
    				} else {
    					if (rot2>0)
    						rot2 -=360;
    					else
    						rot2 +=360;
    				}


    			}

                rotXYZ1[i] = rot1;
                rotXYZ2[i] = rot2;
            }

            jointDataResult.rotX = poids * rotXYZ1[0] + (1.-poids) * rotXYZ2[0];
            jointDataResult.rotY = poids * rotXYZ1[1] + (1.-poids) * rotXYZ2[1];
            jointDataResult.rotZ = poids * rotXYZ1[2] + (1.-poids) * rotXYZ2[2];

            /*
            jointDataResult.rotX = poids * jointData1.rotX + (1-poids) * jointData2.rotX;
            jointDataResult.rotY = poids * jointData1.rotY + (1-poids) * jointData2.rotY;
            jointDataResult.rotZ = poids * jointData1.rotZ + (1-poids) * jointData2.rotZ;
            */

            // Mélange des translations
            if (jointResult->isTransJoint()) {
                jointDataResult.transX = poids * jointData1.transX + (1-poids) * jointData2.transX;
                jointDataResult.transY = poids * jointData1.transY + (1-poids) * jointData2.transY;
                jointDataResult.transZ = poids * jointData1.transZ + (1-poids) * jointData2.transZ;
            }


            // On stocke le résultat
            bvhResult->getMocapData()->setJointData(jointDataResult, numeroFrame, jointResult);


        }

    }


    // Renvoie le résultat
    return bvhResult;

    /*
	// On commence le mélange sur une portion égale a 40% de la fin de l'animation
	int frameDebutMelange = (int)(0.6 * getNbFrame());

	// Mélange degressif de la fin vers frameDebutMelange
	for (numeroFrame=getNbFrame()-1; numeroFrame>=frameDebutMelange; numeroFrame--) {
		// Poids du mélange
		float poids = 0.8*(float)(numeroFrame-frameDebutMelange) / (getNbFrame()-frameDebutMelange-1.);
		// Pour chaque joint de la frame
		SkeletalData::JointListIndexedByName listeJoints =  skeletalData->getListeJointsByName();
		for (SkeletalData::JointListIndexedByName::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
            Joint * jointCourant = it->second;

            char axes[] = {'X', 'Y', 'Z'};

            // - Mélange des 3 translations si elle existent
            if (jointCourant->isTransJoint()) {

                for (int k=0; k<3; k++) {
                    float trans1 = mocapData->getTrans(numeroFrame, axes[k], jointCourant);
                    float trans2 = mocapData->getTrans((numeroFrame+1)%getNbFrame(), axes[k], jointCourant);

                    // Mélange
                    mocapData->setTrans((1.-poids) * trans1 + poids * trans2, numeroFrame, axes[k], jointCourant);
                }
            }

            // - Mélange des 3 rotations
            for (int k=0; k<3; k++) {
                float rot1 = mocapData->getRot(numeroFrame, axes[k], jointCourant);
                float rot2 = mocapData->getRot((numeroFrame+1)%getNbFrame(), axes[k], jointCourant);

                // Avant d'effectuer le mélange, on vérifie qu'il ne s'agit
    			// pas de 2 angles <-90° et >90° dont le mélange donnerait une mauvaise valeur
    			// Exemple: -179 + 179° donnerait 0° alors que le résultat doit être 180°
    			if ( ((rot1<-90)&&(rot2>90)) || ((rot1>90)&&(rot2<-90)) ) {


    				if (fabs(rot1)> fabs(rot2)) {
    					if (rot1>0)
    						rot1 -=360;
    					else
    						rot1 +=360;
    				} else {
    					if (rot2>0)
    						rot2 -=360;
    					else
    						rot2 +=360;
    				}


    			}

    			// On effectue le mélange avec la frame qui suit
    			float rotMelange = (1.-poids) * rot1 + (poids * rot2);

    			// Stockage de la valeur
    			mocapData->setRot( modulo(rotMelange, -180,180) , numeroFrame, axes[k], jointCourant);

            }
        }

	}
	*/
}

/*
//////////////////////////////////////////////////
// PROC:   Bvh::genererMocap()
// DOES:   Génere une mocap différente (en construction, pour tests seulement)
//////////////////////////////////////////////////
void Bvh::genererMocap() {
	// Paramètres random
	srand(time(NULL));
	float rdBz = (rand()%100)/90.0;
	float rdBx = (rand()%100)/60.0-0.4;
	float rdJz = (rand()%100)/40.0;
	float rdJx = (rand()%100)/80.0;


	// Pour chaque frame
	for (int k=0; k<getNbFrame(); k++) {

		cout << "Frame " << k+1 << "/" << getNbFrame() << "..." << endl;


		Point3D * brasG = getPositionJointByNameInFrame(k, "LeftWrist", true);
		Point3D * brasD = getPositionJointByNameInFrame(k, "RightWrist", true);
		Point3D * jambeD = getPositionJointByNameInFrame(k, "LeftAnkle", true);
		Point3D * jambeG = getPositionJointByNameInFrame(k, "RightAnkle", true);

		appliquerIkCcd(k, "LeftCollar", Point3D(brasG->getX()*rdBx,brasG->getY(),brasG->getZ()*rdBz), 100);
		appliquerIkCcd(k, "RightCollar", Point3D(brasD->getX()*rdBx,brasD->getY(),brasD->getZ()*rdBz), 100);
		appliquerIkCcd(k, "LeftHip", Point3D(jambeD->getX() - rdJx,jambeD->getY(),jambeD->getZ()*rdJz), 100);
		appliquerIkCcd(k, "RightHip", Point3D(jambeG->getX() + rdJx,jambeG->getY(),jambeG->getZ()*rdJz), 100);



		delete brasG;
		delete brasD;
		delete jambeD;
		delete jambeG;

	}


	// Sauvegarde du mouvement généré
	time_t temps = time(NULL);
    ostringstream oss;
    oss << "./gen/" << temps << ".bvh";
	string nomFichier = oss.str();
	write(nomFichier.c_str());


}
*/
//////////////////////////////////////////////////
// PROC:   Bvh::isFirstFootSupportFrame(int frameCourante)
// DOES:  Indique si la phase de support du premier pied est en cours
//////////////////////////////////////////////////
bool Bvh::isFirstFootSupportPeriod(int frameCourante) {
    return (frameCourante > frameDebutSupport && frameCourante > frameFinSupport);

}

//////////////////////////////////////////////////
// PROC:   Bvh::changerPremierPiedSupport()
// DOES: Change le premier pied de support
//////////////////////////////////////////////////
void Bvh::changerPremierPiedSupport() {
    // On détecte les phase de support de chaque pied (si ce n'est pas déjà fait)
    detecterSupportPied(jambeDroite, jambeGauche);

    // L'idée est de déplacer l'ensemble des frames de la premiere phase de support vers la phase de retour
    // et inversement

    // Pour chaque joint de la frame
	SkeletalData::JointListIndexedByName listeJoints =  skeletalData->getListeJointsByName();
	for (SkeletalData::JointListIndexedByName::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
        // Récupération du joint courant
        Joint * jointCourant = it->second;


        // Pour chaque frame de l'animation
        int totalElementDeplace = 0;
        int dernierIndexDeplace = 0;

        // Tant que l'on n'a pas déplacé autant d'élément (de jointData pour le joint courant) que le nombre de frames
    	while (totalElementDeplace < getNbFrame()) {

            // jointDataNew est l'élément en cours de déplacement
            // Au départ de l'algorithme, il s'agit du premier élément (situé à dernierIndexDeplace)
            MocapData::JointData jointDataNew = mocapData->getJointData(dernierIndexDeplace, jointCourant);
            // On calcule alors le prochaine index ou ira jointDataNew
            dernierIndexDeplace = (dernierIndexDeplace + (getNbFrame()-frameFinSupport-1)) % getNbFrame();

            // Tant que l'on n'a pas fait un tour complet, c'est à dire que l'on n'est pas revenu
            // à l'index de départ
            int sauvLastIndex=dernierIndexDeplace;
            do {

                // On récupere les données à l'index ou l'on va écrire
                MocapData::JointData jointDataTemp = mocapData->getJointData(dernierIndexDeplace, jointCourant);

                // On y place les données du joint sauvegardé dans jointDataNew
                mocapData->setJointData(jointDataNew,
                                        dernierIndexDeplace,
                                        jointCourant);

                // Finalement on place les données sauvegardées à l'index qui vient d'être écrasé
                // dans jointDataNew (et on boucle l'algo)
                jointDataNew = jointDataTemp;

                // On totaliste un élément de plus déplacé pour ce joint
                totalElementDeplace++;

                // Mise à jour index
                int nouvelIndex = (dernierIndexDeplace + (getNbFrame()-frameFinSupport-1)) % getNbFrame();
                dernierIndexDeplace = nouvelIndex;


            } while(sauvLastIndex != dernierIndexDeplace);

            //totalElementDeplace += frameFinSupport - frameDebutSupport;

            // On passe à l'index suivant (si utile, si encore des éléments à déplacer)
            dernierIndexDeplace +=1;

        }

    }

    // On final, on échange aussi les marqueurs des joints des jambes
    string jambeDroiteTemp = this->getJambeDroite();
    this->setJambeDroite(this->getJambeGauche());
    this->setJambeGauche(jambeDroiteTemp);

}


//////////////////////////////////////////////////
//			 CLASSE BVHGLISSEMENTM_PIED
//////////////////////////////////////////////////


//////////////////////////////////////////////////
// PROC:   BvhGlissementPied::BvhGlissementPied()
// DOES:   Constructeur
//////////////////////////////////////////////////
BvhGlissementPied::BvhGlissementPied(Bvh * bvh, string nomHautJambe) {
	// Enregistrement du parent
	this->bvh = bvh;

	// Nom de la jambe
	this->nomHautJambe = nomHautJambe;

	// Initialisation de l'objet
	init();

	// Création et sauvegarde des chaines représentant les jambes
	chaine = bvh->getChaineJointVersTerminal(nomHautJambe);

	// On stocke le nom du pied
	//nomPied = (*(chaine->begin()))->getName();
	nomPied = bvh->getEndJointName(nomHautJambe);
}

//////////////////////////////////////////////////
// PROC:   BvhGlissementPied::init()
// DOES:   Initialisation de l'objet
//////////////////////////////////////////////////
void BvhGlissementPied::init() {
	pointPiedDebutMonde = NULL;
	pointPiedDebutPers = NULL;
	A = NULL;
	B = NULL;
	derniereFrameTraitee = -1;

}


//////////////////////////////////////////////////
// PROC:   BvhGlissementPied::fixerGlissementPied()
// DOES:   Fixe le glissement d'un pied
//		   Il faut donner la position du personnage, sa direction, le nom de la partie supérieure de la jambe
//		   Ainsi que les frames de début et de fin de support du pied, et le numéro de la frame à traiter
//////////////////////////////////////////////////
void BvhGlissementPied::fixerGlissementPied(int numeroFrame, int frameDebutSupport, int frameFinSupport, float xPers, float yPers, float direction){

	// Nombre de passe CCD
	const int nbrIteration = 40;

    //#define DEBUG_IKCCD

	// Test de faisabilité (par exemple, si le mouvement n'est pas de la marche)
	if (frameDebutSupport==-1 ||frameFinSupport==-1) return;

	// DEBUT: premier support du pied du cycle
	if (numeroFrame == frameDebutSupport) {

			//detecterSupportPied(getEndJointName(nomHautJambe1),getEndJointName(nomHautJambe2));

			// On calcule la position du pied au premier support dans le repere personnage
			delete pointPiedDebutMonde;
			pointPiedDebutMonde = bvh->getPositionJointByNameInFrame(frameDebutSupport, nomPied, true);

			// On sauvegarde cette position pour un traitement dans la seconde phase
			delete pointPiedDebutPers;
			pointPiedDebutPers = new Point3D(*pointPiedDebutMonde);

			// On passe pointPiedDebutMonde dans le repere Monde (en tenant compte de la rotation du personnage,
			// de sa position, et inversion du sens de 2 axes)
			float z = -pointPiedDebutMonde->getZ();
			float x = -pointPiedDebutMonde->getX();
			float alpha = M_PI*(-direction)/180.;
			pointPiedDebutMonde->setX(x * cosf(alpha) - z*sinf(alpha));
			pointPiedDebutMonde->setZ(x * sinf(alpha) + z*cosf(alpha));
			//pointPiedDebutMonde->setY(0);
			Point3D result = pointPiedDebutMonde->add(Point3D(xPers,0,yPers));
			delete pointPiedDebutMonde;
			pointPiedDebutMonde = new Point3D(result);


	} else
	// PREMIERE PHASE: contrainte de la pose du pied
	if ((numeroFrame > frameDebutSupport && numeroFrame <= frameFinSupport && frameDebutSupport<frameFinSupport)
		|| ((numeroFrame > frameDebutSupport || numeroFrame <= frameFinSupport) && frameDebutSupport>frameFinSupport)

		&& pointPiedDebutMonde) {

		// On va alors calculer la position fixe du pied dans le repere personnage apres que le personnage ait bougé

		// On part de la position du pied dans le repere monde auquel on soustrait les coordonées du personnage
		Point3D pointPiedFixePers = Point3D(-pointPiedDebutMonde->getX()+xPers,pointPiedDebutMonde->getY(),-pointPiedDebutMonde->getZ()+yPers);

		// On corrige l'éventuelle rotation du personnage, et on se retrouve dans le repere personnage
		float x = pointPiedFixePers.getX();
		float z = pointPiedFixePers.getZ();
		float alpha = M_PI*(direction)/180.;
		pointPiedFixePers.setX(x * cosf(alpha) - z*sinf(alpha));
		pointPiedFixePers.setZ(x * sinf(alpha) + z*cosf(alpha));


		// DEBUG: affichage zone de contact fixée
        #ifdef DEBUG_IKCCD
		glPushMatrix();

		glRotatef(90.0, 0.0, 1.0, 0.0);
		glTranslatef(0.0, yPers ,xPers);
		glRotatef(90, 0.0, 1.0, 0.0);

		glRotatef(180, 0.0, 0.0, 1.0);
		glRotatef(direction, 0.0, 0.0, 1.0);
		glRotatef(-90, 1.0, 0.0, 0.0);

		glTranslatef(pointPiedFixePers.getX(),pointPiedFixePers.getY(), pointPiedFixePers.getZ());;

		glColor3d(0,0,1);
		glutSolidSphere(0.5,10,5);
		glPopMatrix();
		#endif

		// On applique la méthode CCD sur le point déterminé
		//bvh->appliquerIkCcd(numeroFrame, nomHautJambe, pointPiedFixePers, nbrIteration);
		bvh->appliquerIkCcd(numeroFrame, chaine, pointPiedFixePers, nbrIteration);

		// Force la remise à jour de A et B dans la deuxieme phase
		delete A;
		delete B;
		A = NULL;
		B = NULL;


	}
	// DEUXIEME PHASE: retour cohérent du pied
	else {

		// Phase de retour du pied
		if ((pointPiedDebutPers && numeroFrame != frameDebutSupport)
		    & (numeroFrame != derniereFrameTraitee)){

			// Sauvegarde de la dernière frame traitée
			// Ceci permet de ne pas affecter la trajectoire de retour du pied, même
			// si le personnage est en train de bouger (et que donc on apelle la fonction
			// pour mettre à jour la position du pied au sol)
			derniereFrameTraitee = numeroFrame;

			// Si on n'a pas détecter la fin du mouvement et les 2 positions des pieds (avant et apres correction)
			if (!A || !B) {
				//detecterSupportPied(getEndJointName(nomHautJambe1),getEndJointName(nomHautJambe2));

				// La position de fin de pose
				B = bvh->getPositionJointByNameInFrame(frameFinSupport, nomPied, true);
				// La position sans correction juste apres la fin de pose
				A = bvh->getPositionJointByNameInFrame((frameFinSupport+1)%bvh->getNbFrame(), nomPied, true);

				// On place A et B dans le répere centré sur pointPosPied (là ou on a posé le pied en début de support
				// dans le repere centré sur le personnage)
				A->setZ(A->getZ() - pointPiedDebutPers->getZ());
				A->setY(A->getY() - pointPiedDebutPers->getY());
				B->setZ(B->getZ() - pointPiedDebutPers->getZ());
				B->setY(B->getY() - pointPiedDebutPers->getY());

			}


			// Optimisation: on arrete le traitement si A et B sont proches
			// (aucune correction du retour du pied à faire)
			if (A->distanceVers(*B)<0.05) return;

			// Calcul de alpha: rotation pour aligner A et B
			// Alpha = angle entre (A,Axe) - angle entre (B,Axe)
			//float alpha = atan2f(A->getY(), A->norme()) - atan2f(B->getY(), B->norme());
			float angleA = atan2f(fabs(A->getY()), fabs(A->getZ()));
			float angleB = atan2f(fabs(B->getY()), fabs(B->getZ()));
			float alpha;


            // Inversion de signe selon la différence de niveau en début et fin de support
            if (pointPiedDebutPers->getY() < B->getY())
                alpha = angleA - angleB;
            else
                alpha = angleB - angleA;

			// Position courante du pied
			Point3D * P = bvh->getPositionJointByNameInFrame(numeroFrame, nomPied, true);
			P->setZ(P->getZ() - pointPiedDebutPers->getZ());
			P->setY(P->getY() - pointPiedDebutPers->getY());

			// Nouveau point de la trajectoire de retour du pied
			Point3D pointCorrige = Point3D(0,0,0);
			pointCorrige.setY((B->norme()/ A->norme()) * (P->getY() * cosf(alpha) - P->getZ() * sinf(alpha)) );
			pointCorrige.setZ((B->norme()/ A->norme()) * (P->getY() * sinf(alpha) + P->getZ() * cosf(alpha)) );
			pointCorrige.setX(B->getX());  // On garde le meme écartement de la jambe qu'à la fin de la phase de pose

			// Replace le point dans le repère personnage
			pointCorrige.setZ(pointCorrige.getZ() + pointPiedDebutPers->getZ());
			pointCorrige.setY(pointCorrige.getY() + pointPiedDebutPers->getY());

			// Applique finalement la méthode CCD
			//bvh->appliquerIkCcd(numeroFrame, nomHautJambe, pointCorrige, nbrIteration);
			bvh->appliquerIkCcd(numeroFrame, chaine, pointCorrige, nbrIteration);


			// Libération mémoire
			delete P;


			//DEBUG: affichage du point de contact du pied prévu en début de support
			#ifdef DEBUG_IKCCD
			glPushMatrix();
			//glScalef(0.2,0.2,0.2);

			glRotatef(90, 1.0, 0.0, 0.0);
			glTranslatef(xPers, 0., -yPers );
			glRotatef(direction, 0, 1, 0);

			glTranslatef(pointCorrige.getX(), pointCorrige.getY(), pointCorrige.getZ());

			glColor3d(1,0,1);
			glutSolidSphere(0.5,10,5);
			glPopMatrix();



			//DEBUG: affichage du point de contact du pied prévu en début de support
			glPushMatrix();

			glRotatef(90.0, 0.0, 1.0, 0.0);
			glTranslatef(0.0, yPers ,xPers);
			glRotatef(90, 0.0, 1.0, 0.0);

			glRotatef(180, 0.0, 0.0, 1.0);
			glRotatef(direction, 0.0, 0.0, 1.0);
			glRotatef(-90, 1.0, 0.0, 0.0);

			glTranslatef(pointPiedDebutPers->getX(), pointPiedDebutPers->getY(), pointPiedDebutPers->getZ());

			glColor3d(1,0,1);
			glutSolidSphere(0.5,10,5);
			glPopMatrix();

			#endif

		}


	}

}

#include "SkinLoader.h"
#include "Mat4f.h"

#include <GL/gl.h>

SkinLoader::SkinLoader() : MocapLoader() {

}

SkinLoader::~SkinLoader() {

}


bool SkinLoader::load(string nomFichier) {

    string extension = nomFichier.substr(nomFichier.find_last_of('.'));
    // On extrait le nom du fichier sans l'extension et dans "_anim" ou "_skin"
    string nomFichierBase = nomFichier.substr(0,nomFichier.find_last_of('_'));

    // On construit les 2 noms de fichier
    string fichierAnim = nomFichierBase + "_anim" + extension;
    string fichierSkin = nomFichierBase + "_skin" + extension;

    // Chargement du fichier
    load(fichierSkin, fichierAnim);

    return false;
}

bool SkinLoader::load(string nomFichierSkin, string nomFichierAnim) {

    // Vide la liste des joints
    listeJointsById.clear();

    // Création du parser sur le fichierSkin
	Parser * parser = new Parser(nomFichierSkin);
	if (parser->isAnError()) {
        delete parser;
		return false;

	}

    // Verification de l'entête
	parser->expect("version");
	parser->expect("1");

    // CHARGEMENT DU SQUELETTE

	// Lecture de la hiérarchie
	parser->expect("nodes");

	// Lecture de la racine
    int boneIndex = parser->getInt();

    //string boneName = parser->getstring();
    //boneName.assign(boneName, 1, boneName.size()-2); // On enleve les guillemets
    string boneName = parser->getstringBetween('\"');

    int boneParentIndex = parser->getInt();
    // On s'assure qu'il s'agit de la racine
    if (boneParentIndex != -1) return false;

    // Création du joint découlant
    string chtype[6];
    chtype[0] = "Xposition";
    chtype[1] = "Yposition";
    chtype[2] = "Zposition";
    chtype[3] = "Zrotation";
    chtype[4] = "Xrotation";
    chtype[5] = "Yrotation";

    float offset[3];
    offset[0] = offset[1] = offset[2] = 0;

    Joint * racine = new Joint(boneName, 6, true, offset, chtype, boneIndex);

    // Ajout de la racine
    listeJointsById[boneIndex] = racine;

    chtype[0] = "Zrotation";
    chtype[1] = "Xrotation";
    chtype[2] = "Yrotation";
    // Et du reste du squelette
    int nbJoints = 1;
	while (1){
        // On s'assure que l'on n'est pas à la fin de la définition du squelette
        if (!parser->nextIsNumber()) break;

        // Lecture du bone

        int boneIndex = parser->getInt();

        //string boneName = parser->getstring();
        //boneName.assign(boneName, 1, boneName.size()-2); // On enleve les guillemets
        string boneName = parser->getstringBetween('\"');
        int boneParentIndex = parser->getInt();


        if ((boneParentIndex != -1)) {
            // Log
            //clog << boneIndex << boneName << boneParentIndex << endl;
            // Ajout du bone
            Joint * currentJoint = new Joint(boneName, 3, false, offset, chtype, boneIndex);
            nbJoints++;
            listeJointsById[boneIndex] = currentJoint;

            // Enregistrement aupres du parent
            listeJointsById[boneParentIndex]->addChildJoint(currentJoint);
        }
    }
    // Fin de la section
    parser->expect("end");

    // POSITIONNE LES ENDSITE (a modifier/supprimer la notion de endsite)
	for (JointListIndexedByID::iterator it = listeJointsById.begin(); it != listeJointsById.end(); it++) {

        Joint * jointCourant = listeJointsById[it->first];
        // Si aucun enfant, alors le joint est un endsite
        if (jointCourant->getListeEnfants()->empty()) {
            jointCourant->addEndSite(offset); /* 0 0 0 */
        }
	}

    // On crée le skeletalData à partir de la racine lues (et de ses fils)
    skeletalData = new SkeletalData(racine);

    // CHARGEMENT de la posture de référence
    parser->expect("skeleton");
    parser->expect("time");
    parser->expect("0");

    // On cree un espace vide de stockage des données pour la posture de référence
	MocapData * referencePosData;
    if (nbJoints>0) {
        referencePosData = new MocapData(1, nbJoints*3+3, 1);
	} else {
        referencePosData = NULL;
    }

	// Lecture
 	while (1){
        // On s'assure que l'on n'est pas à la fin de la définition du squelette
        if (!parser->nextIsNumber()) break;

        int boneIndex = parser->getInt();
        float posX = parser->getFloat();
        float posY = parser->getFloat();
        float posZ = parser->getFloat();
        float rotX = parser->getFloat() * 180 / M_PI; // Les valeurs lues sont en radians
        float rotY = parser->getFloat() * 180 / M_PI; // Nous convertissons en degrés
        float rotZ = parser->getFloat() * 180 / M_PI;

        // Si le joint existe
        if (listeJointsById.find(boneIndex) != listeJointsById.end()) {

            // On récupere le joint courant
            Joint * currentJoint = listeJointsById[boneIndex];

            // Si il y a au moins un joint (et plus en général)
            if (nbJoints>0) {
                // On rajoute le joint dans le mocap data
                referencePosData->addJoint(currentJoint);


                if (currentJoint->getName() == racine->getName()) {
                    currentJoint->setOffsetPos(0, 'X');
                    currentJoint->setOffsetPos(0, 'Y');
                    currentJoint->setOffsetPos(0, 'Z');

                    referencePosData->setTransXYZ(posX, posZ, -posY, 0, currentJoint);

                    // On passe dans notre système de rotation ZXY
                    convertEulerZYXtoZXY(rotX, rotY, rotZ, rotX, rotY, rotZ, true);
                    referencePosData->setRotXYZ(rotX, rotZ, rotY, 0, currentJoint);
                } else {

                    // Les positions ne changent pas en fait, c'est pour celà qu'on les place en tant qu'offset
                    currentJoint->setOffsetPos(posX, 'X');
                    currentJoint->setOffsetPos(posY, 'Y');
                    currentJoint->setOffsetPos(posZ, 'Z');

                    // On passe dans notre système de rotation ZXY
                    convertEulerZYXtoZXY(rotX, rotY, rotZ, rotX, rotY, rotZ);
                    referencePosData->setRotXYZ(rotX, rotY, rotZ, 0, currentJoint);
                }
            }
        }

    }

    // Fin de la section
    parser->expect("end");


    // CHARGEMENT DES TRIANGLES DU FICHIER DE SKIN
    parser->expect("triangles");

    // On cree un espace vide de stockage des données de skin et de maillage
    if (nbJoints > 0) {
    	skinData = new SkinData(referencePosData);
    }else {
        // Ici on ne charge que le modèle donc il n'y a aucun squelette et pose de référence
        skinData = new SkinData(NULL);

    }
	// On considere que le chemin vers les textures est le meme que l'animation en cours de chargement
	skinData->setTexturesPath( nomFichierSkin.substr(0,nomFichierSkin.find_last_of('/')) + "/" );

	// Compteur de triangle
	int nbTriangles = 0;

    while (1){

        // Augmentation du nombre de triangle
        nbTriangles++;

        // Lecture du nom de la texture
        string textureFileName = parser->getstring();
        if (textureFileName == "end" || parser->isEOF()) break;

        SkinData::Vertex tabVertex[3];

        // Lecture des 3 vertex du triangles
        for (int i=0; i <3; i++) {

            // Lecture du bone Id correspondant au vertex (ignoré pour l'instant)
            parser->getInt();

            // Lecture d'un vertex
            float posX = parser->getFloat();
            float posY = parser->getFloat();
            float posZ = parser->getFloat();
            float normX = parser->getFloat();
            float normY = parser->getFloat();
            float normZ = parser->getFloat();
            float textU = parser->getFloat();
            float textV = parser->getFloat();

            // Création du vertex
            // Attention on flip les normales
            //SkinData::Vertex *vertex = skinData->getVertex(posX, posZ, -posY, normX, normZ, -normY, textU, textV);
            SkinData::Vertex vertex = SkinData::Vertex(posX, posZ, -posY, normX, normZ, normY, textU, textV);

            // Si il existe des influences
            if (!parser->nextIsEOL()) {
                // Nombre d'influence
                int nbInfluence = parser->getInt();

                // Lecture des influences des bones sur le vertex
                //while(!parser->nextIsEOL() && !parser->isEOF()) {
                for (int k=0; k<nbInfluence; k++) {
                    // Lecture du bone qui influence
                    int boneInfluencerId = parser->getInt();

                    // Lecture de la valeur de l'influence
                    float boneInfluence = parser->getFloat();

                    // Ajout de l'influence au vertex
                    vertex.addWeightInfluence(boneInfluencerId, boneInfluence);
                }
            }

            // Enregistrement du vertex
            tabVertex[i] = vertex;
        }

        // Création et enregistrement du triangle formé des 3 vertex précédent
        skinData->addTriangle(tabVertex[0], tabVertex[1], tabVertex[2], textureFileName);

    }

    // Logue le nombre de triangle
    clog << nbTriangles << " triangles" << endl;

    // FIN DU CHARGEMENT DU FICHIER SKIN
	// Detruit le parser (et ferme le fichier donc)
	delete parser;


    // CHARGEMENT DU FICHIER D'ANIMATION

    // Initialisation
    mocapData = NULL;

    // Création du parser sur le fichierAnim
	Parser * parserAnim = new Parser(nomFichierAnim);
	if (parserAnim->isAnError()) {
        // On efface le parser
        delete parserAnim;

        // Si le fichier d'anim n'est pas spécifié, c'est juste le maillage qui interesse, donc
        // ce n'est pas une erreur
        if (nomFichierAnim != "") {
    		return false;
    	} else {
            // On efface les objets inutiles
            delete skeletalData;
            skeletalData = NULL;
            return true;
        }
	}

    // Verification de l'entête
	parserAnim->expect("version");
	parserAnim->expect("1");


    // CHARGEMENT DU SQUELETTE pour vérifier que la hiérarchie est bien la même

	// Lecture de la hiérarchie
	parserAnim->expect("nodes");
	while (1){
        // On s'assure que l'on n'est pas à la fin de la définition du squelette
        if (!parserAnim->nextIsNumber()) break;

        // Lecture du bone
        int boneIndex = parserAnim->getInt();
        //string boneName = parserAnim->getstring();
        //boneName.assign(boneName, 1, boneName.size()-2); // On enleve les guillemets
        string boneName = parserAnim->getstringBetween('\"');
        int boneParentIndex = parserAnim->getInt();

        if ((boneParentIndex != -1)) {
            // Verification de l'existance du bone
            if (listeJointsById[boneIndex]->getName() != boneName)
                return false;

        }
    }
    // Fin de la section
    parserAnim->expect("end");


    // CHARGEMENT DES FRAMES DU FICHIER D'ANIM
    parserAnim->expect("skeleton");
    const int trancheAllocationFrame = 10;

    // On cree un espace vide de stockage des données
	mocapData = new MocapData(trancheAllocationFrame, nbJoints*3+3, 0.033333);

	// Nb frames
	int nbFrames = 0;

	while (1) {
        //clog << nbFrames << endl;
        // Teste si la section est fini, ou s'il faut lire une autre frame
        string nomSection = parserAnim->getstring();
        if (nomSection == "end")
            break;
        else if (nomSection != "time")
            return false;

        // Augmence le nombre de frame
        nbFrames++;


        // Lecture du numéro de la frame
        int numeroFrame = parserAnim->getInt();

        // Agrandir l'espace de stockage si nécéssaire
        if (mocapData->getNbFrames() <= nbFrames) {
            mocapData->setNewFrameNumber(mocapData->getNbFrames()+trancheAllocationFrame);
        }

    	// Lecture
     	while (1){
            // On s'assure que l'on n'est pas à la fin de la définition du squelette
            if (!parserAnim->nextIsNumber()) break;

            int boneIndex = parserAnim->getInt();
            float posX = parserAnim->getFloat();
            float posY = parserAnim->getFloat();
            float posZ = parserAnim->getFloat();
            float rotX = parserAnim->getFloat() * 180 / M_PI; // Les valeurs lues sont en radians
            float rotY = parserAnim->getFloat() * 180 / M_PI; // Nous convertissons en degrés
            float rotZ = parserAnim->getFloat() * 180 / M_PI;


            // Si le joint existe
            if (listeJointsById.find(boneIndex) != listeJointsById.end()) {

                // On rajoute le joint dans le mocap data
                Joint * currentJoint = listeJointsById[boneIndex];
                mocapData->addJoint(currentJoint);

                if (currentJoint->getName() == racine->getName()) {
                    // Les offset sont déjà positionnés à 0 pour la racine
                    mocapData->setTransXYZ(posX, posZ, -posY, numeroFrame, currentJoint);

                    // On passe dans notre système de rotation ZXY
                    convertEulerZYXtoZXY(rotX, rotY, rotZ, rotX, rotY, rotZ, true);
                    mocapData->setRotXYZ(rotX, rotY, rotZ, numeroFrame, currentJoint);
                } else {

                    // Les positions ne changent pas en fait, c'est pour celà qu'on les place en tant qu'offset
                    currentJoint->setOffsetPos(posX, 'X');
                    currentJoint->setOffsetPos(posY, 'Y');
                    currentJoint->setOffsetPos(posZ, 'Z');

                    // On passe dans notre système de rotation ZXY
                    convertEulerZYXtoZXY(rotX, rotY, rotZ, rotX, rotY, rotZ);
                    mocapData->setRotXYZ(rotX, rotY, rotZ, numeroFrame, currentJoint);
                }



            }

        }
    }
    // Fin de la section

    // On ajuste la taille de mocapData au nombre exact de frames
    mocapData->setNewFrameNumber(nbFrames);


    //mocapData = referencePosData; // Pour voir la pose de référence, décommenter cette ligne

    // Fermeture du fichier d'anim
    delete parserAnim;

    // FIN DU CHARGEMENT (ouf...)


    // Indique que le chargement s'est bien déroulé
	return true;

}

void SkinLoader::convertEulerZYXtoZXY(float rotXOld, float rotYOld, float rotZOld, float &rotX, float &rotY, float &rotZ, bool isRacine) {

    // Effectue la rotation dans opengl
    glPushMatrix();
    glLoadIdentity();
    if (isRacine)
        glRotatef(-90, 1, 0, 0);
    glRotatef(rotZOld, 0, 0, 1);
    glRotatef(rotYOld, 0, 1, 0);
    glRotatef(rotXOld, 1, 0, 0);

    // Recupere la matrice de rotation associé à la vue courante
    //GLfloat *buf2 = new GLfloat[16];
    Mat4f buf2;
    glGetFloatv(GL_MODELVIEW_MATRIX, &buf2);

    // Recupère les 3 nouveaux angles d'euler associés
    float a = asinf(buf2[6]);
    float b = atan2(-buf2[2], buf2[10]);
    float c = atan2(-buf2[4], buf2[5]);
    /*
    if (a<M_PI/2-0.1 && a >-M_PI/2+0.1 ){
    	if (buf2[10] * cosf(a) >0) {
            b = atan2(-buf2[8], buf2[10]);
        } else {
            b = atan2(buf2[8], buf2[10]) + M_PI;
        }
    	if (buf2[5] * cosf(a) >0) {
            c = atan2(-buf2[4], buf2[5]);
        } else {
            c = atan2(buf2[4], buf2[5]) + M_PI;
        }
    } else {
        if (buf2[6]>0)
            b = atan2(buf2[4], buf2[6]);
        else
            b = atan2(-buf2[4], buf2[6]) + M_PI;
        c = 0;
    }
    */

    // Libération mémoire et restauration
    //delete buf2;
    glPopMatrix();

    // Calcul des résultat en degré
    rotX = a * 180 / M_PI;
    rotY = b * 180 / M_PI;
    rotZ = c * 180 / M_PI;


}

// ECRITURE: NON IMPLEMENTEE
bool SkinLoader::write(const string nomFichier) {
    return false;
}

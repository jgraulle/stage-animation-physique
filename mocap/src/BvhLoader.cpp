#include "BvhLoader.h"

BvhLoader::BvhLoader(): MocapLoader() {

}

BvhLoader::~BvhLoader() {
    
}

Joint * BvhLoader::readJointsFromParser(string currentJointName, Parser * parser) {


    // Valeur par defaut pour la construction du Joint
	//bool endSite = false;
	bool transJoint = false;

    // Commence la lecture des proprietés du Joint
	parser->expect("{");

    // OFFSET
	parser->expect("OFFSET");
	float offset[3];
	offset[0] = parser->getFloat();
	offset[1] = parser->getFloat();
	offset[2] = parser->getFloat();

    // CHANNELS
	parser->expect("CHANNELS");

	int nchannels = parser->getInt();
	string chtype[6];
	for (int i=0; i<nchannels; i++) {
		chtype[i] = parser->getstring();
	}

    // S'il y a 6 channels, alors il y a forcément des valeurs de translation
	if (nchannels==6) {
		if (chtype[0] != "Xposition" || chtype[1] != "Yposition" || chtype[2] != "Zposition") {
			//printf("Non-typical translation order: '\n");//,parser->getCurrentLine().c_str());
			return NULL;
		}
		transJoint = true;               
	}

    
	//int chOffset = totalChannels;
    totalChannels += nchannels;
    
    // CREATION DU JOINT
    Joint * jointCourant = new Joint(currentJointName, nchannels, transJoint, offset, chtype);

    // Enregistre le joint courant                
    listeJoints.push_back(jointCourant);
   
    // SOUS-JOINTS
	while (!parser->isAnError()) {
		string Noeud = parser->getstring();

		if (Noeud == "JOINT") {

			//string childName = parser->getstring();
			string childName = parser->getstringBetween('\n');
			

			//Joint * nouvelEnfant = new Joint(childName, parser, totalChannels);
			//Joint * nouvelEnfant = new Joint(childName, nchannels, transJoint, endSite, offset, endLength, chOffset);
			Joint * nouvelEnfant = readJointsFromParser(childName, parser);
			if (nouvelEnfant != NULL)
    			jointCourant->addChildJoint(nouvelEnfant);
    				
			continue;
			
		} else if (Noeud == "End") {

			parser->expect("Site");
			parser->expect("{");

			parser->expect("OFFSET");
			
            float endLength[3];
			endLength[0] = parser->getFloat();
			endLength[1] = parser->getFloat();
			endLength[2] = parser->getFloat();
		
			jointCourant->addEndSite(endLength);

			parser->expect("}");

			continue;
		} else {
			break;
		}

	}  
    

    // Renvoie le joint crée
    return jointCourant;  
}

bool BvhLoader::load(string nomFichier) {
    
    // Vidage de la liste des joints
    listeJoints.clear();
    
    // Remise à 0 du nombre de channels
    totalChannels = 0;
        
    // Création du parser sur le fichier
	Parser * parser = new Parser(nomFichier);
	if (parser->isAnError()) {
		return false;
	}    

    // CHARGEMENT DU SQUELETTE
    
	// Lecture de la hiérarchie
	parser->expect("HIERARCHY");
	parser->expect("ROOT");
	
	// Racine
	//string rootname = parser->getstring();
	string rootname = parser->getstringBetween('\n');
    //int nchannels = 0;
	//racine = new Joint(rootname, parser, nchannels);
	

	Joint * racine = readJointsFromParser(rootname, parser);
    
    // Création du squeletal data
    skeletalData = new SkeletalData(racine);

	int nchannels = totalChannels;

    
    // On peut alors construire la liste des joints
	//majListeJoints();

	// Lecture des données de capture
	parser->expect("MOTION");

	parser->expect("Frames:");

	int nframes = parser->getInt();

	parser->expect("Frame");
	parser->expect("Time:");

	float dt = parser->getFloat();


    // CHARGEMENT DES DONNEES DE L'ANIMATION
    /*
	float * data = (float *) malloc(nframes*nchannels*sizeof(float));
	// Lecture de chaque frame
	for (int n=0, indx=0; n<nframes; n++) {
		// Pour chaque frame, lecture de l'ensemble de paramètres
		for (int c=0; c<nchannels; c++) {
			data[indx++] = parser->getFloat();
		}
	}
	
	// On crée un espace de stockage pour les données
	mocapData = new MocapData(nframes, nchannels, dt, data);
	
	free(data);
	*/
	
	// On cree un espace vide de stockage des données
	mocapData = new MocapData(nframes, nchannels, dt);

	// Pour chaque frame
	for (int numeroFrame = 0; numeroFrame < mocapData->getNbFrames(); numeroFrame++) {
        // Pour chaque joint
    	for (list<Joint*>::iterator it = listeJoints.begin(); it != listeJoints.end(); it++) {
            Joint * jointCourant = *it;	
            
            // Enregistre le joint
            mocapData->addJoint(jointCourant);
            
            // Translation éventuelle (en général la racine) (attention l'ordre de lecture est important)
            if (jointCourant->isTransJoint()) {
                float Xpos = parser->getFloat();
                float Ypos = parser->getFloat();
                float Zpos = parser->getFloat();
                mocapData->setTransXYZ(Xpos, Ypos, Zpos, numeroFrame, jointCourant);
                
            }        
            // Rotation (attention l'ordre de lecture est important)
            float Zrot = parser->getFloat();
            float Xrot = parser->getFloat();
            float Yrot = parser->getFloat();
            mocapData->setRotXYZ(Xrot, Yrot, Zrot, numeroFrame, jointCourant);
            
        }
    }
	



	// Detruit le parser (et ferme le fichier donc)
	delete parser;

    // Indique que le chargement s'est bien déroulé
	return true;
	
}

// Ecrit un joint dans le fichier fp préalablement ouvert
void BvhLoader::writeJointHeader(Joint * joint, FILE* fp, int decalage) {

	//Decalage
	for (int i=0; i<decalage; i++) {
		fprintf(fp,"\t");
	}

	// Nom du joint + type
	if (joint == skeletalData->getRootSqueleton()) {
		fprintf(fp, "ROOT %s\n", joint->getName().c_str());

	} else {
		fprintf(fp, "JOINT %s\n", joint->getName().c_str());
	}

	//Decalage
	for (int i=0; i<decalage; i++) {
		fprintf(fp,"\t");
	}

	// Debut de la structure
	fprintf(fp, "{\n");

	//Decalage
	for (int i=0; i<decalage+1; i++) {
		fprintf(fp,"\t");
	}

	// Offset
	fprintf(fp, "OFFSET %f %f %f\n", joint->getOffsetPos('X'),joint->getOffsetPos('Y'),joint->getOffsetPos('Z'));

	//Decalage
	for (int i=0; i<decalage+1; i++) {
		fprintf(fp,"\t");
	}

	// Channels
	fprintf(fp, "CHANNELS %d ", joint->getNbChannels());
	fprintf(fp, "%s\n", joint->getChannelsType().c_str());


	// Les fils s'il y en a
	list<Joint*> * listeCourante = joint->getListeEnfants();
	list<Joint*>::iterator it = listeCourante->begin();
	while (it != listeCourante->end()) {
		Joint * unenfant = *(it++);
		writeJointHeader(unenfant, fp, decalage +1);
	}

	// Le end site
	if (joint->isTerminal()) {
		decalage++;
		//Decalage
		for (int i=0; i<decalage; i++) {
			fprintf(fp,"\t");
		}		fprintf(fp, "End Site\n");
		//Decalage
		for (int i=0; i<decalage; i++) {
			fprintf(fp,"\t");
		}
		// Debut de la structure
		fprintf(fp, "{\n");
		//Decalage
		for (int i=0; i<decalage +1; i++) {
			fprintf(fp,"\t");
		}
		// Offset
		fprintf(fp, "OFFSET %f %f %f\n", joint->getOffsetEndPos('X'),joint->getOffsetEndPos('Y'),joint->getOffsetEndPos('Z'));
		//Decalage
		for (int i=0; i<decalage; i++) {
			fprintf(fp,"\t");
		}
		// Fin de la structure
		fprintf(fp, "}\n");
	
		decalage--;
	}
	
	//Decalage
	for (int i=0; i<decalage; i++) {
		fprintf(fp,"\t");
	}

	// Fin de la structure
	fprintf(fp, "}\n");
	
}

void BvhLoader::writeOneFrame(FILE* fp, int numeroFrame, Joint * joint) {
    // Ecriture du joint courant
    
    // Translation
    if (joint->isTransJoint()) {
        fprintf(fp, "%f\t%f\t%f\t", mocapData->getTrans(numeroFrame, 'X', joint),
                                    mocapData->getTrans(numeroFrame, 'Y', joint),
                                    mocapData->getTrans(numeroFrame, 'Z', joint)
                                    );
    }   
    fprintf(fp, "%f\t%f\t%f\t", mocapData->getRot(numeroFrame, 'Z', joint),
                                mocapData->getRot(numeroFrame, 'X', joint),
                                mocapData->getRot(numeroFrame, 'Y', joint)
                                );     
    // Puis de ses enfants
    list<Joint*> * listeCourante = joint->getListeEnfants();	
	for (list<Joint*>::iterator it = listeCourante->begin(); it != listeCourante->end(); it++) {
		// Recupération de l'enfant
		Joint * unEnfant = *it;
		writeOneFrame(fp, numeroFrame, unEnfant);
    }
}

// Ecriture du bvh
bool BvhLoader::write(const string nomFichier) {
    // Verifie les paramètres
    if (skeletalData == NULL || mocapData == NULL) return false;
    
	// Ouverture du fichier
	FILE *fp = fopen(nomFichier.c_str(),"wt");

	// ECRITURE DU SQUELETTE
	fprintf(fp, "HIERARCHY\n");

	writeJointHeader(skeletalData->getRootSqueleton(), fp, 0);

	// ECRITURE DES DONNEES

	// Descriptif des données
	fprintf(fp, "MOTION\n");
	fprintf(fp, "Frames: %d\n", mocapData->getNbFrames());
	fprintf(fp, "Frame Time: %f\n", mocapData->getDureeFrame());


	// Chaque frame ligne par ligne
	for (int j=0; j<mocapData->getNbFrames(); j++) {
        /*
		for (int i=0; i<mocapData->getNChannels(); i++) {
			fprintf(fp, "%f\t", mocapData->getRawData(j, i));
		}
		fprintf(fp, "\n");
		*/
		writeOneFrame(fp, j, skeletalData->getRootSqueleton());
		fprintf(fp, "\n");
	}

	// Fermeture du fichier
	fclose(fp);

	return true;
}

#include "ActorRenderSkin.h"

#include <IL/il.h>
//#include <IL/ilu.h>
#include <IL/ilut.h>


ActorRenderSkin::ActorRenderSkin(MocapData *mocapData, SkeletalData * skeletalData, SkinData * skinData):
    ActorRender(mocapData, skeletalData, skinData)
{
    this->mocapManipulator = new MocapManipulator(mocapData, skeletalData);

    if (skinData) {
        // Allocation des matrices de transformation
        int tailleTotale = skeletalData->getListeJointsById().size();
        matListAtFrameN = new Mat4f[tailleTotale+1]();
        inverseMatListAtRefPos = new Mat4f[tailleTotale+1]();

        // Liste des matrices de transformations pour chaque frame
        matListAtFrame = NULL;

        vertexListCorrectedAtFrame = NULL;

        MocapManipulator * mocapManipulatorRef = new MocapManipulator(skinData->getReferencePosData(), skeletalData);
        // Pré calcule les matrice inverses de transformation pour calculer le position initiale du modele
        computeInverseMatAtFrameN(0, mocapManipulatorRef, inverseMatListAtRefPos);
        delete mocapManipulatorRef;
    }

    // Valeur par défaut
    lastFrameRendered = -1;


}


ActorRenderSkin::~ActorRenderSkin(){
    delete mocapManipulator;

    // Libération mémoire des textures
	for (TextureIdByTextureName::iterator it = listeTextureId.begin(); it != listeTextureId.end(); it++) {
        glDeleteTextures(1, &it->second);
    }
}

void ActorRenderSkin::loadTextures(TextureIdByTextureName &listeTextureId) {

    // Initialisation de ilut
    ilInit() ;
    iluInit();
    ilutEnable(ILUT_OPENGL_CONV);
    ilutRenderer(ILUT_OPENGL);

    // Chargement de toute les textures dans la liste des textures

    // On parcours chaque triangle pour récuperer le nom de sa texture et la charger
    const SkinData::TriangleList& listeTriangle = skinData->getTriangleList();

    // Pour chaque triangle
	for(SkinData::TriangleList::const_iterator it2 = listeTriangle.begin(); it2 != listeTriangle.end(); it2++) {
        // Triangle courant
		const SkinData::Triangle& triangle = *it2;

        // Recupere le nom de la texture
        if (triangle.getTextureFileName() != "null.bmp") {
            // Si la texture n'existe pas déjà, on la charge et on stocke son id OpenGL
            if ((listeTextureId.find(triangle.getTextureFileName()) == listeTextureId.end())) {
                listeTextureId[triangle.getTextureFileName()] =
                    ilutGLLoadImage((char *)((skinData->getTexturesPath() + triangle.getTextureFileName()).c_str()));
                clog << "Chargement texture: "<< skinData->getTexturesPath() + triangle.getTextureFileName() << " ";
                if (ilGetError() == IL_NO_ERROR) {
                    clog << "... ok";
                } else {
                    clog << "... ECHEC";
                }
                clog << endl;
            }
        }


	}

}

void ActorRenderSkin::computeInverseMatAtFrameN(int numeroFrame, MocapManipulator * mocapManip, ActorRenderSkin::MatJointIndexedByJointId &inverseMatListAtFrameN) {

    // Recupere la liste de tout les joints
    SkeletalData::JointListIndexedById liste = skeletalData->getListeJointsById();
	for (SkeletalData::JointListIndexedById::iterator it = liste.begin(); it != liste.end(); it++) {

        // Calcul de la matrice de transformation inverse et ajout de la matrice dans la liste
		mocapManip->getInverseMatTransformation(numeroFrame,
                                                it->first,
                                                inverseMatListAtFrameN[it->first]);

	}
}


void ActorRenderSkin::computeMatAtFrameN(int numeroFrame, MocapManipulator * mocapManip, ActorRenderSkin::MatJointIndexedByJointId &matListAtFrameN) {
    // Allocation mémoire si celà n'a pas été fait
    if (!matListAtFrame) {
        matListAtFrame =  new MatJointIndexedByJointId[mocapData->getNbFrames()+1];


        for (int i=0; i<mocapData->getNbFrames()+1; i++) {

           matListAtFrame[i] = NULL;

        }
    }


    // Si l'ensemble de mat a déjà été computé pour la frame en cours, alors
    // on récupere juste un pointeur sur l'ensemble
    if (canOptimize() && (matListAtFrame[numeroFrame])) {

        matListAtFrameN = matListAtFrame[numeroFrame];
        return;
    }

    // Sinon allocation pour l'ensemble de mat pour la frame en cours
    int tailleTotale = skeletalData->getListeJointsById().size();
    matListAtFrame[numeroFrame] = new Mat4f[tailleTotale+1]();

    // Puis calcul de l'ensemble


   glPushMatrix();
    // Recupere la liste de tout les joints

    SkeletalData::JointListIndexedById liste = skeletalData->getListeJointsById();
	for (SkeletalData::JointListIndexedById::iterator it = liste.begin(); it != liste.end(); it++) {

		// Calcul de la matrice de transformation et stockage
		//mocapManip->getPositionJointByNameInFrame(numeroFrame, it->first, false, &matListAtFrameN[skeletalData->getJointByName(it->first)->getId()], true);
		mocapManip->getJointMatriceTransformation(numeroFrame,
                                                it->first,
                                                matListAtFrame[numeroFrame][it->first], false, true);

        // On effectue la multiplication par la matrice de transformation inverse
        //matListAtFrameN[it->first] = inverseMatListAtRefPos[it->first] *  matListAtFrameN[it->first];


        glLoadMatrixf(&matListAtFrame[numeroFrame][it->first]);
        glMultMatrixf(&inverseMatListAtRefPos[it->first]);
        glGetFloatv(GL_MODELVIEW_MATRIX, &matListAtFrame[numeroFrame][it->first]);

        matListAtFrameN = matListAtFrame[numeroFrame];


	}
	glPopMatrix();
}



void ActorRenderSkin::drawSkinnedVertex(const SkinData::Vertex &vertex, const int numeroFrame) {

    // La position apres correction du vertex
    Point3D posVertexCorrige = Point3D(0,0,0);
    //Point3D normCorrigee = Point3D(0,0,0);

    if (!canOptimize() || (vertexListCorrectedAtFrame[numeroFrame][&vertex].norme() == 0)) {

        // On récupere la liste des influences du vertex
        const SkinData::Vertex::WeightIndexedByBoneId  &liste = vertex.getWeightList();
        for (SkinData::Vertex::WeightIndexedByBoneId::const_iterator it = liste.begin(); it != liste.end(); it++) {
            // On récupere les 2 paramètres de la map
            int boneIndex = it->first;
            float influence = it->second;

            // On récupere le joint courant
           // Joint * currentJoint = skeletalData->getJointById(boneIndex);

            // Et la position du vertex
            Point3D pVertex = Point3D(vertex.getPosX(), vertex.getPosY(), vertex.getPosZ());
            //Point3D pNorm = Point3D(vertex.getNormX(), vertex.getNormY(), vertex.getNormZ());

            // On recupere la matrice de transformation inverse du joint courant
            // On calcule alors la position du vertex dans le repere local du joint dans la pos de Ref
            // On passe ensuite ce vertex dans le repere monde par la tranformation du joint courant
            // a la frame en cours
            pVertex = pVertex * matListAtFrameN[boneIndex];
            //pNorm = pNorm * matListAtFrameN[boneIndex];

            // On applique l'influence du joint en cours et on passe au joint influenceur suivant
            posVertexCorrige = posVertexCorrige.add(pVertex.mult(influence));
            //normCorrigee = normCorrigee.add(pNorm.mult(influence));


        }

        // On enregistre le vertex nouvellement corrigé pour la frame courante
        if (canOptimize())
            vertexListCorrectedAtFrame[numeroFrame][&vertex] = posVertexCorrige;

    } else {

        // Sinon on restaure la position qui a déjà été calculée
        posVertexCorrige = vertexListCorrectedAtFrame[numeroFrame][&vertex];

    }



    // Affichage normale + vertex
    // Remarque: on ne corrige pas la normale mais on devrait :)
    glTexCoord2f(vertex.getTextU(), vertex.getTextV());
    glNormal3f(vertex.getNormX(),vertex.getNormY(),vertex.getNormZ());
    //glNormal3f(normCorrigee.getX(),normCorrigee.getY(),normCorrigee.getZ());
    glVertex3d(posVertexCorrige.getX(),posVertexCorrige.getY(),posVertexCorrige.getZ());


}


void ActorRenderSkin::drawFrame(int numeroFrame) {


	glPushMatrix();



	// Activation des textures
	//glPushAttrib(GL_TEXTURE_2D);
//    glEnable(GL_TEXTURE_2D);

    // Fixe l'echelle de rendu
    glScalef(mocapData->getEchelle(),mocapData->getEchelle(),mocapData->getEchelle());
    if (skinData != NULL) {

        // Chargement des textures
        if (listeTextureId.empty())
            loadTextures(listeTextureId);

        // La liste des positions des joints à la frame courant (seulement si le numéro de frame a changé)
        // Sinon on garde l'ancien calcul
        if (lastFrameRendered != numeroFrame) {
            //matListAtFrameN.clear();
            computeMatAtFrameN(numeroFrame, mocapManipulator, matListAtFrameN);

        }


        // Affiche le mesh
        // Récupere la liste des triangles du modele
        const SkinData::TriangleList& listeTriangle = skinData->getTriangleList();


        // OPTIMISATION
        // Liste des vertex par frame
        if (!vertexListCorrectedAtFrame && canOptimize()) {
            vertexListCorrectedAtFrame = new VertexListCorrectedAtFrameN[mocapData->getNbFrames()];

        }



        // Pour chaque triangle
        glPushMatrix();
        glRotatef(-90,0,1,0);
        glColor3f(1.0,1.0,1.0);
    	glBegin(GL_TRIANGLES);

        GLuint lastTextureId = 0;
    	for (SkinData::TriangleList::const_iterator it2 = listeTriangle.begin();it2 != listeTriangle.end(); it2++) {
            // Triangle courant
    		const SkinData::Triangle& triangle = *it2;

            // Binde la texture courante
            if (listeTextureId.find(triangle.getTextureFileName()) != listeTextureId.end()) {
                if (lastTextureId != listeTextureId[triangle.getTextureFileName()]) {
                    glEnd();
                    glEnable(GL_TEXTURE_2D);
                    lastTextureId = listeTextureId[triangle.getTextureFileName()];
                    glBindTexture(GL_TEXTURE_2D, listeTextureId[triangle.getTextureFileName()]);
                    glBegin(GL_TRIANGLES);
                }

            } else {
                // Aucune texture
                glEnd();
                glDisable(GL_TEXTURE_2D);
                glColor3f(1.0,1.0,1.0);
                glBegin(GL_TRIANGLES);
            }


            // Affichage du permier vertex
            drawSkinnedVertex(triangle.getVertex1(), numeroFrame);

            // Affichage du second vertex
            drawSkinnedVertex(triangle.getVertex2(), numeroFrame);

            // Affichage du troisième vertex
            drawSkinnedVertex(triangle.getVertex3(), numeroFrame);


    	}

    	glEnd();
        glPopMatrix();

    }
	//glPopAttrib();

    // Affichage du squelette par dessus le modèle (sans test de profondeur)
    // Affiche la racine et ses sous branches récursivement
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_DEPTH_TEST);
    drawJoint(numeroFrame, skeletalData->getRootSqueleton());
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);

	glPopMatrix();

	// On fixe la derniere frame rendu
	lastFrameRendered = numeroFrame;

}

void ActorRenderSkin::drawJoint(int numeroFrame, Joint * joint) {


	glPushMatrix();

	// Recupère les offsets du joint
	float offsetPosX = joint->getOffsetPos('X');
	float offsetPosY = joint->getOffsetPos('Y');
	float offsetPosZ = joint->getOffsetPos('Z');

	// Dessine une ligne du parent jusqu'au joint courant (sauf pour la racine
	// qui n'a pas de parent)
	if (joint->getId() != skeletalData->getRootSqueleton()->getId()) {
    	glLineWidth(1);
    	glColor3f(1,1,1);
    	glBegin(GL_LINES);
    	glVertex3f(0,0,0);
    	glVertex3f(offsetPosX, offsetPosY, offsetPosZ);
    	glEnd();
    }
    /*
	// S'il ne s'agit pas de la racine, on trace le membre
	if (!joint->isTransJoint()) {


		// On se place au milieu du membre
		Point3D p1 = Point3D(offsetPosX, offsetPosY, offsetPosZ);
		glPushMatrix();

		glTranslatef(offsetPosX/2,offsetPosY/2, offsetPosZ/2);

        float alpha = atan2(offsetPosY, offsetPosX)*180./3.14;
		// On commence par aligner le repère avec le membre sur 1 axe
		glRotatef(alpha,0,0,1);

		// On va maintenant calculer les nouvelles coordonnées du Point(a,b,c)
		// pour finir l'aligement
		glPushMatrix();

		glLoadIdentity();
		glRotatef(alpha,0,0,1);

		Point3D ptemp = Point3D(offsetPosX, offsetPosY, offsetPosZ);
		GLfloat buf[16];
		glGetFloatv(GL_MODELVIEW_MATRIX,buf);
		ptemp.multVecteurMatrice(buf);

		glPopMatrix();

		// On finit l'alignement
		// Remarque: le calcul précédent sert car
		// glRotatef(atan2(c,a)*180./3.14,0,0,1) est incorrect étant
		// donné la précédente rotation
		glRotatef(atan2(ptemp.getZ(),ptemp.getX())*180./3.14,0,1,0);


		// On affiche le membre
		glScalef(p1.norme()/2,2,2);
		glColor3f(p1.norme()/20,0.1,0.3);
		glutSolidSphere(sphereSize,10,5);


		//if (name == "Head") {
			//glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); // Polygon rasterization mode (polygon outlined)
			//g_3DModel.Draw();
		//	g_3DModel.DrawPart("Head");
		//}



		glPopMatrix();
	}
	*/

	// Appliquer les transformation du joint (translation et rotation)
	this->appliquerTransformation(numeroFrame, joint);


    // Affiche le joint
	glColor3f(0,1,0);
	GLUquadric * quadric = gluNewQuadric();
	gluQuadricTexture(quadric, GL_TRUE);
	gluSphere(quadric, sphereSize, 4, 4);
	gluDeleteQuadric(quadric);


	// Dans le cas d'un terminal
	if (joint->isTerminal()) {
    	float endLength[3];
    	endLength[0] = joint->getOffsetEndPos('X');
    	endLength[1] = joint->getOffsetEndPos('Y');
    	endLength[2] = joint->getOffsetEndPos('Z');

		glLineWidth(1);
		glColor3f(0.8,0.4,0);
		glBegin(GL_LINES);
		glVertex3f(0,0,0);
		glVertex3f(endLength[0],endLength[1],endLength[2]);
		glEnd();

		glTranslatef(endLength[0],endLength[1],endLength[2]);
		glColor3f(0,0,1);
		GLUquadric * quadric = gluNewQuadric();
		gluQuadricTexture(quadric, GL_TRUE);
		gluSphere(quadric, sphereSize, 4, 4);
		gluDeleteQuadric(quadric);
	}


    // Affiche récursivement les autres sous-joints
	list<Joint*>::iterator it = joint->getListeEnfants()->begin();
	while (it != joint->getListeEnfants()->end()) {
		Joint * unEnfant = *(it++);
		drawJoint(numeroFrame, unEnfant);
	}

	glPopMatrix();


}

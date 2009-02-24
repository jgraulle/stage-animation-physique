#include "ActorRenderFil.h"

//glColor3f(0.8,0.6,0.4);

ActorRenderFil::ActorRenderFil(MocapData *mocapData, SkeletalData *skeletalData, SkinData *skinData):
    ActorRender(mocapData, skeletalData, skinData)
{

}


ActorRenderFil::~ActorRenderFil(){

}

void ActorRenderFil::drawFrame(int numeroFrame) {
	glPushMatrix();

    // Fixe l'echelle de rendu
	glScalef(mocapData->getEchelle(),mocapData->getEchelle(),mocapData->getEchelle());

    // Affiche la racine et ses sous branches récursivement
    drawJoint(numeroFrame, skeletalData->getRootSqueleton());

	glPopMatrix();

}

void ActorRenderFil::drawJoint(int numeroFrame, Joint * joint) {

	glPushMatrix();

	// Recupère les offsets du joint
	float offsetPosX = joint->getOffsetPos('X');
	float offsetPosY = joint->getOffsetPos('Y');
	float offsetPosZ = joint->getOffsetPos('Z');

	// Dessine une ligne du parent jusqu'au joint courant
	if (joint->getId() != skeletalData->getRootSqueleton()->getId()) {
        glLineWidth(1);
    	glColor3f(1,1,1);
    	glBegin(GL_LINES);
    	glVertex3f(0,0,0);
    	glVertex3f(offsetPosX, offsetPosY, offsetPosZ);
    	glEnd();
    }

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
		GLUquadric * quadric = gluNewQuadric();
		gluQuadricTexture(quadric, GL_TRUE);
		gluSphere(quadric, sphereSize, 10, 5);
		gluDeleteQuadric(quadric);


		//if (name == "Head") {
			//glPolygonMode (GL_FRONT_AND_BACK, GL_LINE); // Polygon rasterization mode (polygon outlined)
			//g_3DModel.Draw();
		//	g_3DModel.DrawPart("Head");
		//}



		glPopMatrix();
	}


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

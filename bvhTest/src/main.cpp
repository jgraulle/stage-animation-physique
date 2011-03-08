/*
 * main.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Perso.h"

#include <MoteurGraphique.h>
#include <GL/glfw.h>

Perso * perso;
bool update = true;

// gestion du drag and drop de la souris
void gestionSouris(Camera & camera) {
	// coordonnee de la souris courrante
	int x, y;
	glfwGetMousePos(&x, &y);
	int cran = glfwGetMouseWheel();

	// variables conservant l'ancienne position de la souris
	static int sourisX=x, sourisY=y, cranAnc = cran;

	// si la souris a bouge
	if (sourisX!=x || sourisY!=y) {
		// si le bouton gauche est presse
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
			camera.addRot((double)(x-sourisX), (double)(y-sourisY));
		}
		// si le bouton droit est presse
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3((double)(x-sourisX)*camera.getDistance()/200.0f, 0.0, (double)(y-sourisY)*camera.getDistance()/200.0f));
		}
		// si le bouton du milieu est presse
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3(0.0, (double)(sourisY-y)*camera.getDistance()/200.0f, 0.0));
		}
	}
	// si la molette de la souris a tourne
	if (cranAnc != cran) {
		camera.addDistance((double)(cranAnc-cran)*camera.getDistance()/5.0f);
	}

	// si une des touche fleche du clavier est presse
	if (glfwGetKey(GLFW_KEY_UP)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, -0.2));
	if (glfwGetKey(GLFW_KEY_DOWN)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, 0.2));
	if (glfwGetKey(GLFW_KEY_LEFT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(-0.2, 0.0, 0.0));
	if (glfwGetKey(GLFW_KEY_RIGHT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.2, 0.0, 0.0));

	// mise a jour de l'ancienne position de la souris
	sourisX = x;
	sourisY = y;
	cranAnc = cran;
}

void gestionTouche(int touche, int etat) {
	if (etat==GLFW_PRESS) {
		switch (touche) {
		case 'R':
			perso->changeTransformationRoot();
			break;
		case 'T':
			perso->changeTranslationChildren();
			break;
		case 'U':
			update = !update;
			break;
		case 'D':
			perso->nextFrame();
			break;
		case 'Q':
			perso->previousFrame();
			break;
		}
	}
}

int main(int argc, char * argv[]) {
	// verifier que le nom du bvh a ouvrir est bu en parametre
	if (argc!=2) {
		cerr << "usage : " << argv[0] << " fileName.bvh" << endl;
		return -1;
	}
	const char * bvhFileName = argv[1];

	try {
		// creation du monde
		Moteur * moteur = Moteur::getInstance();
		moteur->setWindowName("bvhViewer");
		Monde3D * monde3D = moteur->getMonde3D();

		// camera
		Camera * camera = monde3D->getCamera();
		camera->setCentreObser(Vector3(0.0f, 0.0f, 0.0f));
		Quaternion direction = Quaternion::IDENTITY;
		camera->setDistance(100.0);
		camera->setRot(0.0, 0.0);

		// ajout des objets aux mondes
		perso = new Perso(bvhFileName, Material());
		monde3D->add("perso", perso);

		glfwSetKeyCallback(gestionTouche);

		// affichage du monde
		do {
			if (update)
				moteur->update();
			gestionSouris(*camera);
			moteur->display();
			moteur->swapBuffer();
		} while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

	} catch (Erreur e) {
		cout << e.what() << endl;
	} catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}


/*
 * main.cpp
 *
 *  Created on: 26 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Perso.h"

#include <MoteurGraphique.h>
#include <GL/glfw.h>

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
			camera.moveCentreObser(Vector3((double)(x-sourisX)/10, 0.0, (double)(y-sourisY)/10));
		}
		// si le bouton du milieu est presse
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3(0.0, (double)(sourisY-y)/10, 0.0));
		}
	}
	// si la molette de la souris a tourne
	if (cranAnc != cran) {
		camera.addDistance((double)(cranAnc-cran));
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

int main() {
	try {
		// creation du monde
		Moteur * moteur = Moteur::getInstance();
		Monde3D * monde3D = moteur->getMonde3D();
		Monde2D * monde2D = moteur->getMonde2D();

		// camera
		Camera * camera = monde3D->getCamera();
		camera->setCentreObser(Vector3(0.0f, 0.0f, 0.0f));
		Quaternion direction = Quaternion::IDENTITY;
		camera->setDistance(10);
		camera->setRot(0.0, 0.0);

		// ajout des lumieres
		monde3D->add("lumiere0", new Lumiere(Vector3(10.0,10.0,10.0), Couleur::BLANC));

		// chargement des ressources
		const Image * im = ImagesManager::getInstance()->add(new Image("data/cube.png"));
		TexturesManager::getInstance()->add(new Texture("data/cube.png", true));
		Material mat("data/cube.png");
		MeshManager::getInstance()->add(new Cube("cube"));
		MeshManager::getInstance()->add(new Sphere("sphere", 16, 8));

		// ajout des objets aux mondes
		monde3D->add("cube", new Objet3D(mat, "cube", Transform(Vector3(-2.0,0.0,0.0))));
		monde3D->add("sphere", new Objet3D(mat, "sphere", Transform(Vector3(2.0,0.0,0.0), Quaternion(), Vector3(1.0, 1.0, 1.0))));
		monde3D->add("perso1", new Perso("data/Example1.bvh", mat, Transform(Vector3(-2.0,0.0,0.0), Quaternion(), Vector3(0.1, 0.1, 0.1))));
		monde3D->add("perso2", new Perso("data/walk.bvh", mat, Transform(Vector3(2.0,0.0,0.0), Quaternion(), Vector3(0.1, 0.1, 0.1))));

		// affichage du monde
		do {
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


/*
 * main.cpp
 *
 *  Created on: 5 fevr. 2009
 *      Author: jeremie
 */

#include <iostream>
#include <sstream>
#include <IL/il.h>
#include <IL/ilut.h>
#include <GL/glfw.h>

#include "../../moteurGraphique/src/MoteurGraphique.h"
#include "../../mocap/src/Bvh.h"
#include "../../mocap/src/ActorRenderSkin.h"

class Personnage: public Objet3D {
public:
	Personnage(Material material, const string fileName, Transform transform = Transform())
	: Objet3D(material, NULL, transform), tempsAnim(0.0), numFrame(0) {
		// chargement de la mocap
		bvh = new Bvh(fileName);
	    actorRenderSkin = new ActorRenderSkin(bvh->getMocapData(), bvh->getSkeletalData(), bvh->getSkinData());
	    actorRenderFil = new ActorRenderFil(bvh->getMocapData(), bvh->getSkeletalData(), bvh->getSkinData());
	}
	virtual ~Personnage() {
		delete actorRenderFil;
		delete actorRenderSkin;
		delete bvh;
	}
	virtual void update(f32 elapsed) {
		tempsAnim += elapsed;
		numFrame = int(tempsAnim*25.0);
		if (numFrame>=bvh->getNbFrame()) {
			numFrame = 0;
			tempsAnim = 0.0f;
		}
	}
	virtual void display() const {
		actorRenderSkin->drawFrame(numFrame);
	}
private:
	Bvh * bvh;
	ActorRenderSkin * actorRenderSkin;
	ActorRenderFil * actorRenderFil;
	f32 tempsAnim;
	int numFrame;
};

using namespace std;

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

int main(int argc, char **argv) {
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
//		monde3D->add("lumiere1", new Lumiere(Vector3(0.0,0.0,3.0), Couleur::ROUGE, 1.0f, 0.0f, 0.0f, 0, 20.0f, Vector3(0.0f, 0.0f, -1.0f)));

		// chargement des ressources
		const Image * im = ImagesManager::getInstance()->add(new Image("data/cube.png"));
		TexturesManager::getInstance()->add(new Texture("data/cube.png", true));
		Material mat("data/cube.png");
		MeshManager::getInstance()->add(new Cube("cube"));
		MeshManager::getInstance()->add(new Sphere("sphere", 16, 8));
		f32 positions[][3] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
		f32 normals[][3] = {{0.0f, 0.0f, 1.0f}};
		f32 texCoords[][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
		u16 faces[][3][3] = {{{0,1,2},{0,0,0},{0,1,2}}};
		MeshManager::getInstance()->add(new Mesh("testMesh", positions, normals, texCoords, faces, 1));

		// ajout des objets aux mondes
		monde3D->add("cube", new Objet3D(mat, "cube", Transform(Vector3(-4.0,0.0,0.0))));
		for (int i=0; i<10; i++) {
			for (int j=0; j<10; j++) {
				char tampon[80];
				sprintf(tampon, "sphere%2d%2d", i, j);
				monde3D->add(tampon, new Objet3D(mat, "sphere", Transform(Vector3(i,j,0.0), Quaternion(), Vector3(0.4, 0.4, 0.4))));
			}
		}
		monde3D->add("testMesh", new Objet3D(mat, "testMesh", Transform(Vector3(4.0,0.0,0.0))));
		monde3D->add("perso", new Personnage(mat, "data/carl_anim.smd", Transform(Vector3(4.0,0.0,2.0), Quaternion(), Vector3(0.1, 0.1, 0.1))));
//		monde2D->add("care", 1, new Objet2D("cube.png", Vecteur2(100.0f, 100.0f), Vecteur2(100.0f, 100.0f)));

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

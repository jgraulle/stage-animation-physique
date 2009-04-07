/*
 * Moteur.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Moteur.h"

#include "Math/Random.h"

#include <IL/il.h>
#include <sstream>
#include <GL/glfw.h>

using namespace std;

// acceder a la seule instance creer du moteur
Moteur * Moteur::getInstance() {
	static Moteur moteur;
	return &moteur;
}

// constructeur
Moteur::Moteur()
: background(0.0f,0.0f,0.0f,1.0f) {
	// initialisation de la librairie devIL
	ilInit();
	// initialisation de la librairie GLFW
	glfwInit();
	// creation d'une fenetre
	if(!glfwOpenWindow(800, 600, 8,8,8,8, 24,0, GLFW_WINDOW)) {
		glfwTerminate();
		exit(-1);
	}
	tailleEcran.set(800.0f, 600.0f);
	glfwSetWindowTitle("MoteurGraphique");
	glfwSwapInterval(1);
//	glfwDisable(GLFW_MOUSE_CURSOR);
	glClearColor(background.getR(), background.getV(), background.getB(), background.getA());
	glEnable(GL_DEPTH_TEST);		// test de profondeur avec le z-buffer
	glEnable(GL_CULL_FACE);			// trie des faces
	glEnable(GL_TEXTURE_2D);		// activer les textures
	glShadeModel(GL_SMOOTH);		// activer le calcul de la lumieres sur les objets de maniere smooth
	glEnable(GL_LIGHTING);			// activer la lumiere
	glEnable(GL_BLEND);				// activer la transprence
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//	glEnable(GL_ALPHA_TEST);		// activer une fonction de test pour la transparence
//	glAlphaFunc(GL_GREATER, 0.0);	// les objets completement transparent ne mettent pas a jour le z-buffer
	// initialisation du generateur de nombre aleatoire
	Random::init();
	// creation des mondes
	monde3D = new Monde3D(tailleEcran);
	monde2D = new Monde2D(tailleEcran);
}

// destructeur
Moteur::~Moteur() {
	glfwTerminate();
	delete monde3D;
	delete monde2D;
}

// taille de l'ecran en pixel
const Vecteur2 & Moteur::getTailleEcran() const {
	return tailleEcran;
}
void Moteur::setTailleEcran(const Vecteur2 & t) {
	tailleEcran = t;
	monde2D->setTailleProjection(tailleEcran);
	glfwSetWindowSize(int(t.getX()), int(t.getY()));
	glViewport(0, 0, int(t.getX()), int(t.getY()));
}

// met a jour tous les elements
void Moteur::update() {
	// Calcul des FPS
	static double t, t0 = glfwGetTime(), tAnc = glfwGetTime();
	static int fps = 0, frames = 0;
	t = glfwGetTime();
	if( (t-t0) > 1.0) {
		fps = (int)((double)frames / (double)(t-t0));
		t0 = t;
		frames = 0;
		ostringstream oss;
		oss << "moteur - " << fps;
		glfwSetWindowTitle(oss.str().c_str());
	}
	frames++;
	elapsed = t-tAnc;
	tAnc = t;
	monde2D->update(elapsed);
	monde3D->update(elapsed);
}

f32 Moteur::getElapsed() {
	return elapsed;
}

// echanger les 2 buffer et afficher le buffer courrant
void Moteur::swapBuffer() {
	glfwSwapBuffers();
}

// accesseur
Monde2D * Moteur::getMonde2D() {
	return monde2D;
}
Monde3D * Moteur::getMonde3D() {
	return monde3D;
}

// affiche le monde 2D et le monde 3D
void Moteur::display() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	// afficher la 3d avant la 2D si non les pixel transparent de la 2D vont quand meme ecrire le z buffer
	// du cout les objet 3D qui sont derriere ne vont pas passer le test du z buffer
	// regle : afficher les objets transparent en dernier ou alors desactiver l'ecriture du z-buffer lors de leurs affichage
	monde3D->display();
//	monde2D->display();
	// Affichage, s'il y a, des erreurs OpenGL
	GLenum error = glGetError();
	if(error != GL_NO_ERROR)
		cout << "Erreur OpenGL : " << gluErrorString(error) << endl;
}

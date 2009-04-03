/*
 * main.cpp
 *
 *  Created on: 12 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#define CONETWIST_USE_OBSOLETE_SOLVER false

#include "TransformConv.h"
#include "DebugDraw.h"
#include "RagDoll.h"
#include "SkeletonMesh.h"

#include <iostream>
#include <sstream>
#include <GL/glfw.h>
#include "../../moteurGraphique/src/MoteurGraphique.h"
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

using namespace std;

bool playPhysique = false;
bool displayPhysique = false;
DebugDraw * debugDrawer;

btRigidBody::btRigidBodyConstructionInfo createRigidBodyInfo(btScalar mass, Transform & transform, btCollisionShape* collisionShape) {
	btVector3 inertie;
	collisionShape->calculateLocalInertia(mass, inertie);
	return btRigidBody::btRigidBodyConstructionInfo(mass, (btMotionState*)new TransformConv(&transform), collisionShape, inertie);
}

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
			camera.moveCentreObser(Vector3((double)(x-sourisX)/10.0, 0.0, (double)(y-sourisY)/10.0));
		}
		// si le bouton du milieu est presse
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3(0.0, (double)(sourisY-y)/10.0, 0.0));
		}
	}
	// si la molette de la souris a tourne
	if (cranAnc != cran) {
		camera.addDistance((double)(cranAnc-cran)*2.0);
	}

	// si une des touche fleche du clavier est presse
	if (glfwGetKey(GLFW_KEY_UP)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, -0.02));
	if (glfwGetKey(GLFW_KEY_DOWN)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.0, 0.0, 0.02));
	if (glfwGetKey(GLFW_KEY_LEFT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(-0.02, 0.0, 0.0));
	if (glfwGetKey(GLFW_KEY_RIGHT)==GLFW_PRESS)
		camera.moveCentreObser(Vector3(0.02, 0.0, 0.0));

	// mise a jour de l'ancienne position de la souris
	sourisX = x;
	sourisY = y;
	cranAnc = cran;
}

void gestionTouche(int touche, int etat) {
	if (etat==GLFW_PRESS) {
		switch (touche) {
		case GLFW_KEY_SPACE:
				playPhysique=!playPhysique;
				break;
		case 'D':
			displayPhysique=!displayPhysique;
			break;
		case 'W':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawWireframe);
			break;
		case 'C':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawConstraints);
			break;
		case 'L':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawConstraintLimits);
			break;
		}
	}
}

int main(int argc, char **argv) {
	try {
		// creation du monde
		Moteur * moteur = Moteur::getInstance();
		Monde3D * monde3D = moteur->getMonde3D();

		// camera
		Camera * camera = monde3D->getCamera();
		camera->setCentreObser(Vector3(0.0f, 3.0f, 0.0f));
		Quaternion direction = Quaternion::IDENTITY;
		camera->setDistance(20);
		camera->setRot(0.0, 45.0);

		// ajout des lumieres
		monde3D->add("lumiere0", new Lumiere(Vector3(10.0,10.0,10.0), Couleur::BLANC));
		monde3D->add("lumiere1", new Lumiere(Vector3(-10.0,10.0,-10.0), Couleur::BLANC));

		// chargement des ressources
		ImagesManager::getInstance()->add("cube", new Image("data/cube.png"));
		TexturesManager::getInstance()->add("cube", new Texture("cube", true));
		Material mat("cube");
		MeshManager::getInstance()->add("cube", new Cube());

		// creation du monde physique
		btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
		btAxisSweep3 * overlappingPairCache = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
		btSequentialImpulseConstraintSolver * solver = new btSequentialImpulseConstraintSolver;
		btDiscreteDynamicsWorld * dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
		debugDrawer = new DebugDraw();
		dynamicsWorld->setDebugDrawer(debugDrawer);

		// ajout du sol graphique
		Quaternion quat = Quaternion::IDENTITY;
		Objet3D * solGraphique = new Objet3D(mat, "cube", Transform(Vector3(0.0,-5.0,0.0), quat, Vector3(50.0, 5.0, 50.0)));
		monde3D->add("sol", solGraphique);
		// ajout du sol physique
		btRigidBody * solPhysique = new btRigidBody(createRigidBodyInfo(0.0, solGraphique->getTransform(), new btBoxShape(btVector3(50.0, 5.0, 50.0)*SCALING)));
		dynamicsWorld->addRigidBody(solPhysique);

		// chargement des bvh
		Quaternion q = Quaternion::IDENTITY;
		q.FromAngleAxis(-M_PI_2, Vector3::UNIT_Y);
		SkeletonMesh * skeletonMeshA = new SkeletonMesh("data/walk.bvh", q, 0.1f, false);

		list<RagDoll*> ragDolls;
		for (int i=0; i<1; i++) {
			ostringstream buf;
			buf << "perso.a." << i << '.';
			ragDolls.push_back(new RagDoll(buf.str(), skeletonMeshA, mat, Transform(Vector3((i%10)*3.0, 5.0, (i/10.0)*3.0)), dynamicsWorld, monde3D));
		}

		glfwSetKeyCallback(gestionTouche);

		// affichage du monde
		do {
			moteur->update();
			if (playPhysique) {
				dynamicsWorld->stepSimulation(moteur->getElapsed());
				for (list<RagDoll*>::iterator it = ragDolls.begin(); it!=ragDolls.end(); it++)
					(*it)->update(moteur->getElapsed());
			}
			gestionSouris(*camera);
			moteur->display();
			glLoadIdentity();
			moteur->getMonde3D()->getCamera()->positionner();
			if (displayPhysique)
				dynamicsWorld->debugDrawWorld();
			moteur->swapBuffer();
		} while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

		delete dynamicsWorld;
		delete skeletonMeshA;
		// TODO delete de tous les elments proprement

	} catch (Erreur e) {
		cout << e.what() << endl;
	} catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}

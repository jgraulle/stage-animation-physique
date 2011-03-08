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
#include "MotionSave.h"
#include "SkeletonMesh.h"

#include <iostream>
#include <sstream>
#include <GL/glfw.h>
#include <MoteurGraphique.h>
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <IL/il.h>
#include <IL/ilu.h>
#include <IL/ilut.h>

using namespace std;

const int NUM_VERTS_X = 20;
const int NUM_VERTS_Y = 20;
const int NUM_VERTS = NUM_VERTS_X*NUM_VERTS_Y;
const float TRIANGLE_SIZE = 2.0f;
const int NUM_TRIANGLES = 2*(NUM_VERTS_X-1)*(NUM_VERTS_Y-1);
static float WAVE_HEIGHT = 0.3f;

f32 positions[NUM_VERTS][3];
f32 normals[NUM_TRIANGLES][3];
f32 texCoords[][2] = {{0.0f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f}, {1.0f, 0.0f}};
int faces[NUM_TRIANGLES][3][3];

btDiscreteDynamicsWorld * dynamicsWorld;
bool playPhysique = true;
DebugDraw * debugDrawer;
int nbrProjectile = 0;
f32 vitesseProjectile = 10.0;

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
		case GLFW_KEY_SPACE:
			{
				int x, y;
				glfwGetMousePos(&x, &y);
				Material mat = Material("cube", Couleur(0.0,0.0,0.0), Couleur(0.8,0.8,0.8), Couleur(0.0,0.0,0.0));
				Transform transCam = Transform(Moteur::getInstance()->getMonde3D()->getCamera()->getCentreObser(), Quaternion(-Moteur::getInstance()->getMonde3D()->getCamera()->getRotX()*M_PI/180.0, Vector3::UNIT_Y), Vector3(0.25,0.25,0.25));
				Objet3D * projectileGraphique = new Objet3D(mat, "cube", transCam);
				nbrProjectile++;
				ostringstream tampon;
				tampon << "projectile" << nbrProjectile;
				Moteur::getInstance()->getMonde3D()->add(tampon.str(), projectileGraphique);
				btCollisionShape * projectileForme = new btBoxShape(btVector3(0.25, 0.25, 0.25));
				btRigidBody * projectilePhysique = new btRigidBody(createRigidBodyInfo(20.0f, projectileGraphique->getTransform(), projectileForme));
				Quaternion q = Quaternion(-Moteur::getInstance()->getMonde3D()->getCamera()->getRotX()*M_PI/180.0, Vector3::UNIT_Y);
				Vector3 d = q * -Vector3::UNIT_Z;
				projectilePhysique->setLinearVelocity(TransformConv::graphToBt(d * vitesseProjectile));
				dynamicsWorld->addRigidBody(projectilePhysique);
			}
			break;
		case 'P':
			playPhysique=!playPhysique;
			break;
		case 'D':
			if (debugDrawer->isDansLeMonde())
				debugDrawer->retirerDuMonde();
			else
				debugDrawer->remettreDansMonde();
			break;
		case 'W':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawWireframe);
			break;
		case 'X':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawConstraints);
			break;
		case 'C':
			debugDrawer->modDebugMode(btIDebugDraw::DBG_DrawConstraintLimits);
			break;
		}
	}
}

int main(int argc, char **argv) {
	try {
		// creation du monde
		Moteur * moteur = Moteur::getInstance();
		moteur->setBackgroundColor(Couleur::NOIR);
		moteur->setWindowName("bulletTest");
		Monde3D * monde3D = moteur->getMonde3D();

		// camera
		Camera * camera = monde3D->getCamera();
		camera->setCentreObser(Vector3(0.0f, 1.0f, 0.0f));
		Quaternion direction = Quaternion::IDENTITY;
		camera->setDistance(5);
		camera->setRot(0.0, 0.0);

		// ajout des lumieres
		monde3D->add("lumiere0", new Lumiere(Vector3(10.0,10.0,10.0), Couleur(0.5,0.5,0.5), Couleur(0.9,1.0,0.9), Couleur(0.0,0.0,0.0)));
		monde3D->add("lumiere1", new Lumiere(Vector3(-10.0,10.0,-10.0), Couleur(0.5,0.5,0.5), Couleur(0.9,0.9,1.0), Couleur(0.0,0.0,0.0)));

		// chargement des ressources
		ImagesManager::getInstance()->add("cube", new Image("data/cube.png"));
		ImagesManager::getInstance()->add("sol", new Image("data/sol.jpg"));
		TexturesManager::getInstance()->add("cube", new Texture("cube", true));
		TexturesManager::getInstance()->add("sol", new Texture("sol", true));
		Material mat = Material("cube", Couleur(0.0,0.0,0.0), Couleur(0.8,0.8,0.8), Couleur(0.0,0.0,0.0));
		Material matSol = Material("sol", Couleur(0.0,0.0,0.0), Couleur(0.8,0.8,0.8), Couleur(0.0,0.0,0.0));
		MeshManager::getInstance()->add("cube", new Cube());
		MeshManager::getInstance()->add("cone", new Cone(32));

		// creation du monde physique
		btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
		btAxisSweep3 * overlappingPairCache = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
		btSequentialImpulseConstraintSolver * solver = new btSequentialImpulseConstraintSolver;
		dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);

		// ajout du debug physique
		debugDrawer = new DebugDraw(dynamicsWorld);
		monde3D->add("debugPhysique", debugDrawer, true);

		// ajout du sol graphique
		int i = 0;
		for (int x=0; x<NUM_VERTS_X; x++) {
			for (int y=0; y<NUM_VERTS_Y; y++) {
				positions[i][0] = x*TRIANGLE_SIZE-TRIANGLE_SIZE*(NUM_VERTS_X-1)*0.5;
				positions[i][1] = WAVE_HEIGHT*sinf((float)x)*cosf((float)y);
				positions[i][2] = y*TRIANGLE_SIZE-TRIANGLE_SIZE*(NUM_VERTS_Y-1)*0.5;
				i++;
			}
		}

		i = 0;
		for (int x=0; x<NUM_VERTS_X-1; x++) {
			for (int y=0;y<NUM_VERTS_Y-1;y++) {
				// face 1
				// position
				faces[i][0][0] = y*NUM_VERTS_X+x;
				faces[i][0][1] = y*NUM_VERTS_X+x+1;
				faces[i][0][2] = (y+1)*NUM_VERTS_X+x+1;
				// normale
				Vector3 v11 = Vector3(&positions[faces[i][0][0]][0])-Vector3(&positions[faces[i][0][1]][0]);
				Vector3 v12 = Vector3(&positions[faces[i][0][0]][0])-Vector3(&positions[faces[i][0][2]][0]);
				Vector3 n1 = v11.crossProduct(v12);
				n1.normalise();
				normals[i][0] = n1.x;
				normals[i][1] = n1.y;
				normals[i][2] = n1.z;
				faces[i][1][0] = faces[i][1][1] = faces[i][1][2]= i;
				// texture
				faces[i][2][0] = 0;
				faces[i][2][1] = 1;
				faces[i][2][2] = 2;
				i++;
				// face 2
				// position
				faces[i][0][0] = y*NUM_VERTS_X+x;
				faces[i][0][1] = (y+1)*NUM_VERTS_X+x+1;
				faces[i][0][2] = (y+1)*NUM_VERTS_X+x;
				// normale
				Vector3 v21 = Vector3(&positions[faces[i][0][0]][0])-Vector3(&positions[faces[i][0][1]][0]);
				Vector3 v22 = Vector3(&positions[faces[i][0][0]][0])-Vector3(&positions[faces[i][0][2]][0]);
				Vector3 n2 = v21.crossProduct(v22);
				n2.normalise();
				normals[i][0] = n2.x;
				normals[i][1] = n2.y;
				normals[i][2] = n2.z;
				faces[i][1][0] = faces[i][1][1] = faces[i][1][2]= i;
				// texture
				faces[i][2][0] = 0;
				faces[i][2][1] = 1;
				faces[i][2][2] = 2;
				i++;
			}
		}
		MeshManager::getInstance()->add("sol", new Mesh(positions, normals, texCoords, faces, NUM_TRIANGLES));

		Quaternion quat = Quaternion::IDENTITY;
		Objet3D * solGraphique = new Objet3D(matSol, "sol", Transform(Vector3(0.0,-1.0,0.0), quat));
		monde3D->add("sol", solGraphique);
		// ajout du sol physique
		btTriangleIndexVertexArray * m_indexVertexArrays = new btTriangleIndexVertexArray(NUM_TRIANGLES, (int*)&faces[0][0][0], 3*3*sizeof(int), NUM_VERTS,(btScalar*) &positions[0][0], 3*sizeof(f32));
		btBvhTriangleMeshShape * trimeshShape  = new btBvhTriangleMeshShape(m_indexVertexArrays, true, btVector3(-1000,-1000,-1000), btVector3(1000,1000,1000));
		btCollisionShape * groundShape = trimeshShape;
//		btCollisionShape * groundShape = new btBoxShape(btVector3(50.0, 5.0, 50.0)*SCALING);
		btRigidBody * solPhysique = new btRigidBody(createRigidBodyInfo(0.0f, solGraphique->getTransform(), groundShape));
		solPhysique->setFriction(3.0f);
		dynamicsWorld->addRigidBody(solPhysique);

/*		// ajout d'obstacle
		for (int i=0; i<10; i++) {
			float rayon = Random::getf32(0.2, 1.0);
			float hauteur = Random::getf32(0.1, 0.5);
			Quaternion q = Quaternion(Random::getf32(0.0f, M_PI*2.0f), Vector3::UNIT_Y);
			Objet3D * obstacle = new Objet3D(mat, "cone", Transform(Vector3(Random::getf32(-30.0, 30.0),Random::getf32(0.5, 1.0),Random::getf32(-30.0, 30.0)), quat, Vector3(rayon, hauteur, rayon)));
			ostringstream buf;
			buf << "obstacle." << i << '.';
			monde3D->add(buf.str(), obstacle);
			btRigidBody * solPhysique = new btRigidBody(createRigidBodyInfo(100.0, obstacle->getTransform(), new btConeShape(rayon*SCALING, hauteur*SCALING)));
			solPhysique->setFriction(5.0f);
			dynamicsWorld->addRigidBody(solPhysique);
		}
*/
		// chargement des bvh
		SkeletonMesh * skeletonMeshA = new SkeletonMesh("data/walk.bvh", Quaternion(-M_PI_2, Vector3::UNIT_Y), 0.025f, false, false);
		SkeletonMesh * skeletonMeshB = new SkeletonMesh("data/walk2.bvh", Quaternion::IDENTITY, 0.1f, false, false);

		// curseur
		Objet3D * curseurGraphique = new Objet3D(mat, "cube", Transform(Vector3::ZERO, Quaternion::IDENTITY, Vector3(0.25,0.25,0.25)));
		Moteur::getInstance()->getMonde3D()->add("curseur", curseurGraphique);

		dynamicsWorld->stepSimulation(1.0f);

		list<string> nameRagDolls;
		for (int i=0; i<10; i++) {
			ostringstream buf;
			buf << "perso.a." << i << '.';
			nameRagDolls.push_front(buf.str());
			Quaternion q;
			q = Quaternion(Random::getf32(0.0f, M_PI*2.0f), Vector3::UNIT_Y);
			RagDoll * ragDollA = new RagDoll(buf.str(), skeletonMeshA, mat, Transform(Vector3((i%5)*3.0, 1.5, (i/5.0)*3.0), q), dynamicsWorld, monde3D);
			monde3D->add(*nameRagDolls.begin(), ragDollA);
			ragDollA->setBorne(0, 155);
			ragDollA->setNumFrame(Random::getInt(0, 155));
			dynamicsWorld->stepSimulation(0.2f);
		}
/*		RagDoll * ragDollB = new RagDoll("perso.b", skeletonMeshA, mat, Transform(Vector3(0.0, 2.0, 0.0)), dynamicsWorld, monde3D, "tombeEnRestantDebout");
		monde3D->add("perso.b", ragDollB);
		ragDollB->setBorne(0, 155);
		nameRagDolls.push_front("perso.b");
*//*
		// creer un objet de sauvegarde du mouvement
		MotionSave * motionSaveB = new MotionSave(ragDollB, 1.0f/30.0f);
		monde3D->add("motionSave.b", motionSaveB);
*/
		glfwSetKeyCallback(gestionTouche);

		// affichage du monde
		ILuint imageName;
		ilutRenderer(ILUT_OPENGL);
		do {
			curseurGraphique->getTransform().setPosition(monde3D->getCamera()->getCentreObser());
			curseurGraphique->getTransform().setRotation(Quaternion(-monde3D->getCamera()->getRotX()*M_PI/180.0, Vector3::UNIT_Y));

			if (playPhysique) {
				moteur->update();
				dynamicsWorld->stepSimulation(moteur->getElapsed());
			}
			gestionSouris(*camera);
			moteur->display();

/*			ostringstream buf;
			buf << "data/" << glfwGetTime() << ".tga";
			char tampon[80];
			strcpy(tampon, buf.str().c_str());
			ilGenImages(1, &imageName);
			ilBindImage(imageName);
			ilutGLScreen();
			ilSaveImage(tampon);
			ilDeleteImages(1, &imageName);
*/
			moteur->swapBuffer();
		} while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

		// lancer la sauvegarde du mouvement
//		motionSaveB->bvhSave("data/sav.bvh");

		// destruction des sauvegardes
//		delete monde3D->remove("motionSave.b");
		// destruction des ragDolls
		for (list<string>::const_iterator it=nameRagDolls.begin(); it!=nameRagDolls.end(); it++)
			monde3D->remove(*it);
		// du monde physique
		delete solPhysique;
		delete dynamicsWorld;
		// destruction des squelettes
		delete skeletonMeshA;
		delete skeletonMeshB;
		// TODO verifier fuite memoire
	} catch (Erreur e) {
		cout << e.what() << endl;
	} catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}

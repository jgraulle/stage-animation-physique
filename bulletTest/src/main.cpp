/*
 * main.cpp
 *
 *  Created on: 12 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "TransformConv.h"
#include "DebugDraw.h"
#include "RagDoll.h"

#include <iostream>
#include <sstream>
#include <GL/glfw.h>
#include "../../moteurGraphique/src/MoteurGraphique.h"
#include <btBulletDynamicsCommon.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>

using namespace std;

btRigidBody::btRigidBodyConstructionInfo createRigidBodyInfo(btScalar mass, Transform & transform, btCollisionShape* collisionShape) {
	btVector3 inertie;
	collisionShape->calculateLocalInertia(mass, inertie);
	return btRigidBody::btRigidBodyConstructionInfo(mass, (btMotionState*)new TransformConv(&transform), collisionShape, inertie);
}

void ajouterPerso(const string & name, Vector3 positionPerso, Material & mat, Monde3D * monde3D, btDynamicsWorld * dynamicsWorld) {
	// ajout de la colonne
	Objet3D * colonneGraphique = new Objet3D(mat, "cube", Transform(positionPerso, Quaternion(), Vector3(0.1, 0.7, 0.1)));
	monde3D->add(name+"colonne", colonneGraphique);
	btRigidBody * colonnePhysique = new btRigidBody(createRigidBodyInfo(0.0, colonneGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.35, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(colonnePhysique);
	colonnePhysique->setActivationState(DISABLE_DEACTIVATION);

//	btPoint2PointConstraint * contrainteColonne = new btPoint2PointConstraint(*colonnePhysique, btVector3(0.0, 0.35, 0.0)*SCALING);
//	dynamicsWorld->addConstraint(contrainteColonne, true);

	// ajout du bras droit
	Objet3D * brasDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.5,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"brasDroit", brasDroiteGraphique);
	btRigidBody * brasDroitePhysique = new btRigidBody(createRigidBodyInfo(0.8, brasDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(brasDroitePhysique);
	brasDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * epauleDroiteContrainte = new btConeTwistConstraint(*colonnePhysique, *brasDroitePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.25, 0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.2, 0.0, 0.0)*SCALING));
	epauleDroiteContrainte->setLimit(M_PI_2*0.9, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(epauleDroiteContrainte, false);

	// ajout de l'avant bras droit
	Objet3D * avantBrasDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.5+0.4,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"avantBrasDroit", avantBrasDroiteGraphique);
	btRigidBody * avantBrasDroitePhysique = new btRigidBody(createRigidBodyInfo(0.2, avantBrasDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(avantBrasDroitePhysique);
	avantBrasDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * coudeDroiteContrainte = new btHingeConstraint(*brasDroitePhysique, *avantBrasDroitePhysique, btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(0.2, 0.0, 0.0)*SCALING), btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(-0.2, 0.0, 0.0)*SCALING));
	coudeDroiteContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le coude au 3/4 en avant mais pas du tout en arriere
	dynamicsWorld->addConstraint(coudeDroiteContrainte, true);

	// ajout du bras gauche
	Objet3D * brasGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.5,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"brasGauche", brasGaucheGraphique);
	btRigidBody * brasGauchePhysique = new btRigidBody(createRigidBodyInfo(0.8, brasGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(brasGauchePhysique);
	brasGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * epauleGaucheContrainte = new btConeTwistConstraint(*colonnePhysique, *brasGauchePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.25, 0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.2, 0.0, 0.0)*SCALING));
	epauleGaucheContrainte->setLimit(M_PI_2*0.9, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(epauleGaucheContrainte, false);

	// ajout de l'avant bras gauche
	Objet3D * avantBrasGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.5-0.4,0.35,0.0), Quaternion(), Vector3(0.4, 0.1, 0.1)));
	monde3D->add(name+"avantGaucheDroit", avantBrasGaucheGraphique);
	btRigidBody * avantBrasGauchePhysique = new btRigidBody(createRigidBodyInfo(0.2, avantBrasGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.2, 0.05, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(avantBrasGauchePhysique);
	avantBrasGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * coudeGaucheContrainte = new btHingeConstraint(*brasGauchePhysique, *avantBrasGauchePhysique, btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(-0.2, 0.0, 0.0)*SCALING), btTransform(btQuaternion(0.0, M_PI_2, 0.0), btVector3(0.2, 0.0, 0.0)*SCALING));
	coudeGaucheContrainte->setLimit(-M_PI_2-M_PI_4, 0.0);	// on peut plier le coude au 3/4 en avant mais pas du tout en arriere
	dynamicsWorld->addConstraint(coudeGaucheContrainte, true);

	// ajout de la cuisse droit
	Objet3D * cuisseDroiteGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.25,-0.7,0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"cuisseDroite", cuisseDroiteGraphique);
	btRigidBody * cuisseDroitePhysique = new btRigidBody(createRigidBodyInfo(0.8, cuisseDroiteGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(cuisseDroitePhysique);
	cuisseDroitePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * bassinDroitContrainte = new btConeTwistConstraint(*colonnePhysique, *cuisseDroitePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.25, -0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.0, 0.25, 0.0)*SCALING));
	bassinDroitContrainte->setLimit(M_PI_4, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(bassinDroitContrainte, true);

	// ajout du mollet droit
	Objet3D * molletDroitGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(0.25, -0.7-0.5, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"molletDroit", molletDroitGraphique);
	btRigidBody * molletDroitPhysique = new btRigidBody(createRigidBodyInfo(0.2, molletDroitGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(molletDroitPhysique);
	molletDroitPhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * genouxDroitContrainte = new btHingeConstraint(*cuisseDroitePhysique, *molletDroitPhysique, btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, -0.25, 0.0)*SCALING), btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, 0.25, 0.0)*SCALING));
	genouxDroitContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le genoux au 3/4 en arriere mais pas du tout en avant
	dynamicsWorld->addConstraint(genouxDroitContrainte, true);

	// ajout de la cuisse gauche
	Objet3D * cuisseGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.25, -0.7, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"cuisseGauche", cuisseGaucheGraphique);
	btRigidBody * cuisseGauchePhysique = new btRigidBody(createRigidBodyInfo(0.8, cuisseGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(cuisseGauchePhysique);
	cuisseGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btConeTwistConstraint * bassinGaucheContrainte = new btConeTwistConstraint(*colonnePhysique, *cuisseGauchePhysique, btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(-0.25, -0.35, 0.0)*SCALING), btTransform(btQuaternion(0.0, 0.0, M_PI_4), btVector3(0.0, 0.25, 0.0)*SCALING));
	bassinGaucheContrainte->setLimit(M_PI_4, M_PI_4, M_PI_4);
	dynamicsWorld->addConstraint(bassinGaucheContrainte, true);

	// ajout du mollet gauche
	Objet3D * molletGaucheGraphique = new Objet3D(mat, "cube", Transform(positionPerso+Vector3(-0.25, -0.7-0.5, 0.0), Quaternion(), Vector3(0.1, 0.5, 0.1)));
	monde3D->add(name+"molletGauche", molletGaucheGraphique);
	btRigidBody * molletGauchePhysique = new btRigidBody(createRigidBodyInfo(0.2, molletGaucheGraphique->getTransform(), new btBoxShape(btVector3(0.05, 0.25, 0.05)*SCALING)));
	dynamicsWorld->addRigidBody(molletGauchePhysique);
	molletGauchePhysique->setActivationState(DISABLE_DEACTIVATION);

	btHingeConstraint * genouxGaucheContrainte = new btHingeConstraint(*cuisseGauchePhysique, *molletGauchePhysique, btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, -0.25, 0.0)*SCALING), btTransform(btQuaternion(M_PI_2, 0.0, 0.0), btVector3(0.0, 0.25, 0.0)*SCALING));
	genouxGaucheContrainte->setLimit(0.0, M_PI_2+M_PI_4);	// on peut plier le genoux au 3/4 en arriere mais pas du tout en avant
	dynamicsWorld->addConstraint(genouxGaucheContrainte, true);

	molletGauchePhysique->setAngularVelocity(btVector3(2.5, 0.0, 2.5));
}

// gestion du drag and drop de la souris
void gestionSouris(Camera & camera) {
	// coordonn�e de la souris courrante
	int x, y;
	glfwGetMousePos(&x, &y);
	int cran = glfwGetMouseWheel();

	// variables conservant l'ancienne position de la souris
	static int sourisX=x, sourisY=y, cranAnc = cran;

	// si la souris a boug�
	if (sourisX!=x || sourisY!=y) {
		// si le bouton gauche est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS) {
			camera.addRot((double)(x-sourisX), (double)(y-sourisY));
		}
		// si le bouton droit est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_RIGHT)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3((double)(x-sourisX)/10.0, 0.0, (double)(y-sourisY)/10.0));
		}
		// si le bouton du milieu est press�
		if (glfwGetMouseButton(GLFW_MOUSE_BUTTON_MIDDLE)==GLFW_PRESS) {
			camera.moveCentreObser(Vector3(0.0, (double)(sourisY-y)/10.0, 0.0));
		}
	}
	// si la molette de la souris a tourn�
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
		camera->setDistance(20);
		camera->setRot(0.0, 45.0);

		// ajout des lumieres
		monde3D->add("lumiere0", new Lumiere(Vector3(10.0,10.0,10.0), Couleur::BLANC));
		monde3D->add("lumiere1", new Lumiere(Vector3(-10.0,10.0,-10.0), Couleur::BLANC));

		// chargement des ressources
		const Image * im = ImagesManager::getInstance()->add(new Image("data/cube.png"));
		TexturesManager::getInstance()->add(new Texture("data/cube.png", true));
		Material mat("data/cube.png");
		MeshManager::getInstance()->add(new Cube("cube"));
//		MeshManager::getInstance()->add(new Sphere("sphere", 16, 8));
		MeshManager::getInstance()->add(new Cylinder("cylindre", 16));
//		MeshManager::getInstance()->add(new Capsule("capsule", 2.0, 8.0, 16, 8));
/*		f32 positions[][3] = {{0.0f, 0.0f, 0.0f}, {1.0f, 0.0f, 0.0f}, {0.0f, 1.0f, 0.0f}};
		f32 normals[][3] = {{0.0f, 0.0f, 1.0f}};
		f32 texCoords[][2] = {{0.0f, 0.0f}, {1.0f, 0.0f}, {0.0f, 1.0f}};
		u16 faces[][3][3] = {{{0,1,2},{0,0,0},{0,1,2}}};
		MeshManager::getInstance()->add(new Mesh("testMesh", positions, normals, texCoords, faces, 1));
		monde3D->add("testMesh", new Objet3D(mat, "testMesh", Transform(Vector3(4.0,0.0,0.0))));
*/
		// creation du monde physique
		btDefaultCollisionConfiguration * collisionConfiguration = new btDefaultCollisionConfiguration();
		btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
		btAxisSweep3 * overlappingPairCache = new btAxisSweep3(btVector3(-10000,-10000,-10000), btVector3(10000,10000,10000), 1024);
		btSequentialImpulseConstraintSolver * solver = new btSequentialImpulseConstraintSolver;
		btDiscreteDynamicsWorld * dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
		DebugDraw * debugDrawer = new DebugDraw();
		dynamicsWorld->setDebugDrawer(debugDrawer);

		// ajout du sol graphique
		Quaternion quat = Quaternion::IDENTITY;
//		quat.FromAngleAxis(M_PI/2.0, Vector3::UNIT_X);
		Objet3D * solGraphique = new Objet3D(mat, "cube", Transform(Vector3(0.0,-5.0,0.0), quat, Vector3(50.0, 5.0, 50.0)));
		monde3D->add("sol", solGraphique);
		// ajout du sol physique
		btRigidBody * solPhysique = new btRigidBody(createRigidBodyInfo(0.0, solGraphique->getTransform(), new btBoxShape(btVector3(50.0, 5.0, 50.0)*SCALING)));
		dynamicsWorld->addRigidBody(solPhysique);

/*		// debug forme
		Objet3D * cylindreGraphique = new Objet3D(mat, "cylindre", Transform(Vector3(4.0,5.0,0.0), Quaternion(), Vector3(2.0, 2.0, 4.0)));
		monde3D->add("testCapsule", cylindreGraphique);
		btRigidBody * cylindrePhysique = new btRigidBody(createRigidBodyInfo(2.0, cylindreGraphique->getTransform(), new btCapsuleShapeZ(2.0*SCALING, 8.0*SCALING)));
		cylindrePhysique->setAngularVelocity(btVector3(2.0, 0.0, 2.0));
		dynamicsWorld->addRigidBody(cylindrePhysique);
*/

/*		// ajout de dominos
		for (float x=-30.0; x<10.0; x+=1.4) {
			char tampon[80];
			sprintf(tampon, "domino%2f%", x);
			// ajout d'un cube graphique
			Objet3D * cubeGraphique = new Objet3D(mat, "cube", Transform(Vector3(x,1.0,0), Quaternion(), Vector3(0.1, 1.0, 0.4)));
			monde3D->add(tampon, cubeGraphique);
			// ajout d'un cube physique
			btRigidBody * cubePhysique = new btRigidBody(createRigidBodyInfo(0.01, cubeGraphique->getTransform(), new btBoxShape(btVector3(0.1, 1.0, 0.4)*SCALING)));
			dynamicsWorld->addRigidBody(cubePhysique);
		}
		// ajout d'une sphere qui declanche les dominos
		// ajout d'une sphere graphique
		Objet3D * sphere = new Objet3D(mat, "sphere", Transform(Vector3(12.0, 1.0, 0.0), Quaternion(), Vector3(1.0, 1.0, 1.0)));
		monde3D->add("sphere", sphere);
		// ajout d'un sphere physique
		btRigidBody * spherePhysique = new btRigidBody(createRigidBodyInfo(0.02, sphere->getTransform(), new btSphereShape(1.0*SCALING)));
		spherePhysique->setLinearVelocity(btVector3(-5.0, 0.0, 0.0));
		dynamicsWorld->addRigidBody(spherePhysique);
*/
/*
		for (float y=-5.0; y<5.0; y+=2.0) {
			for (float x=-5.0; x<5.0; x+=2.0) {
				stringstream tampon;
				tampon << "RagDoll(" << x << "," << y << ")-";
				new RagDoll(tampon.str(), monde3D, dynamicsWorld, mat, btVector3(x, Random::getf32(2.90, 3.10), y));
				//ajouterPerso(tampon.str(), Vector3(x, Random::getf32(2.90, 3.10), y), mat, monde3D, dynamicsWorld);
			}
		}
*/

		RagDoll * toto = new RagDoll("coucou2", monde3D, dynamicsWorld, mat, btVector3(0.0, 3.0, 3.0));
		ajouterPerso("cocuou", Vector3(0.0, 3.0, 0.0), mat, monde3D, dynamicsWorld);

/*		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		Disable texture(GL_TEXTURE_2D);
		Disable lumiere(GL_LIGHTING);
*/
		// affichage du monde
		do {
			moteur->update();
			dynamicsWorld->stepSimulation(moteur->getElapsed()*1.0);
			gestionSouris(*camera);
			moteur->display();
			glLoadIdentity();
			moteur->getMonde3D()->getCamera()->positionner();
//			dynamicsWorld->debugDrawWorld();
			moteur->swapBuffer();
			toto->applyForce();
		} while (!glfwGetKey(GLFW_KEY_ESC) && glfwGetWindowParam(GLFW_OPENED));

		delete dynamicsWorld;

	} catch (Erreur e) {
		cout << e.what() << endl;
	} catch (exception e) {
		cout << e.what() << endl;
	}
	return 0;
}

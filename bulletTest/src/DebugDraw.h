/*
 * DebugDraw.h
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef DEBUGDRAW_H_
#define DEBUGDRAW_H_

#include <btBulletDynamicsCommon.h>
#include <MoteurGraphique.h>

class DebugDraw : public Objet3D, public btIDebugDraw {
public:
	DebugDraw(btDiscreteDynamicsWorld * dynamicsWorld);

	virtual ~DebugDraw();

	// Objet3D : affichage
	virtual void display() const;

	// btIDebugDraw
	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);
	virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);
	virtual void reportErrorWarning(const char * warningString);
	virtual void draw3dText(const btVector3& location,const char* textString);
	virtual void setDebugMode(int debugMode);
	virtual void addDebugMode(int debugMode);
	virtual void remDebugMode(int debugMode);
	virtual void modDebugMode(int debugMode);
	virtual int	getDebugMode() const;

private:
	int debugMode;
	btDiscreteDynamicsWorld * dynamicsWorld;
};

#endif /* DEBUGDRAW_H_ */

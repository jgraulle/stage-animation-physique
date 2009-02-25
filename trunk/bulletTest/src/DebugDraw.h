/*
 * DebugDraw.h
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef DEBUGDRAW_H_
#define DEBUGDRAW_H_

#include <btBulletDynamicsCommon.h>

class DebugDraw : public btIDebugDraw {
public:
	DebugDraw();

	virtual ~DebugDraw();

	virtual void drawLine(const btVector3& from,const btVector3& to,const btVector3& color);

	virtual void drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color);

	virtual void reportErrorWarning(const char * warningString);

	virtual void draw3dText(const btVector3& location,const char* textString);

	virtual void setDebugMode(int debugMode);

	virtual int	getDebugMode() const;
};

#endif /* DEBUGDRAW_H_ */

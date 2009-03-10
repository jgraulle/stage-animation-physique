/*
 * DebugDraw.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "DebugDraw.h"
#include "TransformConv.h"

#include "../../moteurGraphique/src/MoteurGraphique.h"

DebugDraw::DebugDraw()
//: debugMode(DBG_DrawWireframe) {}
: debugMode(DBG_DrawConstraints | DBG_DrawConstraintLimits) {}

DebugDraw::~DebugDraw() {}

void DebugDraw::drawLine(const btVector3& from,const btVector3& to,const btVector3& color) {
	Disable lumiere(GL_LIGHTING);
	Disable texture(GL_TEXTURE_2D);
	Disable profondeur(GL_DEPTH_TEST);
	glBegin(GL_LINES);
	glColor4f(color.getX(), color.getY(), color.getZ(),1.f);
	glVertex3d(from.getX()/SCALING, from.getY()/SCALING, from.getZ()/SCALING);
	glVertex3d(to.getX()/SCALING, to.getY()/SCALING, to.getZ()/SCALING);
	glEnd();
}

void DebugDraw::drawContactPoint(const btVector3& PointOnB,const btVector3& normalOnB,btScalar distance,int lifeTime,const btVector3& color) {}

void DebugDraw::reportErrorWarning(const char * warningString) {
	cout << warningString;
}

void DebugDraw::draw3dText(const btVector3& location,const char* textString) {
	cout << textString;
}

void DebugDraw::setDebugMode(int m) {
	cout << "DebugDraw::setDebugMode" << m <<  endl;
	debugMode = m;
}

void DebugDraw::addDebugMode(int m) {
	cout << "DebugDraw::addDebugMode" << m <<  endl;
	debugMode |= m;
}

void DebugDraw::remDebugMode(int m) {
	cout << "DebugDraw::remDebugMode" << m <<  endl;
	debugMode &= ~m;
}

void DebugDraw::modDebugMode(int m) {
	cout << "DebugDraw::modDebugMode" << m <<  endl;
	if (debugMode & m)
		remDebugMode(m);
	else
		addDebugMode(m);
}

int	DebugDraw::getDebugMode() const {
	return debugMode;
	//return DBG_DrawWireframe | DBG_DrawConstraints;
	//return DBG_DrawConstraints | DBG_DrawConstraintLimits;
}

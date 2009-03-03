/*
 * TransformConv.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "TransformConv.h"

Vector3 TransformConv::btToGraph (const btVector3 & v) {
	return Vector3(v.getX()/SCALING, v.getY()/SCALING, v.getZ()/SCALING);
}

Quaternion TransformConv::btToGraph (const btQuaternion & q) {
	return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

btVector3 TransformConv::graphToBt (const Vector3 & v) {
	return btVector3(v.x*SCALING, v.y*SCALING, v.z*SCALING);
}

btQuaternion TransformConv::graphToBt (const Quaternion & q) {
	return btQuaternion(q.x, q.y, q.z, q.w);
}

TransformConv::TransformConv(Transform * tranform) : tranform(tranform) {}

TransformConv::~TransformConv() {}

void TransformConv::getWorldTransform(btTransform & worldTrans) const {
	worldTrans.setOrigin(graphToBt(tranform->getPosition()));
	worldTrans.setRotation(graphToBt(tranform->getOrientation()));
}

void TransformConv::setWorldTransform(const btTransform & worldTrans) {
	tranform->setPosition(btToGraph(worldTrans.getOrigin()));
	tranform->setRotation(btToGraph(worldTrans.getRotation()));
}

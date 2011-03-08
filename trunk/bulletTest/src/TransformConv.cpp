/*
 * TransformConv.cpp
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "TransformConv.h"

Vector3 TransformConv::btToGraph(const btVector3 & v) {
	return Vector3(v.getX()/SCALING, v.getY()/SCALING, v.getZ()/SCALING);
}

Quaternion TransformConv::btToGraph(const btQuaternion & q) {
	return Quaternion(q.getW(), q.getX(), q.getY(), q.getZ());
}

Transform TransformConv::btToGraph(const btTransform & t) {
	return Transform(btToGraph(t.getOrigin()), btToGraph(t.getRotation()));
}

btVector3 TransformConv::graphToBt(const Vector3 & v) {
	return btVector3(v.x*SCALING, v.y*SCALING, v.z*SCALING);
}

btQuaternion TransformConv::graphToBt(const Quaternion & q) {
	return btQuaternion(q.x, q.y, q.z, q.w);
}

btTransform TransformConv::graphToBt(const Transform & t) {
	return btTransform(graphToBt(t.getOrientation()), graphToBt(t.getPosition()));
}

TransformConv::TransformConv(Transform * tranform) : tranform(tranform) {
	btTrans.setOrigin(graphToBt(tranform->getPosition()));
	btTrans.setRotation(graphToBt(tranform->getOrientation()));
}

TransformConv::~TransformConv() {}

void TransformConv::getWorldTransform(btTransform & worldTrans) const {
	worldTrans = btTrans;
}

void TransformConv::setWorldTransform(const btTransform & worldTrans) {
	tranform->setPosition(btToGraph(worldTrans.getOrigin()));
	tranform->setRotation(btToGraph(worldTrans.getRotation()));
	btTrans = worldTrans;
}

/*
 * TransformConv.h
 *
 *  Created on: 25 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef TRANSFORMCONV_H_
#define TRANSFORMCONV_H_

#include <btBulletDynamicsCommon.h>
#include "../../moteurGraphique/src/MoteurGraphique.h"

#define SCALING 1.0

class TransformConv : public btMotionState {
public:
	static Vector3 btToGraph (const btVector3 &);
	static Quaternion btToGraph (const btQuaternion &);
	static btVector3 graphToBt (const Vector3 &);
	static btQuaternion graphToBt (const Quaternion &);

	TransformConv(Transform * tranform);
	virtual ~TransformConv();

	virtual void getWorldTransform(btTransform & worldTrans) const;
	virtual void setWorldTransform(const btTransform & worldTrans);

private:
	Transform * tranform;
};

#endif /* TRANSFORMCONV_H_ */

#ifndef _BVH_H
#define _BVH_H

#ifdef __cplusplus
extern "C" {
#endif

#include "motion.h"

int motion_load_bvh(MOTION **pmotion, const char *filename);

#ifdef __cplusplus
}
#endif

#endif


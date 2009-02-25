#ifndef _MOTION_H
#define _MOTION_H

#include "vec.h"

typedef struct
{
	char *name;
	int child;
	int next;
	
	VEC3 offset;

	float channels[6];
	int bindings[6];
	int channels_n;
} JOINT;

#define JOINT_XPOS (1<<0)
#define JOINT_YPOS (1<<1)
#define JOINT_ZPOS (1<<2)

#define JOINT_XROT (1<<3)
#define JOINT_YROT (1<<4)
#define JOINT_ZROT (1<<5)

typedef struct
{
	char *filename;
	
	JOINT *joints;
	int joints_size;
	int joints_n;
	int root_id;
	
	float frame_time;
	float **frames;
	int frames_n;
	int channels_n;
} MOTION;


typedef struct
{
	int id;
	float time;
	float motion_time;
	
	JOINT *joints;
	int joints_n;
	int root_id;
} MFRAME;


extern MOTION *motion_new(char *filename);
extern void motion_free(MOTION *motion, int alloc);
extern int motion_add(MOTION *motion);

extern JOINT *joint_init(JOINT *joint, char *name);

extern void motion_print(MOTION *motion);
extern float motion_get_time(MOTION *motion);

extern MFRAME *motion_frame_init(MFRAME *frame, MOTION *motion);
extern int motion_get_frame(MFRAME *frame, MOTION *motion, int f);
extern void motion_frame_free(MFRAME *frame, int alloc);

extern int motion_frame_get_id(MFRAME *frame);
extern float motion_frame_get_time(MFRAME *frame);
extern float motion_frame_get_motion_time(MFRAME *frame);

extern int motion_frame_get_root_joint(MFRAME *frame);
extern int motion_frame_get_joints_n(MFRAME *frame);

extern char *joint_get_name(MFRAME *frame, int joint_id);
extern int joint_get_child(MFRAME *frame, int joint_id);
extern int joint_get_next(MFRAME *frame, int joint_id);

extern int joint_get_channel_id(MFRAME *frame, int joint_id, int binding);
extern int joint_get_offset(MFRAME *frame, int joint_id, VEC3 t);
extern int joint_get_position(MFRAME *frame, int joint_id, VEC3 t);
extern int joint_get_orientation(MFRAME *frame, int joint_id, VEC3 r, int bindings[3]);

extern int joint_get_hierarchy(MFRAME *frame, int joint_id, int *hierarchy, int hierarchy_size);

#endif


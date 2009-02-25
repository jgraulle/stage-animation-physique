/*
	motion.c
	
	mailto:jciehl@bat710.univ-lyon1.fr
	avril 2006
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>

#include "array.h"
#include "motion.h"

MOTION *motion_new(char *filename)
{
	MOTION *motion;
	
	motion= (MOTION *) malloc(sizeof(MOTION));
	assert(motion!=NULL);
	
	motion->filename= strdup(filename);
	motion->root_id= -1;
	motion->joints= NULL;
	motion->joints_size= 0;
	motion->joints_n= 0;
	
	motion->frame_time= 0.f;
	motion->frames= NULL;
	motion->frames_n= 0;
	motion->channels_n= 0;
	
	return motion;
}

int motion_add(MOTION *motion)
{
	array_add((void **) &motion->joints, &motion->joints_size, 
		motion->joints_n, 
		sizeof(JOINT), 10);
	
	return motion->joints_n++;
}


void motion_free(MOTION *motion, int alloc)
{
	int i;
	
	if(motion==NULL)
		return;
	
	if(motion->filename!=NULL)
		free(motion->filename);
	
	if(motion->joints!=NULL)
	{
		for(i= 0; i < motion->joints_n; i++)
			if(motion->joints[i].name!=NULL)
				free(motion->joints[i].name);
		
		free(motion->joints);
	}		
	
	if(motion->frames!=NULL)
	{
		for(i= 0; i < motion->frames_n; i++)
			if(motion->frames[i]!=NULL)
				free(motion->frames[i]);
			
		free(motion->frames);
	}
	
	if(alloc)
		free(motion);
	
}


JOINT *joint_init(JOINT *joint, char *name)
{
	assert(joint!=NULL);
	
	if(name!=NULL)
		joint->name= strdup(name);
	else
		joint->name= NULL;
	joint->child= -1;
	joint->next= -1;

	joint->channels_n= 0;
	vec3_zero(joint->offset);
	
	return joint;
}

static void node_print(MOTION *motion, int level, int node_id)
{
	int i;
	
	while(node_id!=-1)
	{
		for(i= 0; i < level; i++)
			printf("    ");
		
		if(motion->joints[node_id].name!=NULL)
			printf("joint '%s'  ", motion->joints[node_id].name);
		else
			printf("joint 'noname' ( level %d:%d )  ", level, node_id);
	
	#if 0
		printf("( offset  %f %f %f )  ", 
			motion->joints[node_id].offset[0],
			motion->joints[node_id].offset[1],
			motion->joints[node_id].offset[2]);
		
		printf("( channels %d : ", motion->joints[node_id].channels_n);
		for(i= 0; i < motion->joints[node_id].channels_n; i++)
		{
			switch(motion->joints[node_id].bindings[i])
			{
				case JOINT_XPOS:
					printf("Xposition ");
					break;
				case JOINT_YPOS:
					printf("Yposition ");
					break;
				case JOINT_ZPOS:
					printf("Zposition ");
					break;
					
				case JOINT_XROT:
					printf("Xrotation ");
					break;
				case JOINT_YROT:
					printf("Yrotation ");
					break;
				case JOINT_ZROT:
					printf("Zrotation ");
					break;
			}
		}
		
		printf(")");
	#endif
		
		printf("\n");
		
		node_print(motion, level +1, motion->joints[node_id].child);
		node_id= motion->joints[node_id].next;
	}
}


void motion_print(MOTION *motion)
{
	printf("motion : '%s'\n", motion->filename);
	
	if(motion->root_id==-1)
	{
		printf("    no root joint\n");
		return;
	}
	
	node_print(motion, 1, motion->root_id);
	printf("%d channels\n", motion->channels_n);
	printf("%d frames (time %f)\n", motion->frames_n, motion->frame_time);
}

float motion_get_time(MOTION *motion)
{
	return (float) motion->frames_n * motion->frame_time;
}


int motion_get_frame(MFRAME *frame, MOTION *motion, int f)
{
	int i, j, k;

	if(frame==NULL)
		return -1;
	
	if(f < 0 || f >= motion->frames_n)
		return -1;
	
	frame->id= f;
	frame->time= (float) f * motion->frame_time;
	frame->motion_time= (float) motion->frames_n * motion->frame_time;
	
	k= 0;
	for(i= 0; i < motion->joints_n; i++)
	{
		for(j= 0; j < motion->joints[i].channels_n; j++, k++)
			frame->joints[i].channels[j]= motion->frames[f][k];
	}
	
	return 0;
}

MFRAME *motion_frame_init(MFRAME *frame, MOTION *motion)
{
	int i, j;
	
	if(frame==NULL)
	{
		frame= (MFRAME *) malloc(sizeof(MFRAME));
		assert(frame!=NULL);
	}
	
	frame->joints_n= motion->joints_n;
	frame->root_id= motion->root_id;
	frame->joints= (JOINT *) malloc(sizeof(JOINT) * motion->joints_n);
	assert(frame->joints!=NULL);
	
	for(i= 0; i < motion->joints_n; i++)
	{
		if(motion->joints[i].name!=NULL)
			frame->joints[i].name= strdup(motion->joints[i].name);
		else
			frame->joints[i].name= NULL;
		frame->joints[i].child= motion->joints[i].child;
		frame->joints[i].next= motion->joints[i].next;
		
		vec3_copy(frame->joints[i].offset, motion->joints[i].offset);
		
		frame->joints[i].channels_n= motion->joints[i].channels_n;
		for(j= 0; j < motion->joints[i].channels_n; j++)
		{
			frame->joints[i].bindings[j]= motion->joints[i].bindings[j];
			frame->joints[i].channels[j]=0.f;
		}
	}
	
	return frame;
}

void motion_frame_free(MFRAME *frame, int alloc)
{
	int i;
	
	if(frame==NULL)
		return;

	if(frame->joints!=NULL)
	{
		for(i= 0; i < frame->joints_n; i++)
			if(frame->joints[i].name!=NULL)
				free(frame->joints[i].name);
		
		free(frame->joints);
	}		
	
	if(alloc)
		free(frame);
}


int motion_frame_get_id(MFRAME *frame)
{
	if(frame==NULL)
		return -1;
	
	return frame->id;
}

float motion_frame_get_time(MFRAME *frame)
{
	if(frame==NULL)
		return -1;
	
	return frame->time;
}

float motion_frame_get_motion_time(MFRAME *frame)
{
	if(frame==NULL)
		return -1;
	
	return frame->motion_time;
}


int motion_frame_get_joints_n(MFRAME *frame)
{
	if(frame==NULL)
		return -1;
	
        return frame->joints_n;
}

int motion_frame_get_root_joint(MFRAME *frame)
{
	if(frame==NULL)
		return -1;
	
        return frame->root_id;
}


char *joint_get_name(MFRAME *frame, int joint_id)
{
	if(frame==NULL)
		return "(null)";
        
        if(frame->joints[joint_id].name==NULL)
            return "(noname)";
        else
            return frame->joints[joint_id].name;
}

int joint_get_child(MFRAME *frame, int joint_id)
{
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

        return frame->joints[joint_id].child;
}

int joint_get_next(MFRAME *frame, int joint_id)
{
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;
        
        return frame->joints[joint_id].next;
}

int joint_get_channel_id(MFRAME *frame, int joint_id, int binding)
{
	int i;
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

	for(i= 0; i < frame->joints[joint_id].channels_n; i++)
		if(frame->joints[joint_id].bindings[i]==binding)
			return i;
		
	return -1;
}

int joint_get_offset(MFRAME *frame, int joint_id, VEC3 t)
{
	vec3_zero(t);
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

	vec3_copy(t, frame->joints[joint_id].offset);
	
	return 0;	
}

int joint_get_position(MFRAME *frame, int joint_id, VEC3 t)
{
        int i;
	int code;
	
	vec3_zero(t);
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

        code= 0;
	for(i= 0; i < frame->joints[joint_id].channels_n; i++)
	{
		if(frame->joints[joint_id].bindings[i] & JOINT_XPOS)
                {
                        t[0]= frame->joints[joint_id].channels[i];
                        code++;
                }
                else if(frame->joints[joint_id].bindings[i] & JOINT_YPOS)
                {
                        t[1]= frame->joints[joint_id].channels[i];
                        code++;
                }
                else if(frame->joints[joint_id].bindings[i] & JOINT_ZPOS)
                {
                        t[2]= frame->joints[joint_id].channels[i];
                        code++;
                }
	}

        if (code==3)
            return 0;
        else
            return -1;
}

int joint_get_orientation(MFRAME *frame, int joint_id, VEC3 r, int bindings[3])
{
	int i, j;
	
	vec3_zero(r);
	bindings[0]= -1;
	bindings[1]= -1;
	bindings[2]= -1;
	
	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

	j= 0;
	for(i= 0; j < 3 && i < frame->joints[joint_id].channels_n; i++)
	{
		if(frame->joints[joint_id].bindings[i] 
		& (JOINT_XROT | JOINT_YROT | JOINT_ZROT))
		{
			r[j]= frame->joints[joint_id].channels[i];
			bindings[j]= frame->joints[joint_id].bindings[i];
			j++;
		}
	}
	
	//
	if(j!=3)
		return -1;
	return 0;	
}


static int get_hierarchy_r(MFRAME *frame, int joint_id, int *hierarchy, int id, int level)
{
        int code;
    
        if(id < 0 || id >= frame->joints_n)
            return -1;

        hierarchy[level]= id;
	if(id==joint_id)
            return level +1;
        
        code= get_hierarchy_r(frame, joint_id, hierarchy, 
            joint_get_child(frame, id), level +1);
        if(code!=-1)
            return code;
        
        code= get_hierarchy_r(frame, joint_id, hierarchy, 
            joint_get_next(frame, id), level);
        if(code!=-1)
            return code;

        return -1;
}

int joint_get_hierarchy(MFRAME *frame, int joint_id, int *hierarchy, int hierarchy_size)
{
        int n;
    
 	if(frame==NULL)
		return -1;
	
	if(joint_id < 0 || joint_id >= frame->joints_n)
		return -1;

        if(hierarchy==NULL)
            return -1;
        if(hierarchy_size < frame->joints_n)
            return -1;
        
        n= get_hierarchy_r(frame, joint_id, hierarchy, frame->root_id, 0);
        return n;
}

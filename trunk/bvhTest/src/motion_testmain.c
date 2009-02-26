/*
	motion_testmain
*/

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "motion.h"
#include "bvh.h"


void joint_print_binding(float v, int binding)
{
	printf("%f ", v);
	if(binding==JOINT_XROT)
		printf("X ");
	else if(binding==JOINT_YROT)
		printf("Y ");
	else if(binding==JOINT_ZROT)
		printf("Z ");
}

void joint_print(MFRAME *frame, int joint_id)
{
	VEC3 t;
	VEC3 r;
	int bindings[3];

	while(joint_id!=-1)
	{
                printf("frame %d joint '%s'  ",
                        motion_frame_get_id(frame),
                        joint_get_name(frame, joint_id));

		joint_get_offset(frame, joint_id, t);
		printf("  offset ( %f %f %f )\n", t[0], t[1], t[2]);
		joint_get_position(frame, joint_id, t);
		printf("    T ( %f %f %f )\n", t[0], t[1], t[2]);

		joint_get_orientation(frame, joint_id, r, bindings);
		printf("    R ( ");
			joint_print_binding(r[0], bindings[0]);
			joint_print_binding(r[1], bindings[1]);
			joint_print_binding(r[2], bindings[2]);
		printf(")");

		printf("\n");

		joint_print(frame, joint_get_child(frame, joint_id));
		joint_id= joint_get_next(frame, joint_id);
	}
}

void frame_print(MFRAME *frame)
{
	joint_print(frame, motion_frame_get_root_joint(frame));
}


int main2(int argc, char **argv)
{
	MOTION *motion;
	MFRAME frame;
	int i, j, joints_n, n;
        int *hierarchy;

	if(argc!=2)
	{
		printf("usage : %s motion.bvh\n", argv[0]);
		return 0;
	}

	if(motion_load_bvh(&motion, argv[1]) < 0)
	{
		printf("failed.\n");
		return 1;
	}

	motion_print(motion);
	printf("time %f\n", motion_get_time(motion));

	/* affiche les donnees de la sequence d'animation */
	motion_frame_init(&frame, motion);

	for(i= 0; motion_get_frame(&frame, motion, i)==0; i++)
	{
		printf("frame %d : time %f / %f\n",
			motion_frame_get_id(&frame),
			motion_frame_get_time(&frame),
			motion_frame_get_motion_time(&frame));

		frame_print(&frame);
	}

	printf("stop\n");

        /* affiche la hierarchie de chaque articulation de l'animation */
        motion_get_frame(&frame, motion, 0);
        joints_n= motion_frame_get_joints_n(&frame);
        hierarchy= (int *) malloc(sizeof(int) * joints_n);
        for(j= 0; j < joints_n; j++)
        {
                n= joint_get_hierarchy(&frame, j, hierarchy, joints_n);
                if(n!=-1)
                {
                        printf("%s (%d):\n", joint_get_name(&frame, j), j);
                        for(i= 0; i < n; i++)
                        {
                                printf("  stack[%d]= (%d) :  %s\n", i,
                                    hierarchy[i],
                                    joint_get_name(&frame, hierarchy[i]));
                        }
                }
        }

        free(hierarchy);

        //
	motion_frame_free(&frame, 0);
	motion_free(motion, 1);

	return 0;
}

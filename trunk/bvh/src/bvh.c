/*
	motion capture bvh

	mailto:jciehl@bat710.univ-lyon1.fr
	avril 2006

	base sur
	http://www.dcs.shef.ac.uk/%7Emikem/research/pubs/Motion%20Capture%20File%20Formats%20Explained.pdf
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ctype.h>
#include <string.h>

#include "array.h"
#include "motion.h"
#include "parser.h"

static void print_token(char *tmp)
{

	printf("token : '%s' ", tmp);
/*
	int i;
	for(i= 0; tmp[i]!=0; i++)
		printf("%x ", tmp[i]);
 */
	printf("\n");
}

static int node_load(MOTION *motion, PARSER *parser)
{
	JOINT *joint;
	int joint_id;
	int node_id;
	int tok;
	int code;
	int i;

	joint_id= motion_add(motion);
	joint= &motion->joints[joint_id];

	tok= parser_get_token(parser);
	if(tok==EOF)
		return -1;

	if(strcmp(parser_get_string(parser), "{")==0)
		joint_init(joint, NULL);

	else
	{
		joint_init(joint, parser_get_string(parser));

		if(parser_scan_token(parser, "{") < 0)
			return -1;
	}

	node_id= -1;
	for(;;)
	{
		tok= parser_get_token(parser);
		if(tok==EOF)
			return -1;

		if(tok=='\n' || tok=='\r')
			continue;

		if(strcasecmp(parser_get_string(parser),
                        "OFFSET")==0)
		{
			joint= &motion->joints[joint_id];

			vec3_zero(joint->offset);
			if(parser_get_token(parser)!=0)
				return -1;
			if(sscanf(parser_get_string(parser), "%f",
                                &joint->offset[0])!=1)
				return -1;

			if(parser_get_token(parser)!=0)
				return -1;
			if(sscanf(parser_get_string(parser), "%f",
                                &joint->offset[1])!=1)
				return -1;

			if(parser_get_token(parser)!=0)
				return -1;
			if(sscanf(parser_get_string(parser), "%f",
                                &joint->offset[2])!=1)
				return -1;

			// lire jusqu'a la fin de la ligne
			while(parser_get_token(parser)!='\n')
				print_token(parser_get_string(parser));
		}
		else if(strcasecmp(parser_get_string(parser),
                        "CHANNELS")==0)
		{
			joint= &motion->joints[joint_id];

			if(parser_get_token(parser)!=0)
				return -1;
			if(sscanf(parser_get_string(parser), "%d",
                                &joint->channels_n)!=1)
				return -1;

			for(i= 0; i < joint->channels_n; i++)
				joint->channels[i]= 0.f;

			for(i= 0; i < joint->channels_n; i++)
			{
				if(parser_get_token(parser)!=0)
					return -1;

				if(strcasecmp(parser_get_string(parser),
                                        "Xposition")==0)
					joint->bindings[i]= JOINT_XPOS;
				else if(strcasecmp(parser_get_string(parser),
                                        "Yposition")==0)
					joint->bindings[i]= JOINT_YPOS;
				else if(strcasecmp(parser_get_string(parser),
                                        "Zposition")==0)
					joint->bindings[i]= JOINT_ZPOS;

				else if(strcasecmp(parser_get_string(parser),
                                        "Xrotation")==0)
					joint->bindings[i]= JOINT_XROT;
				else if(strcasecmp(parser_get_string(parser),
                                        "Yrotation")==0)
					joint->bindings[i]= JOINT_YROT;
				else if(strcasecmp(parser_get_string(parser),
                                        "Zrotation")==0)
					joint->bindings[i]= JOINT_ZROT;
				else
				{
					printf("channel : unknown binding '%s'\n",
                                                parser_get_string(parser));
					return -1;
				}
			}

			// lire jusqu'a la fin de la ligne
			while(parser_get_token(parser)!='\n')
				// print_token(tmp);
				{;}
		}
		else if(strcasecmp(parser_get_string(parser),
                        "JOINT")==0
                || strcasecmp(parser_get_string(parser),
                        "End")==0)
		{
			code= node_load(motion, parser);
			if(code < 0)
				return -1;

			if(node_id < 0)
				motion->joints[joint_id].child= code;
			else
				motion->joints[node_id].next= code;

			node_id= code;
			motion->channels_n+= motion->joints[node_id].channels_n;
		}
		else if(strcmp(parser_get_string(parser), "}")==0)
			break;

		else
		{
			printf("NODE (%d) ", tok);
			print_token(parser_get_string(parser));
			return -1;
		}
	}

	return joint_id;
}


static int hierarchy_load(MOTION *motion, PARSER *parser)
{
	int node_id;
	int tok;
	int code;

	// printf("HIERARCHY\n");

	node_id= -1;
	for(;;)
	{
		tok= parser_get_token(parser);
		if(tok==EOF)
			return -1;

		if(tok=='\n' || tok=='\r')
			continue;

		if(strcasecmp(parser_get_string(parser),
                        "ROOT")==0)
		{
			code= node_load(motion, parser);
			if(code < 0)
				return -1;

			if(node_id < 0)
				motion->root_id= code;
			else
				motion->joints[node_id].next= code;

			node_id= code;
			motion->channels_n+= motion->joints[node_id].channels_n;
		}
		else
			break;
	}


	return motion->root_id;
}

static int motion_load(MOTION *motion, PARSER *parser)
{
	int tok;
	int frame, channel;

	// printf("MOTION\n");

	if(parser_casescan_token(parser,
                "Frames") < 0)
		return -1;
	tok= parser_get_token(parser);
	if(tok==EOF || tok=='\n' || tok=='\r')
		return -1;
	if(sscanf(parser_get_string(parser), "%d",
                &motion->frames_n)!=1)
		return -1;
	// lire jusqu'a la fin de la ligne
	while(parser_get_token(parser)!='\n')
		// print_token(tmp);
		{;}

	// printf("frames : %d\n", motion->frames_n);

	if(parser_casescan_token(parser,
                "Frame") < 0)
		return -1;
	if(parser_casescan_token(parser,
                "Time") < 0)
		return -1;
	tok= parser_get_token(parser);
	if(tok==EOF || tok=='\n' || tok=='\r')
		return -1;
	if(sscanf(parser_get_string(parser), "%f",
                &motion->frame_time)!=1)
		return -1;
	// lire jusqu'a la fin de la ligne
	while(parser_get_token(parser)!='\n')
		// print_token(tmp);
		{;}

	// printf("frame time : %f\n", motion->frame_time);

	//
	motion->frames= (float **) malloc(sizeof(float *) * motion->frames_n);
	assert(motion->frames!=NULL);
	for(frame= 0; frame < motion->frames_n; frame++)
		motion->frames[frame]= NULL;

	for(frame= 0; frame < motion->frames_n; frame++)
	{
		motion->frames[frame]= (float *) malloc(sizeof(float) * motion->channels_n);
		assert(motion->frames[frame]!=NULL);

		for(channel= 0; channel < motion->channels_n; channel++)
		{
			tok= parser_get_token(parser);
			if(tok==EOF || tok=='\n' || tok=='\r')
				return -1;
			if(sscanf(parser_get_string(parser), "%f",
                                &motion->frames[frame][channel])!=1)
				return -1;
		}

		// lire jusqu'a la fin de la ligne
		while(parser_get_token(parser)!='\n')
			// print_token(tmp);
			{;}
	}

	return 0;
}


int motion_load_bvh(MOTION **pmotion, const char *filename)
{
	char tmp[1024];
	PARSER parser;

	MOTION *motion;
	int code;

	*pmotion= NULL;
	if(parser_init_open(&parser, filename)==NULL)
		return -1;

	// tokens supplementaires pour les valeurs numeriques
	parser_set_tokens(&parser, "-.");
	// separateurs pour les blocs
	parser_set_separators(&parser, "{}");
	// buffer
        parser_set_string(&parser, tmp, sizeof(tmp));

	motion= motion_new(filename);

	code= 0;
	while(code==0 && parser_get_token(&parser)!=EOF)
	{
		if(strcasecmp(parser_get_string(&parser),
                        "HIERARCHY")==0)
		{
			if(hierarchy_load(motion, &parser) < 0)
				code= -1;
		}

		if(strcasecmp(parser_get_string(&parser),
                        "MOTION")==0)
		{
			if(motion_load(motion, &parser) < 0)
				code= -1;
		}
	}

	*pmotion= motion;
	parser_close(&parser);

	return code;
}


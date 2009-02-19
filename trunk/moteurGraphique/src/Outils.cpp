/*
 * Outils.cpp
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#include "Outils.h"

Enable::Enable(GLenum flag)
: flag(flag) {
	state = glIsEnabled(flag);
	if(!state)
		glEnable(flag);
}
Enable::~Enable() {
	if(!state)
		glDisable(flag);
}

Disable::Disable(GLenum flag)
: flag(flag) {
	state = glIsEnabled(flag);
	if(state)
		glDisable(flag);
}
Disable::~Disable() {
	if(state)
		glEnable(flag);
}

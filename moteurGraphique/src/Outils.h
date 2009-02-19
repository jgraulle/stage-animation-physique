/*
 * Outils.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef OUTILS_H_
#define OUTILS_H_

#include <sstream>

#include <GL/gl.h>

using namespace std;

#define DegToRad(a)							( (a) *  0.01745329252f )
#define RadToDeg(a)							( (a) * 57.29577951f )

// fonction qui transforme n'importe quel type en string si l'operateur << est defini
template<typename T>
std::string tostring( const T & Value ) {
	// utiliser un flux de sortie pour creer la chaine
	std::ostringstream oss;
	// ecrire la valeur dans le flux
	oss << Value;
	// renvoyer une string
	return oss.str();
}

class Enable {
public:
	Enable(GLenum flag);
	~Enable();
private:
	GLenum flag;
	GLboolean state;
};

class Disable {
public:
	Disable(GLenum flag);
	~Disable();
private:
	GLenum flag;
	GLboolean state;
};

#endif /* OUTILS_H_ */

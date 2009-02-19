/*
 * Ressource.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef RESSOURCE_H_
#define RESSOURCE_H_

#include <string>

using namespace std;

template <class T>
class Ressource {
public:
	// constructeur
	Ressource(string name, const T * ressource) : name(name), ressource(ressource) {}

	// destructeur
	virtual ~Ressource() {delete ressource;}

	// acceder au nom de la ressource
	string getName() const {return name;}

	// acceder a la resource
	const T * getRessource() const {return ressource;}

private:
	string name;
	const T * ressource;
};

#endif /* RESSOURCE_H_ */

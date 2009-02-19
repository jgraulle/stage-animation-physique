/*
 * Erreur.h
 *
 *  Created on: 6 fevr. 2009
 *      Author: jeremie GRAULLE
 */

#ifndef ERREUR_H_
#define ERREUR_H_

#include <exception>
#include <string>

using namespace std;

// erreur generique avec message
class Erreur : public exception {
public:

	// constructeur par defaut (avec un message vide)
	Erreur() : msg("") {}

	// constructeur avec un message
	Erreur(string msg) : msg(msg) {}

	// destructeur
	virtual ~Erreur() throw() {}

	// affichage du message
	virtual const char * what() const throw() {return this->msg.c_str();}

	// accesseur
	string getMsg() {return msg;}

	// modifieur
	void setMsg(string msg) {this->msg = msg;}

private:
	// le message
	string msg;
};

class ErreurAllocation : public Erreur
{ public:	ErreurAllocation() : Erreur("erreur allocation dynamique de la memoire") {} };

// fichier non trouve
class ErreurFileNotFound : public Erreur
{ public:	ErreurFileNotFound(string fileName, string description) : Erreur("le fichier "+fileName+" n'a pas ete trouve : "+description) {} };

#endif /*ERREUR_H_*/

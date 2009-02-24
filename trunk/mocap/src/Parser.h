#ifndef _PARSER_H_
#define _PARSER_H_

#include <iostream>
#include <fstream>
#include <stdio.h>
#include <string>

using namespace std;

class Parser {

private:

	// ATTRIBUTS
	string ligneCourante;				// Ligne courante
	bool parseError;					// Flag d'indication d'erreur
	ifstream * fichier;					// Fichier de lecture
	string nomFichier;                  // Sauvegarde du nom du fichier

public:
	// CONSTRUCTEUR
	Parser(const string nomFichier);	// Constructeur
	~Parser();							// Destructeur

	// METHODES
	string getstring();					// Renvoie la derniere ligne lue
	string getstringBetween(char token);
	int expect(const char *word);		// Mange un token
	float getFloat();					// Renvoie un float du fichier
	int getInt();						// Renvoie un int du fichier
	bool isAnError();					// Indique si la dernière opération à provoquée une erreur
	void skipTo(char c,  streamsize max = 256);
	bool nextIsNumber();                // Indique le type du prochain token
	bool nextIsEOL();                   // Indique si le prochain token est un retour ligne
	bool isEOF();
};

#endif

//////////////////////////////////////////////////
//				 CLASSE PARSER
//////////////////////////////////////////////////

#include "Parser.h"


//////////////////////////////////////////////////
// PROC:   Constructeur
// DOES:   ouverture d'un fichier a parser
//////////////////////////////////////////////////
Parser::Parser(const string nomFichier) {
    
	// Ouverture du fichier
	//fp = fopen(nomFichier,"rt");
	fichier = new ifstream(nomFichier.c_str());
	ws(*fichier);
	
	// Sauvegarde du nom du fichier
	this->nomFichier = nomFichier;

	if (!fichier->good()) {
		parseError = true;
		if (nomFichier != "") 
            clog << "Fichier introuvable: \"" << nomFichier << "\"" << endl;
    } else {
		// Pas d'erreur par défaut
		parseError = false;	
		clog << "Ouverture du fichier \"" << nomFichier << "\"" << endl;
    }
}

//////////////////////////////////////////////////
// PROC:   Destructeur
// DOES:   fermeture du fichier
//////////////////////////////////////////////////
Parser::~Parser() {

	// Fermeture du fichier
	if (fichier->good()) {
    	fichier->close();
       clog << "Fermeture fichier " << nomFichier << endl;
    }
}

//////////////////////////////////////////////////
// PROC:   Parser::getstring()
// DOES:   reads in new, non-empty line from Bvh file
//////////////////////////////////////////////////
string Parser::getstring()
{

	if (fichier->eof()) {
		parseError = true;
		return "";
	} else {
		string s;
		*fichier >> s;
		return s;
	}

}

//////////////////////////////////////////////////
// PROC:   Parser::getstringBetween()
//////////////////////////////////////////////////
string Parser::getstringBetween(char token) {
    // On saute les espaces et retour ligne jusqu'au token (y compris)
    char next = fichier->peek();
    while (next == ' ' || next == '\n' || next == token) {
        fichier->get();
        next = fichier->peek();
    }     
    
    string resultat = "";
    
    // Enfin on lis tout jusqu'au prochain token
    next = fichier->peek();
    while (next != token) {
        resultat += fichier->get();
        next = fichier->peek();
    } 
    
    // On n'oublie pas de sauter le dernier token
    fichier->get();
    
    // Resultat

    return resultat;
}

//////////////////////////////////////////////////
// PROC:   Parser::EOF()
// DOES:   indique si la fin du fichier est atteinte
//////////////////////////////////////////////////
bool Parser::isEOF() {
    return fichier->eof();
}

//////////////////////////////////////////////////
// PROC:   Parser::skipTo()
// DOES:   avance dans le flux jusqu'à la rencontre du caractère
//////////////////////////////////////////////////
void Parser::skipTo(char c,  streamsize max) {
    fichier->ignore(max, c);
    
}

//////////////////////////////////////////////////
// PROC:   Parser::nextIsNumber()
// DOES:   indique le type du prochain token
//////////////////////////////////////////////////
bool Parser::nextIsNumber() {
    // On saute les espaces et retour ligne
    char next = fichier->peek();
    while (next == ' ' || next == '\n') {
        fichier->get();
        next = fichier->peek();
    }

    // Teste s'il s'agit d'un nombre ou non
    if (((next >= '0') && (next <= '9')) || (next=='-') || (next=='+')) {
        return true;
    } else {
        return false;
    }
}

//////////////////////////////////////////////////
// PROC:   Parser::nextIsEOL()
// DOES:   indique sir le prochaine token est un retour ligne
//////////////////////////////////////////////////
bool Parser::nextIsEOL() {
    // On saute les espaces et retour ligne
    char next = fichier->peek();
    while (next == ' ') {
        fichier->get();
        next = fichier->peek();
    }  
    
    if (next == '\n') {
        return true;   
    } else {
        return false;   
    }
}

//////////////////////////////////////////////////
// PROC:   Parser::expect()
// DOES:   essaye de lire le paramètre dans le fichier
//////////////////////////////////////////////////
int Parser::expect(const char * word)
{

	string s;
	*fichier >> s;
	if (s == word) return 1;


	clog << "--- Impossible de lire \"" << word << "\"" << endl;
	clog << "la ligne etait: \"" << s << "\"" << endl;
	parseError = true;
	return 0;
}

//////////////////////////////////////////////////
// PROC:   Parser::getFloat()
// DOES:   renvoie un float du fichier
//////////////////////////////////////////////////
float Parser::getFloat()
{
	if (fichier->eof()) {
		parseError = true;
		return 0;
	} else {
		float result;
		*fichier >> result;
		return result;
	}
}

//////////////////////////////////////////////////
// PROC:   Parser::getInt()
// DOES:   renvoie un int du fichier
//////////////////////////////////////////////////
int Parser::getInt()
{
	if (fichier->eof()) {
		parseError = true;
		return 0;
	} else {
		int result;
		*fichier >> result;
		return result;
	}
}

//////////////////////////////////////////////////
// PROC:   Parser::isAnError()
// DOES:   Indique si la dernière opération à provoquée une erreur
//////////////////////////////////////////////////
bool Parser::isAnError() {

	return parseError;
}

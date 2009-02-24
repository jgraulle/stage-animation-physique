
double min(double a, double b) {
	return (a<b?a:b);
}
double max(double a, double b) {
	return (a>b?a:b);
}

float modulo(float valeur, float borneB, float borneA) {
	float result = valeur;
	float intervalle = (borneA-borneB) / 2;
	if (result > borneA) 
		while (result > borneA)
			result-= intervalle;
	if (result <= borneB) 
		while (result <= borneB)
			result+= intervalle;

	return result;

}

int arrondi(float valeur) {
    return (valeur-(int)valeur<.5?(int)valeur:((int)valeur)+1);
}

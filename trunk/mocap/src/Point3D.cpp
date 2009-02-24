#include <stdio.h>
#include <math.h> 
#include "Point3D.h"

Point3D::Point3D(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

Point3D::Point3D(const Point3D &p) {
	this->x = p.x;
	this->y = p.y;
	this->z = p.z;
}


void Point3D::operator=(Point3D p) {
	x = p.x;
	y = p.y;
	z = p.z;
}

void Point3D::setX(float x) {
	this->x = x;
}

void Point3D::setY(float y) {
	this->y = y;
}

void Point3D::setZ(float z) {
	this->z = z;

}

void Point3D::setXYZ(float x, float y, float z) {
	this->x = x;
	this->y = y;
	this->z = z;
}

float Point3D::getX() {
	return x;	
}

float Point3D::getY() {
	return y;	
}

float Point3D::getZ() {
	return z;
}

void Point3D::print() {
	printf("X=%f Y=%f Z=%f\n", x, y, z);
}

// Multiplie un point/vecteur par une matrice 4*4
void Point3D::multVecteurMatrice(float m[16]) {

	float x = getX()*m[0] + getY()*m[4] + getZ()*m[8] + m[12];
	float y = getX()*m[1] + getY()*m[5] + getZ()*m[9] + m[13];
	float z = getX()*m[2] + getY()*m[6] + getZ()*m[10] + m[14];
	float w = getX()*m[3] + getY()*m[7] + getZ()*m[11] + m[15];

	//Point3D * resultat = new Point3D(x/w, y/w, z/w);
	//return Point3D(x/w, y/w, z/w);
	if (w!=0) {
    	this->x = x/w;
    	this->y = y/w;
    	this->z = z/w;
    } else {
        this->x = 0;
        this->y = 0;
        this->z = 0;
    }
    
	/*
	res[0] = v[0]*m[0] + v[1]*m[4] + v[2]*m[8] + v[3]*m[12];
	res[1] = v[0]*m[1] + v[1]*m[5] + v[2]*m[9] + v[3]*m[13];
	res[2] = v[0]*m[2] + v[1]*m[6] + v[2]*m[10] + v[3]*m[14];
	res[3] = v[0]*m[3] + v[1]*m[7] + v[2]*m[11] + v[3]*m[15];
	*/
}

Point3D Point3D::operator*(const Mat4f &m) {
    Point3D resultat;
 	float x = getX()*m[0] + getY()*m[4] + getZ()*m[8] + m[12];
	float y = getX()*m[1] + getY()*m[5] + getZ()*m[9] + m[13];
	float z = getX()*m[2] + getY()*m[6] + getZ()*m[10] + m[14];
	float w = getX()*m[3] + getY()*m[7] + getZ()*m[11] + m[15];

	//Point3D * resultat = new Point3D(x/w, y/w, z/w);
	//return Point3D(x/w, y/w, z/w);
	if (w!=0) {
    	resultat.x = x/w;
    	resultat.y = y/w;
    	resultat.z = z/w;
    } else {
        resultat = Point3D(0,0,0);
    }    
    
    return resultat;  
}

// Distance entre deux points
double Point3D::distanceVers(const Point3D &p2) {

	return sqrt(  (x-p2.x)*(x-p2.x)
				+ (y-p2.y)*(y-p2.y)
				+ (z-p2.z)*(z-p2.z)
  			   );
}

// Produit f32
// Ici les points considérés représentent un vecteur
float Point3D::produitf32(const Point3D &p2) {
	return (  x*p2.x
			+ y*p2.y
			+ z*p2.z
			);

}

// Produit vectoriel
// Ici les points considérés représentent un vecteur
Point3D  Point3D::produitVectoriel(const Point3D &p2) {
	return (Point3D(y*p2.z - z*p2.y,
					-x*p2.z + z*p2.x,
					x*p2.y - y*p2.x
				));
}

// Addition de deux vecteurs
Point3D  Point3D::add(const Point3D &p2) {
	return (Point3D(x+p2.x,
					y+p2.y,
					z+p2.z));
}

// Inversion d'un vecteur
Point3D Point3D::inverse() {

	return (Point3D(-x, -y, -z));
}

// Multiplication d'un vecteur par une constate
Point3D Point3D::mult(float c) {
	return (Point3D(x*c,
					y*c,
					z*c));
}

// Norme d'un vecteur
double Point3D::norme() {

	return distanceVers(Point3D(0,0,0));
}

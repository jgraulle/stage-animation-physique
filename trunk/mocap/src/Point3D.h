#ifndef _POINT3D_H
#define _POINT3D_H

#include "Mat4f.h"


class Point3D
{
 private:
  float x, y, z;							// les composantes du point

 public:
  Point3D(float x=0, float y=0, float z=0);		//constructeur
  Point3D(const Point3D &p);

  void setX(float x);						// définit la composante X
  void setY(float y);						// définit la composante Y
  void setZ(float z);						// définit la composante Z
  void setXYZ(float x, float y, float z);   // definit la composante X et la  compsante Y
  float getX();								// retourne la composante X
  float getY();								// retourne la composante Y
  float getZ();								// retourne la composante Z

  void print();								// Affiche le point
  void multVecteurMatrice(float m[16]);
  double distanceVers(const Point3D &p2);		// Distance entre deux points
  float produitf32(const Point3D &p2);		// Produit f32 entre deux vecteurs
  Point3D produitVectoriel(const Point3D &p2); // Produit vectoriel entre deux vecteurs
  Point3D mult(float c);					// Multiplication de deux vecteurs
  Point3D add(const Point3D &p2);				// Addition de deux vecteurs
  Point3D inverse();						// Inversion d'un vecteur
  double norme();							// Norme d'un vecteur

  void operator=(Point3D p);
  Point3D operator*(const Mat4f &m);
  
};
#endif  //_POINT3D_H

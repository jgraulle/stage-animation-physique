#include "Mat4f.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAILLE 16

Mat4f::Mat4f() {

}

Mat4f::Mat4f(float val[TAILLE]) {
    memcpy(data, val, TAILLE*sizeof(float));
}

Mat4f::Mat4f(const Mat4f &copie) {
    memcpy(data, copie.data, TAILLE*sizeof(float));
}

Mat4f::~Mat4f() {
    
}

float& Mat4f::operator[](int index) {
    return data[index];
}

float Mat4f::operator[](int index) const{
    return data[index];
}

float* Mat4f::operator&(){
    return (float*)&data;
}


Mat4f Mat4f::operator+(Mat4f &mat) {
    Mat4f resultat;
    for (int i=0; i<TAILLE; i++) {
        resultat.data[i] = data[i] + mat.data[i];    
    }
    return resultat;
}

Mat4f Mat4f::operator*(Mat4f &mat) {
    Mat4f resultat;

    for (short k=0; k<4; k++) {
        short  k4 = k<<2;
        resultat[0+k4] = data[0+k4]*mat[0] + data[1+k4]*mat[4] + data[2+k4]*mat[8] + data[3+k4]*mat[12];
        resultat[1+k4] = data[0+k4]*mat[1] + data[1+k4]*mat[5] + data[2+k4]*mat[9] + data[3+k4]*mat[13];
        resultat[2+k4] = data[0+k4]*mat[2] + data[1+k4]*mat[6] + data[2+k4]*mat[10] + data[3+k4]*mat[14];
        resultat[3+k4] = data[0+k4]*mat[3] + data[1+k4]*mat[7] + data[2+k4]*mat[11] + data[3+k4]*mat[15];
    }


    return resultat;
}

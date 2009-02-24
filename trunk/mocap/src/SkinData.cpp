#include "SkinData.h"

SkinData::SkinData(MocapData * referencePosData) {
    //triangleList = new TriangleList();
    this->referencePosData = referencePosData;
    texturesPath = "./";
}

SkinData::SkinData(const SkinData & copie) {
    //triangleList = new TriangleList(*copie.triangleList);
    triangleList = copie.triangleList;
    referencePosData = new MocapData(*copie.referencePosData);
    texturesPath = copie.texturesPath;
}

SkinData::~SkinData() {
    delete referencePosData;

    
    //delete triangleList;
}

/*
SkinData::Vertex *SkinData::getVertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float textU, float textV) {
    return new Vertex(posX, posY, posZ, normX, normY, normZ, textU, textV);

}
*/

void SkinData::addTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3, string textureFileName) {
    triangleList.push_back(Triangle(vertex1, vertex2, vertex3, textureFileName));
}

const SkinData::TriangleList& SkinData::getTriangleList() const {
    return triangleList;
}

MocapData * SkinData::getReferencePosData() {
    return referencePosData;
}

string SkinData::getTexturesPath() {
    return texturesPath;
}

void SkinData::setTexturesPath(string texturesPath) {
    this->texturesPath = texturesPath;
}

// Change la taille de skin
//          valeur = 1.0 => aucun effet
//                 < 1.0 => diminution
//                 > 1.0 => augmentation
void SkinData::resizeSkin(float valeur) {
    
    // Pour chaque triangle  
	for(TriangleList::iterator it = triangleList.begin(); it != triangleList.end(); it++) {
        // Triangle courant
		Triangle& triangle = *it;
		
		triangle.vertex1.posX = triangle.vertex1.getPosX() * valeur;
		triangle.vertex1.posY = triangle.vertex1.getPosY() * valeur;
		triangle.vertex1.posZ = triangle.vertex1.getPosZ() * valeur;
		
		triangle.vertex2.posX = triangle.vertex2.getPosX() * valeur;
		triangle.vertex2.posY = triangle.vertex2.getPosY() * valeur;
		triangle.vertex2.posZ = triangle.vertex2.getPosZ() * valeur;

		triangle.vertex3.posX = triangle.vertex3.getPosX() * valeur;
		triangle.vertex3.posY = triangle.vertex3.getPosY() * valeur;
		triangle.vertex3.posZ = triangle.vertex3.getPosZ() * valeur;

		
    }    

}

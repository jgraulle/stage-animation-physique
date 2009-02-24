#ifndef _SKINDATA_H_
#define _SKINDATA_H_

#include <string>
#include <map>
#include <list>

#include "MocapData.h"

using namespace std;

class SkinData {
public:

    // Classe vertex
    class Vertex {

        friend class SkinData;

        public:
            typedef map<const int, float> WeightIndexedByBoneId;

        private:
           float posX;
           float posY;
           float posZ;
           float normX;
           float normY;
           float normZ;
           float textU;
           float textV;


           WeightIndexedByBoneId listeWeightByBoneId;

        public:
            Vertex() {
                this->posX = 0; this->posY = 0; this->posZ = 0;
                this->normX = 0; this->normY = 0; this->normY = 0;
                this->textU = 0; this->textV = 0;
            }
            Vertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float textU, float textV) {
                this->posX = posX; this->posY = posY; this->posZ = posZ;
                this->normX = normX; this->normY = normY; this->normY = normY;
                this->textU = textU; this->textV = textV;

            }

            float getPosX() const { return posX; }
            float getPosY() const { return posY; }
            float getPosZ() const { return posZ; }
            float getNormX() const { return normX; }
            float getNormY() const { return normY; }
            float getNormZ() const { return normZ; }
            float getTextU() const { return textU; }
            float getTextV() const { return textV; }

            void addWeightInfluence(int boneId, float influence) {
                // Si le bone ID n'a pas déjà une influence
                if (listeWeightByBoneId.find(boneId) == listeWeightByBoneId.end()) {
                     listeWeightByBoneId[boneId] = influence;
                } else {
                    // Sinon on aditionne l'influence avec celle déjà présente
                    listeWeightByBoneId[boneId] += influence;
                }


            }

            float getWeightInfluence(int boneId){
                return listeWeightByBoneId[boneId];
            }

            const WeightIndexedByBoneId& getWeightList() const{
                return listeWeightByBoneId;
            }

    };

    // Classe Triangle
    class Triangle {

        friend class SkinData;
        private:
            Vertex vertex1;
            Vertex vertex2;
            Vertex vertex3;
            string textureFileName;


        public:
            Triangle(Vertex vertex1, Vertex vertex2, Vertex vertex3, string textureFileName)
            {
                this->vertex1 = vertex1;
                this->vertex2 = vertex2;
                this->vertex3 = vertex3;
                this->textureFileName = textureFileName;
            }


            const Vertex& getVertex1() const { return vertex1; }
            const Vertex& getVertex2() const { return vertex2; }
            const Vertex& getVertex3() const { return vertex3; }
            string getTextureFileName() const { return textureFileName; }
    };

    // Liste de vertex
    typedef list<Triangle> TriangleList;


private:

    // ATTRIBUTS
    TriangleList triangleList;
    // On stocke aussi la posture de référence
    MocapData * referencePosData; // Il s'agit donc d'une animation possédant une seule frame

    // Le chemin ou l'on peut aller chercher les textures (les textures ne sont pas pré-chargées pour l'instant)
    string texturesPath;

public:
    // CONSTRUCTEURS
    SkinData(MocapData * referencePosData);
    SkinData(const SkinData & copie);
    ~SkinData();


    // METHODES
    //Vertex *getVertex(float posX, float posY, float posZ, float normX, float normY, float normZ, float textU, float textV);
    void addTriangle(Vertex vertex1, Vertex vertex2, Vertex vertex3, string textureFileName);
    const TriangleList& getTriangleList() const;
    MocapData * getReferencePosData();

    string getTexturesPath();
    void setTexturesPath(string texturesPath);

    void resizeSkin(float valeur);

};

#endif

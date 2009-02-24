#include "MocapLoader.h"
#include "BvhLoader.h"
#include "SkinLoader.h"

MocapLoader::MocapLoader() {
    mocapData = NULL;
    skeletalData = NULL;
    skinData = NULL;
}

MocapLoader::~MocapLoader() {
    
}

MocapData * MocapLoader::getMocapData() {
    return mocapData;   
}


SkeletalData * MocapLoader::getSkeletalData() {
    return skeletalData;
}

SkinData * MocapLoader::getSkinData() {
    return skinData;   
}
    
void MocapLoader::setMocapData(MocapData * mocapData) {
    this->mocapData = mocapData;   
}


void MocapLoader::setSkeletalData(SkeletalData * skeletalData) {
    this->skeletalData = skeletalData;
}

void MocapLoader::setSkinData(SkinData * skinData) {
    this->skinData = skinData;
}

MocapLoader * MocapLoader::getInstanceOfAppriopriateLoader(string nomFichier) {
    // Détermine l'extension du fichier
    string extension = nomFichier.substr(nomFichier.find_last_of('.')+1); 
    std::transform( extension.begin(), extension.end(), extension.begin(), static_cast<int (*)(int)>(std::tolower) );
    
    // Selection du loader selon l'extension du fichier à charger
    if (extension == "bvh") {
        return new BvhLoader();   
    } else if (extension == "smd") {
        return new SkinLoader();      
    } else if (extension == "extensionDeFouPourPlusTard") {
        // Pour extensions futures
        return NULL;
    } else {
        // Extension non gérée
        return NULL;
    }
}

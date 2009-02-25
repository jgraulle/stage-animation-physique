cmake_minimum_required(VERSION 2.6)

# Déclaration du nom du projet
PROJECT(testMoteurGraphique)

SET(CMAKE_VERBOSE_MAKEFILE ON)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE) 

IF(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# recherche de libbvh
SET(BVH_INCLUDE ../bvh/src)
SET(BVH_LIBRARY_NAME bvh)
SET(BVH_LIBRARY_DIR ../bvh/lib)
INCLUDE_DIRECTORIES(${BVH_INCLUDE})
LINK_DIRECTORIES(${BVH_LIBRARY_DIR})

# Generation de la liste des fichiers sources
FILE(GLOB_RECURSE source_files src/*)

# Declaration de l'executable
ADD_EXECUTABLE(test ${source_files})

# Ajout des librairies
target_link_libraries(test ${BVH_LIBRARY_NAME})

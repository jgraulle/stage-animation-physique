cmake_minimum_required(VERSION 2.6)

# Déclaration du nom du projet
PROJECT(moteurGraphique)

SET(CMAKE_VERBOSE_MAKEFILE OFF)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE) 

IF(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# recherche de openGL
FIND_PACKAGE(OpenGL)
IF (OPENGL_FOUND)
	MESSAGE("OPENGL FOUND")
	MESSAGE(${OPENGL_LIBRARIES})
ELSE (OPENGL_FOUND)
	MESSAGE("OPENGL NOT FOUND")
ENDIF (OPENGL_FOUND)
FIND_PACKAGE(GLU)

# recherche de GLFW
IF (WIN32)
	# TODO rechercher dossier minGW
	SET(GLFW_INCLUDE C:/MinGW/include)
ELSE (WIN32)
	# TODO dpkg
	SET(GLFW_INCLUDE /usr/include /usr/local/include)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${GLFW_INCLUDE})

# recherche de devIL
IF (WIN32)
	# TODO rechercher dossier minGW
	SET(DEVIL_INCLUDE C:/MinGW/include)
ELSE (WIN32)
	# TODO dpkg
	SET(DEVIL_INCLUDE /usr/include /usr/local/include)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${DEVIL_INCLUDE})

# Generation de la liste des fichiers sources
FILE (GLOB_RECURSE source_files src/*)

# Declaration de l'executable
SET(LIBRARY_OUTPUT_PATH lib)
ADD_LIBRARY(MoteurGraphique STATIC ${source_files})

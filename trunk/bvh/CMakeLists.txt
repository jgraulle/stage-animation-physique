cmake_minimum_required(VERSION 2.6)

# Declaration du nom du projet
PROJECT(bvh)

SET(CMAKE_VERBOSE_MAKEFILE OFF)

IF (NOT CMAKE_BUILD_TYPE)
	SET(CMAKE_BUILD_TYPE "Release")
ENDIF (NOT CMAKE_BUILD_TYPE) 

IF(COMMAND cmake_policy)
   cmake_policy(SET CMP0003 NEW)
ENDIF(COMMAND cmake_policy)

# Generation de la liste des fichiers sources
FILE (GLOB_RECURSE source_files src/*)

# Declaration de l'executable
SET(LIBRARY_OUTPUT_PATH lib)
ADD_LIBRARY(bvh STATIC ${source_files})

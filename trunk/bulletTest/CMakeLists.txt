cmake_minimum_required(VERSION 2.6)

# Declaration du nom du projet
PROJECT(bulletTest)

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
	SET(OPENGL_gl_LIBRARY opengl32)
	SET(OPENGL_glu_LIBRARY glu32)
ENDIF (OPENGL_FOUND)
FIND_PACKAGE(GLU)

# recherche de GLFW
IF (WIN32)
	# TODO rechercher dossier minGW
	SET(GLFW_INCLUDE C:/MinGW/include)
	SET(GLFW_LIBRARY_NAME glfw)
	#SET(GLFW_LIBRARY_DIR C:/MinGW/lib)
ELSE (WIN32)
	# TODO verifier que la lib est bien installe
	SET(GLFW_INCLUDE /usr/include /usr/local/include)
	SET(GLFW_LIBRARY_NAME pthread glfw X11 Xrandr)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${GLFW_INCLUDE})
#link_directories(${GLFW_LIBRARY_DIR})

# recherche de devIL
IF (WIN32)
	# TODO rechercher dossier minGW
	SET(DEVIL_INCLUDE C:/MinGW/include)
	SET(DEVIL_LIBRARY_NAME devil)
	SET(ILU_LIBRARY_NAME ilu)
	SET(ILUT_LIBRARY_NAME ilut)
	#SET(ILUT_LIBRARY_DIR C:/MinGW/lib)
ELSE (WIN32)
	# TODO verifier que la lib est bien installe
	SET(DEVIL_INCLUDE /usr/include /usr/local/include)
	SET(DEVIL_LIBRARY_NAME IL)
	SET(ILU_LIBRARY_NAME ILUT)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${DEVIL_INCLUDE})
#link_directories(${DEVIL_LIBRARY_DIR})

# recherche de bullet
IF (WIN32)
	# TODO rechercher dossier minGW
	SET(BULLET_INCLUDE ../../bullet-2.74/src)
	SET(BULLET_DYNAMICS_LIBRARY_NAME bulletdynamics)
	SET(BULLET_COLLISION_LIBRARY_NAME bulletcollision)
	SET(BULLET_MATH_LIBRARY_NAME linearMath)
	SET(BULLET_LIBRARY_DIR ../../bullet-2.74/lib)
ELSE (WIN32)
	# TODO verifier que la lib est bien installe
	SET(BULLET_INCLUDE /usr/include /usr/local/include)
	SET(BULLET_DYNAMICS_LIBRARY_NAME BulletDynamics)
	SET(BULLET_COLLISION_LIBRARY_NAME BulletCollision)
	SET(BULLET_MATH_LIBRARY_NAME LinearMath)
ENDIF (WIN32)
INCLUDE_DIRECTORIES(${BULLET_INCLUDE})
link_directories(${BULLET_LIBRARY_DIR})

# recherche de moteurGraphique
SET(MOTEUR_GRAPHIQUE_INCLUDE ../moteurGraphique/src)
SET(MOTEUR_GRAPHIQUE_LIBRARY_NAME MoteurGraphique)
SET(MOTEUR_GRAPHIQUE_LIBRARY_DIR ../moteurGraphique/lib)
INCLUDE_DIRECTORIES(${MOTEUR_GRAPHIQUE_INCLUDE})
LINK_DIRECTORIES(${MOTEUR_GRAPHIQUE_LIBRARY_DIR})

# recherche de libbvh
SET(BVH_INCLUDE ../bvh/src)
SET(BVH_LIBRARY_NAME bvh)
SET(BVH_LIBRARY_DIR ../bvh/lib)
INCLUDE_DIRECTORIES(${BVH_INCLUDE})
LINK_DIRECTORIES(${BVH_LIBRARY_DIR})

# Generation de la liste des fichiers sources
FILE(GLOB_RECURSE source_files src/*)

# Declaration de l'executable
ADD_EXECUTABLE(bulletTest ${source_files})

# Ajout des librairies
target_link_libraries (
	bulletTest
	${BVH_LIBRARY_NAME} 
	${BULLET_DYNAMICS_LIBRARY_NAME} ${BULLET_COLLISION_LIBRARY_NAME} ${BULLET_MATH_LIBRARY_NAME} 
	${MOTEUR_GRAPHIQUE_LIBRARY_NAME}
	${DEVIL_LIBRARY_NAME} ${ILU_LIBRARY_NAME} ${ILUT_LIBRARY_NAME}
	${GLFW_LIBRARY_NAME} ${OPENGL_gl_LIBRARY} ${OPENGL_glu_LIBRARY}
)

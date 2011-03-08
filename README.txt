Instruction de compilation :

Ce projet utilise l'outil cmake pour la gestion de la chaine de compilation. Il a été testé sous linux et windows (compilateur mingw) et il devrait marcher sous Mac.

Il faut installer les librairies :
- opengl-dev
- glfw
- devil
- bullet

Chaque dossier correspond a un projet, fournissant une librairie statique ou un executable. Pour le compiler sous linux il n'y a aucune modification a réaliser, mais sous windows il faut modifier le fichier "CMakeLists.txt" par rapport au dossier d'installation des librairies (qui sont par defaut C:/MinGW/), ensuite il faut générer le makefile avec la commande cmake -G"SYSTEME" (aidez vous de la commande cmake pur déterminer le système)

Il faut d'abord compiler les librairies :
- moteurGraphique
- bvh

Apres vous pouvez compiler :
- moteurGraphiqueTest
- bvhTest
- bulletTest

La version finale du stage est l'executable généré par le projet bulletTest.
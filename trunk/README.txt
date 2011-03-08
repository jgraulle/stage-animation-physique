Instruction de compilation :

Ce projet utilise l'outil cmake pour la gestion de la chaine de compilation. Il a �t� test� sous linux et windows (compilateur mingw) et il devrait marcher sous Mac.

Il faut installer les librairies :
- opengl-dev
- glfw
- devil
- bullet

Chaque dossier correspond a un projet, fournissant une librairie statique ou un executable. Pour le compiler sous linux il n'y a aucune modification a r�aliser, mais sous windows il faut modifier le fichier "CMakeLists.txt" par rapport au dossier d'installation des librairies (qui sont par defaut C:/MinGW/), ensuite il faut g�n�rer le makefile avec la commande cmake -G"SYSTEME" (aidez vous de la commande cmake pur d�terminer le syst�me)

Il faut d'abord compiler les librairies :
- moteurGraphique
- bvh

Apres vous pouvez compiler :
- moteurGraphiqueTest
- bvhTest
- bulletTest

La version finale du stage est l'executable g�n�r� par le projet bulletTest.
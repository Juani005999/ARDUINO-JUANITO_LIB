# ARDUINO-JUANITO_LIB
Librairie d'objets C++ pour développements Arduino

Cette librairie est en cours de développement.
Les objets contenus dans cette librairie sont destinés à fonctionner dans des traitements asynchrones (sans "delay(XX)").
Pour plus d'informations sur les techniques de développement asynchrone, référez-vous à l'utilisation de "millis()" couplée à des chronos plutôt qu'à l'utilisation de "delay(XX)".
Cette librairie à pour fonction de mutualiser l'implementation de fonctionnalités par la réalisation d'un modèle/objets C++.
Ces objets fonctionnent sur le principe suivant :
  - Déclaration de l'objet : dans l'en-tête de votre fichier maître .ino
  - Initialisation de l'objet : dans le setup de votre fichier maître .ino
  - Utilisation de l'objet : dans le Loop de votre fichier maître .ino

Cette librairie va s'enrichir progressivement de nouveaux objets, au grés des besoins et composant rencontrés ;)

Utilisation :
  - Pour utiliser cette librairie, copiez le répertoire "JUANITO_LIB" dans votre répertoire de librairies Arduino (par défaut : "MesDocuments\Arduino\libraries")
  - Ces objets sont destiné à fonctionner dans des environnements asynchrone. Si votre traitement (Loop) principal contient des pauses ("delay(XX)"), ces objets ne fonctionneront pas correctement.
  - Vous trouverez des exemples d'implémentation de ces objets dans la rubrique "Sample projects".

Enjoy, longue vie et prospérité :)

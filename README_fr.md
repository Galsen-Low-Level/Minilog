

Comme son nom l'indique, minilog est un outil dédié à la journalisation native
basé sur les capacités de terminaison (termcapabilities).
Il s'adapte automatiquement aux différents terminaux, grâce à son support des termcapabilities, et est souvent déjà présent sur la plupart des systèmes Unix.

Léger et facile à utiliser, il n'a pas de dépendances externes,
ce qui réduit l'empreinte mémoire lors de son utilisation.

Par défaut, pour un affichage rapide, la sortie standard n'est pas mise en mémoire tampon, c'est-à-dire qu'elle n'est pas temporisée.

 La sortie standard est gérée par le système d'exploitation, ce qui permet un affichage instantané des messages.
il peut être rapidement intégré dans votre flux de travail, car le projet est construit dans un seul fichier appelé objet partagé.

Il peut être intégré dans votre code en créant un lien vers lui.


REMARQUE : pour l'instant, la prise en charge de python est en cours de construction et sera proposée sous la forme d'un paquetage utilisant ce fichier .so.

Comment le construire ?
Pour l'installer ou le construire, nous avons utilisé le gestionnaire de construction appelé meson.

et voici les commandes à taper :
1 -> entrer dans le projet
bash
$ cd Minilog

```
2 -> démarrer la construction avec le méson
```bash
$ meson setup build # construit le projet
$ meson compile -C build # compile le projet

```


A la fin de la compilation, vous aurez un fichier .so qui pourra être intégré dans votre environnement.


Appercu :

Pour obtenir un appercu rapide, un dockerfile est fourni pour construire l'environnement et obtenir une vue d'ensemble.
Ou vous pouvez directement utiliser le projet de construction comme décrit ci-dessus (voir la section sur comment construire)
et dans le dossier de construction vous aurez l'exécutable.

N'hésitez pas à jouer avec le fichier main.c, qui contient des exemples de code avec lesquels vous pouvez également jouer.

Pour les plus timorés, utilisez Docker :
``bash
$ docker build . -t minilog
$ docker run minilog # l'exécuter
```

Voici quelques captures d'écran pour voir à quoi ressemblent les logs




NOTE : Ce projet est dans son état initial et a besoin de retours ou de demandes de fonctionnalités pour l'améliorer autant que possible.
      Ce projet a pour but de satisfaire un grand nombre d'utilisateurs, alors n'hésitez pas à soumettre des pull requests ou des problèmes en plaçant un drapeau.
      feat : pour que je sache qu'il s'agit d'une demande de fonctionnalité,
      idea : pour discuter d'une idée
      question : s'il s'agit d'une question, je serai ravi d'y répondre.





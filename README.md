 # SPACE INVADERS (C / SDL3 & Ncurses)

 ## Introduction
 Ce projet est une réimplémentation du célèbre jeu d'arcade **Space Invaders** en langage C. Il a été conçu pour démontrer une architecture logicielle modulaire respectant le patron de conception **MVC (Modèle-Vue-Contrôleur)**.

 Le jeu propose deux interfaces graphiques interchangeables au lancement :
 1.  **Mode Graphique :** Utilise la librairie **SDL3** (Sprites, Audio, Texte).
 2.  **Mode Texte :** Utilise la librairie **Ncurses** (Rendu ASCII dans le terminal).

 ## Architecture MVC

 Le projet est strictement divisé en trois composants indépendants :

 ### 1. Le Modèle (`src/model/`)
 Contient toute la logique métier et les données du jeu. Il est totalement indépendant de l'affichage.
 * **Entités :** Gestion du `Player`, `Swarm` (Ennemis/Boss), `Projectiles`, `Bunker`.
 * **Physique :** Détection des collisions (AABB), mouvement des entités, calcul des trajectoires.
 * **État :** Gestion du score, des vies, et des états du jeu (Menu, Jeu, Game Over).

 ### 2. La Vue (`src/view/`)
 Responsable uniquement de l'affichage. Elle ne modifie jamais les données du modèle.
 * **Vue SDL (`sdl_view.c`) :** Charge les textures, joue les sons et dessine les sprites à l'écran.
 * **Vue Ncurses (`ncurses_view.c`) :** Convertit les coordonnées du jeu en grille de caractères ASCII pour le terminal.

 ### 3. Le Contrôleur (`src/controller/`)
 Gère les entrées utilisateur et met à jour le modèle.
 * Capture les événements (Clavier, Fenêtre).
 * Traduit les touches (ex: Flèche Gauche) en actions abstraites (`MOVE_LEFT`).

 ---

 ## Dépendances et Pré-requis

 Le projet inclut ses propres librairies dans le dossier `3rdParty` pour assurer la compatibilité, mais votre système doit disposer des outils de compilation de base.

 **Outils système requis (Linux/Debian/Ubuntu) :**
 * `gcc` (Compilateur C)
 * `make` (Build system)
 * `cmake` (Utilisé pour compiler les librairies tierces)
 * `valgrind` (Pour les tests de mémoire)

 **Librairies incluses (dans `3rdParty/`) :**
 * SDL3 (3.2.x)
 * SDL3_image
 * SDL3_mixer
 * SDL3_ttf
 * SDL3_gfx
 * ncurses

 ---

 ## Compilation et Installation

 Le projet utilise un script automatisé pour gérer la compilation des librairies tierces et du jeu.

 ### 1. Préparation
 Donnez les droits d'exécution aux scripts :
 ```bash
 chmod +x build.sh clean.sh
 ```

 ### 2. Compilation
 Lancez le script de construction. Ce script va compiler les librairies SDL/Ncurses (ce qui peut prendre quelques minutes la première fois) puis compiler le jeu.
 ```bash
 ./build.sh
 ```

 ### 3. Nettoyage (Optionnel)
 Pour supprimer tous les fichiers générés et recommencer une compilation propre :
 ```bash
 ./clean.sh
 ```

 ---

 ## Exécution

 Une fois la compilation terminée, l'exécutable `spaceinvaders` se trouve dans le dossier `build/`.

 ### Lancer en mode Graphique (SDL)
 C'est le mode par défaut, avec sprites, animations et sons.
 ```bash
 ./build/spaceinvaders sdl
 # Ou via le Makefile :
 make run-sdl
 ```

 ### Lancer en mode Texte (Ncurses)
 Mode rétro s'exécutant directement dans votre terminal.
 ```bash
 ./build/spaceinvaders ncurses
 # Ou via le Makefile :
 make run-ncurses
 ```

 ---

 ## Commandes Clavier

 Les contrôles sont identiques pour les deux interfaces.

 | Contexte | Touche | Action |
 | :--- | :---: | :--- |
 | **Menu** | `ENTRÉE` | Démarrer le jeu |
 | | `Q` ou `ESC` | Quitter le jeu |
 | **En Jeu** | `←` / `→` | Déplacer le vaisseau |
 | | `ESPACE` | Tirer |
 | | `P` | Mettre en Pause / Reprendre |
 | | `ESC` | Abandonner (Retour Menu) |
 | **Game Over** | `ENTRÉE` | Retour au Menu |

 ---

 ## Vérification Mémoire (Valgrind)

 Le projet a été testé pour garantir l'absence de fuites de mémoire. Un fichier de suppression (`mysuppressions.supp`) est fourni pour ignorer les faux positifs liés aux drivers graphiques.

 Pour lancer l'analyse mémoire :
 ```bash
 make valgrind
 ```
 Le rapport sera généré dans le fichier `valgrind_report.txt`.

 ---

 ## Fonctionnalités

 * **Niveaux progressifs :** Vagues d'ennemis classiques, puis apparition d'un Boss au niveau 2.
 * **Système de Boss :** Le boss possède une barre de vie, des mouvements et des patterns de tir uniques.
 * **Audio (SDL) :** Musique de fond, bruitages de tir et d'explosion (via SDL_mixer).
 * **Persistance :** Sauvegarde automatique du meilleur score (High Score) dans un fichier local.
 * **Animations :** Sprites animés pour les ennemis et effets de particules pour le moteur du joueur.

 ---

 ## Auteurs et Licence

 Projet réalisé dans le cadre du cours de Programmation C.
 Code source sous licence libre (MIT/Unlicense).

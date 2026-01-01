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
 * **Player :** Gestion des coordonnées du vaisseau, des **points de vie (PV)**, du **score courant** et du meilleur score (High Score).
 * **Swarm (Essaim) :** Gestion centralisée des ennemis, de leurs mouvements de groupe, et de l'intelligence artificielle du **Boss**.
 * **Projectiles :** Utilisation d'un **Object Pool** (mémoire pré-allouée) pour gérer les tirs du joueur et des ennemis sans allocations dynamiques constantes.
 * **Bunkers :** Gestion des boucliers destructibles pixel par pixel (ou bloc par bloc).
 * **Game State :** Machine à états finis gérant la phase active du jeu (Menu, Jeu, Pause, Game Over) et la transition de niveaux.

 ### 2. La Vue (`src/view/`)
 Responsable uniquement de l'affichage. Elle ne modifie jamais les données du modèle.
 * **Vue SDL (`sdl_view.c`) :** Charge les textures, joue les sons, affiche le HUD et dessine les sprites à l'écran.
 * **Vue Ncurses (`ncurses_view.c`) :** Convertit les coordonnées flottantes du jeu en grille de caractères ASCII pour le terminal.

 ### 3. Le Contrôleur (`src/controller/`)
 Gère les entrées utilisateur et met à jour le modèle.
 * Capture les événements (Clavier, Fenêtre).
 * Traduit les touches (ex: Flèche Gauche) en actions abstraites (`MOVE_LEFT`) pour le modèle.

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
 | **Game Over** | `ENTRÉE` | Retour au Menu (Sauvegarde le score) |

 ---

 ## Vérification Mémoire (Valgrind)

 Le projet a été testé pour garantir l'absence de fuites de mémoire. Un fichier de suppression (`mysuppressions.supp`) est fourni pour ignorer les faux positifs liés aux drivers graphiques.

 Pour lancer l'analyse mémoire :
 ```bash
 make valgrind
 ```
 Le rapport sera généré dans le fichier `valgrind_report.txt`.

 ---

 ## Fonctionnalités Techniques

 * **Game Loop & Delta Time :** Le jeu utilise un pas de temps variable (Delta Time) pour la physique, mais impose une limite de **60 FPS** pour garantir une vitesse constante sur toutes les machines.
 * **Système de Boss :** Apparition d'un Boss au niveau 2 avec barre de vie et comportement spécifique.
 * **Audio (SDL) :** Musique de fond, bruitages de tir et d'explosion (via SDL_mixer).
 * **Persistance (I/O) :** Sauvegarde automatique du meilleur score dans un fichier **JSON** (`savegame.json`). Le chemin est résolu dynamiquement pour être toujours situé dans le dossier de l'exécutable (`build/`).
 * **Animations :** Sprites animés pour les ennemis et effets de particules pour le moteur du joueur.

 ---

 ## Auteurs et Licence
 Dhiaeddine MERAD
 Abed YLIES

 Projet réalisé dans le cadre du cours de Programmation C.
 Code source sous licence libre MIT.

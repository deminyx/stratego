#ifndef DECIDEMOVE_H
#define DECIDEMOVE_H

/**
* Choix du type de mouvement à faire au prochain tour
* @param SGameState gameState
* 	l'état du jeu courant
* @param int normalMoves
* 	Nombre de mouvements non risqués
* @param int priorityMoves
* 	Nombre de mouvements risqués
* @return int
*	Choix du type : 0 si mouvement risqué ou 1 si mouvement non risqué
*/
int chooseTypeOfMove(const SGameState * const gameState, int normalMoves, int priorityMoves);

/**
* Renvoie le mouvement à faire qui a été décidé par l'IA
* @param SGameState gameState
* 	l'état du jeu courant
* @return SMove
* 	mouvement à faire, décidé par l'IA
*/
void decideMove(const SGameState * const  gameState);

/**
* calcule la probabilité de risque de la force des pieces ennemies voisines inconnues 
* ou les pieces ennemies inconnues à attaquer directement
* @param riskedMoves est le tableau qui contient tous les movements courants risqués
*/
float riskProbability( const SGameState * const gameState,SPos myPosition,SPos enemyPosition);

/**
* Classe les mouvements possibles en fonction du risque encouru
* @param riskedMoves est le tableau qui contient tous les movements courants risqués
* @param normalMoves est le tableau qui contient tous les movements courants  qui ne sont pas risqués
*/
void evaluateMoves(const SGameState * const gameState,GroupMoves *normalMoves,GroupMoves *riskedMoves);

/**
* Donne l'information sur la pièce ennemie voisine pour évaluer le risque encouru si on effectue le mouvement
* @param myPiece est la pièce qui effectue le mouvement 
* @param enemyPiece est la pièce ennemie voisine à analyser pour donne un grade de risque au mouvement effectuer par myPiece
* @param evaluationType permet de savoir si on evalue la piece ennemie voisine ou si on attaque l'ennemi
*/
float attributionRank(EPiece myPiece,EPiece enemyPiece,bool evaluationType);

/** 
* Permet de connaître le nombre d'ennemis encore présents sur le plateau
* 	@param gameState est le gameState du jeu
*/
int nbAliveEnemies(const SGameState * const gameState);

/**
* Donne une priorité aux mouvements normaux remplissant certains critères
* en attribuant un nombre au parametre interne caution de nomalMoves 
* @param normalMoves est le tableau qui contient tous les movements courants  qui ne sont pas risqués
*/
void normalClassication(const SGameState * const gameState, GroupMoves *normalMoves);

/**
* Donne la priorité au mouvements normaux (sans risque d'être attaqué) en fonction du nombre de pièce énnemie environante 
* @param numEnemy est le nombre de pièces ennemies environantes à echapper
*/
float giveNormalRank(int numEnemy);

/**
* Donne une priorité globale au mouvements et éliminer les doublons
* @param priorityMoves est le tableau de mouvement risqué avec priorité globale sans doublons par mouvement
* @param riskedMoves est le tableau qui contient tous les movements courants risqués avec d'éventuels doublons et priorités differentes par mouvement
* @param gameState actuel du jeu
*/
void globalEvaluation(GroupMoves *priorityMoves, GroupMoves riskedMoves, const SGameState * const gameState);

/**
* Cherche les occurences d'un mouvement dans un tableau de mouvement
* @param riskedMoves tableau dans lequel s'effectue la recherche
* @param buffer tableau dans lequel sont copiés les differentes occurences du mouvement cherché
*/
void findOccurences(SMove movement,GroupMoves riskedMoves,GroupMoves *buffer);

/**
* Donne le taux de risque global d'un ensemble de mouvements
* @param buffer est tableau contenant tous les mouvements pour lequel on calculera la probabilité globale puis converti en taux de risque
*/
float globalProbability(GroupMoves buffer);

/**
* Permet de savoir si un mouvement est present dans un tableau de mouvement
* @param mouvement est le mouvement dont on evalue sa presence ou non dans le tableau de mouvement
* @param buffer est le tableau de mouvement dans lequel on evaluera la presence ou non de @param mouvement
*/
bool isMovePresent(SMove mouvement, GroupMoves buffer);

/**
* Permet de vider une liste de mouvement en initialisant toutes ses variables à 0
* @param buffer est le tableau à vider
*/
void emptyList(GroupMoves *buffer);

/**
* Fonction qui renvoie le mouvement comportant le moins de risque, 
* le meilleur mouvement du GroupMoves passé en paramètre
* @param GrouMoves moves
* 	GroupMoves dont on va prendre le meilleur mouvement
*/
SMove takeBestMove(GroupMoves moves);

/**
* Donne le mouvement à effectuer par l'IA
* 
*/
SMove chooseMove(const SGameState * const gameState, GroupMoves moves);

#endif

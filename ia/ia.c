#include <stdbool.h>
#include "structures.h"
#include "ia.h"

typedef enum{
	str_default=0,
	offensive,
	defensive,
	malicious,
	protective,
	risked,
	bluff,
	agressive,
}Strategy;

typedef enum{
	left=0,
	right,
	top,
	bottom,
}Direction;

// Structure qui sera interne à l'ia
typedef struct{
	SBox box;
	bool isVisible;
	bool isBomb;
}InfoPiece;

EColor m_color; // Couleur des pièces de l'IA
SPos m_armyPos, m_enemyPos; // Variables sauvegardant la position des pièces avant un combat
SPiece m_armyPiece, m_enemyPiece; // Variables sauvegardant le type des pièces avant un combat
SMove m_movements[172]; // Tableau contenant les mouvements possibles à chaque tour
SMove m_decidedMove; // Mouvement décidé après une réflexion par l'IA
int m_nbMove; // Nombre de mouvements enregistrés dans le tableau des mouvements
InfoPiece m_board[10][10]; // Tableau de la structure InfoPiece, qui stocke des pièces et des informations dessus
Strategy m_strategy; // Stratégie choisie
int m_caution; // Variable pour prise de risque : vaut 0 si aucun risque à prendre, 10 si faire des mouvements très risqués
bool m_myMove; // Variable pour connaître le mouvement que l'on a fait au tour précédent : false = mouvement normal et true = attaque
bool m_hisMove; // Variable pour connaître le mouvement que l'ennemi a fait au tour précédent : false = mouvement normal et true = attaque


void InitLibrary(char name[50])
{
	printf("InitLibrary\n");
	strcpy(name,"Fabien Picarougne");
}

void StartMatch()
{	
	printf("StartMatch\n");
}

void StartGame(const EColor color, EPiece boardInit[4][10]){
	/* Initialisation du tableau de l'IA avec positionement de pions*/
	printf("StartGame\n");
	m_color = color;
	m_fight = false;
	m_strategy = str_default;
	m_caution = 5;

	switch(m_strategy){

		case defensive:
			/* placement du drapeau */
			boardInit[0][0] = EPflag;
			/* placement des bombes */
			boardInit[1][0] = boardInit[0][1] = boardInit[0][4] = boardInit[1][5] = boardInit[0][6] = boardInit[2][8] = EPbomb;
			/* placement de l'espion */
			boardInit[2][3] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[3][4] = boardInit[3][5] = boardInit[3][8] = boardInit[2][5] = boardInit[1][9] = boardInit[0][9] = EPscout;
			/* placements des démineurs */
			boardInit[1][2] = boardInit[1][3] = boardInit[1][6] = boardInit[1][7] = boardInit[0][3] = EPminer;
			/* placement des sergents */
			boardInit[2][0] = boardInit[2][6] = boardInit[0][5] = boardInit[0][7] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][9] = boardInit[1][4] = boardInit[1][8] = boardInit[0][8] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][1] = boardInit[2][2] = boardInit[2][4] = boardInit[2][7] = EPcaptain;
			/* placement des majors */
			boardInit[3][2] = boardInit[3][7] = boardInit[1][1] = EPmajor;
			/* placement des colonels */
			boardInit[2][9] = boardInit[0][2] = EPcolonel;
			/* placements du général */
			boardInit[3][3] = EPgeneral;
			/* placement du maréchal */
			boardInit[3][6] = EPmarshal;
		break;

		case str_default:
			/* placement du drapeau */
			boardInit[0][6] = EPflag;
			/* placement des bombes */
			boardInit[2][6] = boardInit[2][8] = boardInit[1][6] = boardInit[1][8] = boardInit[0][5] = boardInit[0][7] = EPbomb;
			/* placement de l'espion */
			boardInit[1][3] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][3] = boardInit[3][5] = boardInit[3][8] = boardInit[2][1] = boardInit[2][4] = boardInit[0][1] = boardInit[0][9] = EPscout;
			/* placements des démineurs */
			boardInit[0][0] = boardInit[0][2] = boardInit[0][3] = boardInit[0][8] = boardInit[3][7] = EPminer;
			/* placement des sergents */
			boardInit[1][1] = boardInit[1][7] = boardInit[1][9] = boardInit[0][4] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][2] = boardInit[2][7] = boardInit[2][9] = boardInit[1][5] = EPlieutenant;
			/* placement des capitaines */
			boardInit[3][4] = boardInit[3][9] = boardInit[2][5] = boardInit[1][0] = EPcaptain;
			/* placement des majors */
			boardInit[2][2] = boardInit[1][2] = boardInit[1][4] = EPmajor;
			/* placement des colonels */
			boardInit[3][1] = boardInit[2][3] = EPcolonel;
			/* placements du général */
			boardInit[3][6] = EPgeneral;
			/* placement du maréchal */
			boardInit[2][0] = EPmarshal;
		break;

		case offensive:
			/* placement du drapeau */
			boardInit[0][5] = EPflag;
			/* placement des bombes */
			boardInit[3][4] = boardInit[3][9] = boardInit[2][3] = boardInit[2][5] = boardInit[2][7] = boardInit[2][8] = EPbomb;
			/* placement de l'espion */
			boardInit[2][6] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[3][5] = boardInit[2][2] = boardInit[1][0] = boardInit[1][8] = boardInit[0][0] = boardInit[0][7] = EPscout;
			/* placements des démineurs */
			boardInit[1][1] = boardInit[1][4] = boardInit[1][9] = boardInit[0][1] = boardInit[0][8] = EPminer;
			/* placement des sergents */
			boardInit[1][2] = boardInit[1][7] = boardInit[0][2] = boardInit[0][9] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][3] = boardInit[2][9] = boardInit[1][6] = boardInit[0][3] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][0] = boardInit[1][3] = boardInit[0][4] = boardInit[0][6] = EPcaptain;
			/* placement des majors */
			boardInit[3][7] = boardInit[2][4] = boardInit[1][5] = EPmajor;
			/* placement des colonels */
			boardInit[2][1] = boardInit[3][8] = EPcolonel;
			/* placements du général */
			boardInit[3][6] = EPgeneral;
			/* placement du maréchal */
			boardInit[3][2] = EPmarshal;
		break;

		case protective:
			/* placement du drapeau */
			boardInit[0][9] = EPflag;
			/* placement des bombes */
			boardInit[0][6] = boardInit[0][8] = boardInit[1][7] = boardInit[1][9] = boardInit[2][8] = boardInit[3][9] = EPbomb;
			/* placement de l'espion */
			boardInit[3][7] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][1] = boardInit[3][5] = boardInit[2][0] = boardInit[2][2] = boardInit[2][3] = boardInit[1][1] = boardInit[1][5] = boardInit[1][6] = EPscout;
			/* placements des démineurs */
			boardInit[3][3] = boardInit[2][7] = boardInit[0][2] = boardInit[0][3] = boardInit[0][4] = EPminer;
			/* placement des sergents */
			boardInit[2][9] = boardInit[1][3] = boardInit[1][8] = boardInit[0][7] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][0] = boardInit[3][4] = boardInit[1][4] = boardInit[0][1] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][6] = boardInit[1][0] = boardInit[1][2] = boardInit[0][5] = EPcaptain;
			/* placement des majors */
			boardInit[2][1] = boardInit[2][5] = boardInit[0][0] = EPmajor;
			/* placement des colonels */
			boardInit[3][2] = boardInit[3][6] = EPcolonel;
			/* placements du général */
			boardInit[3][8] = EPgeneral;
			/* placement du maréchal */
			boardInit[2][4] = EPmarshal;
		break;

		case risked://a faire
			/* placement du drapeau */
			boardInit[0][0] = EPflag;
			/* placement des bombes */
			boardInit[1][0] = boardInit[0][1] = boardInit[0][4] = boardInit[1][5] = boardInit[0][6] = boardInit[2][8] = EPbomb;
			/* placement de l'espion */
			boardInit[2][3] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[3][4] = boardInit[3][5] = boardInit[3][8] = boardInit[2][5] = boardInit[1][9] = boardInit[0][9] = EPscout;
			/* placements des démineurs */
			boardInit[1][2] = boardInit[1][3] = boardInit[1][6] = boardInit[1][7] = boardInit[0][3] = EPminer;
			/* placement des sergents */
			boardInit[2][0] = boardInit[2][6] = boardInit[0][5] = boardInit[0][7] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][9] = boardInit[1][4] = boardInit[1][8] = boardInit[0][8] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][1] = boardInit[2][2] = boardInit[2][4] = boardInit[2][7] = EPcaptain;
			/* placement des majors */
			boardInit[3][2] = boardInit[3][7] = boardInit[1][1] = EPmajor;
			/* placement des colonels */
			boardInit[2][9] = boardInit[0][2] = EPcolonel;
			/* placements du général */
			boardInit[3][3] = EPgeneral;
			/* placement du maréchal */
			boardInit[3][6] = EPmarshal;
		break;

		case agressive:
			/* placement du drapeau */
			boardInit[0][0] = EPflag;
			/* placement des bombes */
			boardInit[1][0] = boardInit[0][1] = boardInit[3][4] = boardInit[3][5] = boardInit[3][8] = boardInit[3][9] = EPbomb;
			/* placement de l'espion */
			boardInit[2][2] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[0][2] = boardInit[0][3] = boardInit[0][4] = boardInit[0][5] = boardInit[0][6] = boardInit[0][7] = EPscout;
			/* placements des démineurs */
			boardInit[3][2] = boardInit[2][3] = boardInit[1][3] = boardInit[1][4] = boardInit[0][8] = EPminer;
			/* placement des sergents */
			boardInit[3][6] = boardInit[3][7] = boardInit[2][9] = boardInit[0][9] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[1][2] = boardInit[1][7] = boardInit[1][8] = boardInit[1][9] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][4] = boardInit[2][6] = boardInit[2][7] = boardInit[1][6] = EPcaptain;
			/* placement des majors */
			boardInit[3][3] = boardInit[2][8] = boardInit[1][5] = EPmajor;
			/* placement des colonels */
			boardInit[2][1] = boardInit[2][5] = EPcolonel;
			/* placements du général */
			boardInit[2][0] = EPgeneral;
			/* placement du maréchal */
			boardInit[1][1] = EPmarshal;
		break;

		case malicious:
			/* placement du drapeau */
			boardInit[0][7] = EPflag;
			/* placement des bombes */
			boardInit[0][4] = boardInit[1][5] = boardInit[0][6] = boardInit[1][7] = boardInit[0][8] = boardInit[1][9] = EPbomb;
			/* placement de l'espion */
			boardInit[2][6] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[3][4] = boardInit[3][5] = boardInit[3][8] = boardInit[2][2] = boardInit[2][8] = boardInit[1][3] = EPscout;
			/* placements des démineurs */
			boardInit[2][3] = boardInit[2][7] = boardInit[1][1] = boardInit[0][0] = boardInit[0][2] = EPminer;
			/* placement des sergents */
			boardInit[1][4] = boardInit[1][6] = boardInit[0][5] = boardInit[0][9] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[3][9] = boardInit[2][1] = boardInit[0][1] = boardInit[0][3] = EPlieutenant;
			/* placement des capitaines */
			boardInit[2][4] = boardInit[1][2] = boardInit[1][0] = boardInit[1][8] = EPcaptain;
			/* placement des majors */
			boardInit[3][2] = boardInit[3][7] = boardInit[2][5] = EPmajor;
			/* placement des colonels */
			boardInit[2][9] = boardInit[2][0] = EPcolonel;
			/* placements du général */
			boardInit[3][6] = EPgeneral;
			/* placement du maréchal */
			boardInit[3][3] = EPmarshal;
		break;

		case bluff:
			/* placement du drapeau */
			boardInit[3][3] = EPflag;
			/* placement des bombes */
			boardInit[3][2] = boardInit[3][4] = boardInit[3][5] = boardInit[2][3] = boardInit[2][6] = boardInit[0][7] = EPbomb;
			/* placement de l'espion */
			boardInit[0][5] = EPspy;
			/*placement des eclaireurs */
			boardInit[3][0] = boardInit[3][1] = boardInit[3][7] = boardInit[3][8] = boardInit[3][9] = boardInit[1][7] = boardInit[0][0] = boardInit[0][4] = EPscout;
			/* placements des démineurs */
			boardInit[2][7] = boardInit[1][4] = boardInit[1][5] = boardInit[0][2] = boardInit[0][9] = EPminer;
			/* placement des sergents */
			boardInit[3][6] = boardInit[1][1] = boardInit[0][1] = boardInit[0][6] = EPsergeant;			
			/* placement des lieutenants */
			boardInit[1][0] = boardInit[1][9] = boardInit[0][3] = boardInit[0][8] = EPlieutenant;
			/* placement des capitaines */
			boardInit[1][2] = boardInit[1][3] = boardInit[1][6] = boardInit[1][8] = EPcaptain;
			/* placement des majors */
			boardInit[2][0] = boardInit[2][5] = boardInit[2][9] = EPmajor;
			/* placement des colonels */
			boardInit[2][2] = boardInit[2][8] = EPcolonel;
			/* placements du général */
			boardInit[2][1] = EPgeneral;
			/* placement du maréchal */
			boardInit[2][4] = EPmarshal;
		break;
	}
}

void EndGame()
{
	printf("EndGame\n");
}

void EndMatch()
{
	// Désallocation
	printf("EndMatch\n");
}

SMove NextMove(const SGameState * const gameState);
{
	printf("NextMove\n");
	updateData(gameState); // Première phase, mise à jour des données internes
	analyzeBoard(); // Analyse du plateau => Mise à jour des dplcmts possibles
	decideMove(gameState); // Décision du mouvement à faire
	if (!m_myMove) // Si on a fait un déplacement normal, on le sauvegarde 
		saveMove(); // On sauvegarde le plateau interne avec le mouvement que l'on va faire
	return m_decidedMove;
}

void AttackResult(SPos armyPos,EPiece armyPiece,SPos enemyPos,EPiece enemyPiece)
{	
	printf("AttackResult\n");	
	
	/* Si c'est nous qui avons engagé l'attaque */
	if (m_myMove)
	{
		analyseFight(armyPiece, enemyPiece, armyPos, enemyPos);
	}
	else
	{
		m_hisMove = true;
		analyseFight(enemyPiece, armyPiece, enemyPos, armyPos);
	}
}

void Penalty()
{
	printf("Penalty\n");
}

//---------------- Fonctions personnelles ---------------//

// Fonction interne à AttackResult
SPiece winner(SPiece A, SPiece B)
{
	/* Si la pièce visée est le drapeau,
	la pièce attaquante gagne d'office */
	if (B == EPflag)
		return A;
	/* Si l'attaquant est un espion */
	else if (A == EPspy)
	{
		/* Il gagne seulement si la pièce 
		cible est le maréchal */
		if (B == EPmarshal)
			return A;
		else
			return B;
	}
	/* Sinon si l'attaquant est un démineur */
	else if (A == EPminer)
	{
		/* Il gagne si la pièce attaquée est une 
		bombe, un espion ou un éclaireur */
		if (B < EPminer)
			return A;
		else
			return B;
	}
	/* Sinon, dans tous les autres cas */
	else
	{
		/* Si la pièce attaquée est une bombe ou si sa 
		puissance est supérieure à l'attaquant, elle gagne */
		if ((B == EPbomb) || (B > A))
			return B;
		else
			return A;
	}
}

// Fonction interne à AttackResult
void analyseFight(SPiece PieceA, SPiece PieceB, SPos APos, SPos BPos, )
{
	SPiece winner;

	if (PieceA != PieceB) // Si les deux pièces sont différentes, on analyse le fight
	{
		// On détermine le gagnant du combat		
		winner = winner(PieceA, PieceB);

		if (winner == PieceA) // Si la pièce A a attaqué et gagné, on remplace la pièce B
		{
			/* On place la pièce A sur la case où était la pièce B */
			m_board[BPos.line][BPos.col].box.piece = PieceA;
			m_board[BPos.line][BPos.col].box.content = m_board[APos.line][APos.col].box.content;
		}
		else // Si la pièce A a perdu, on sauvegarde ce qu'est la pièce B
		{	
			m_board[BPos.line][BPos.col].box.piece = PieceB;
		}		

		/* Dans tous les cas, la case d'où vient la pièce A devient vide */
		m_board[APos.line][APos.col].box.piece = EPnone;
		m_board[APos.line][APos.col].box.content = ECnone;
	}
	else // Si les deux pièces sont identiques, elles sont éliminées
	{
		/* Plus rien dans la case de la pièce A */
		m_board[APos.line][APos.col].box.piece = EPnone;
		m_board[APos.line][APos.col].box.content = ECnone;

		/* Plus rien dans la case de la pièce B */
		m_board[BPos.line][BPos.col].box.piece = EPnone;
		m_board[BPos.line][BPos.col].box.content = ECnone;
	}
}

// Première phase, mise à jour des données internes
void updateData(const SGameState * const gameState)
{
	/* Variables internes à la fonction */
	int i, j; // Variables pour les boucles
	int diff[10][10]; // Tableau sauvegardant les différences sur la plateau depuis les changements
	SPos negative, positive; // Tableaux contenant les positions qui ont changé depuis le dernier mouvement

	m_nbMove = 0; // A déplacer dans la fonction précédant l'envoi de mouvement

	/* On analyse les changements qu'il y a eu depuis notre dernier tour, 
	on stocke ça dans le tableau de positions adéquat */
	for (i=0; i < 10; i++)
	{
		for (j=0; j < 10; j++)
		{
			diff[i][j] = m_board[i][j].box.piece - gameState->board[i][j].piece;

			/* Si une pièce n'est plus dans la case en (i,j), on stocke */
			if ((m_board[i][j].box.piece - gameState->board[i][j].piece) < 0)
				temp.line = i; temp.col = j;

			/* Sinon si une pièce est arrivée en (i,j), on stocke */
			else if ((m_board[i][j].box.piece - gameState->board[i][j].piece) > 0)
				positive.line = i; positive.col = j;
		}
	}

	/* Mise à jour du plateau interne */
	m_board[positive.line][positive.col].box.piece = gameState->board[negative.line][negative.col].piece;	
	m_board[positive.line][positive.col].box.content = gameState->board[negative.line][negative.col].content;

	m_board[negative.line][negative.col].box.piece = EPnone;
	m_board[negative.line][negative.col].box.content = ECNone;

	/* Réinitialisation des valeurs */
	m_myMove = false;
	m_hisMove = false;
}

// Analyse du plateau => Mise à jour des déplacements possibles
void analyzeBoard()
{
	unsigned int i, j, compteur = 0;
	for (i=0; i<10; i++)
	{
		for (j=0; j<10; j++)
		{
			/* Si la pièce est une pièce appartenant à l'IA et qu'elle est
			déplaçable, on regarde les cases aux alentours */
			if ((m_board[i][j].box.content == m_color) && (m_board[i][j].box.piece != EPbomb) && (m_board[i][j].box.piece != EPflag))
			{				
				// Analyse du mouvement vers le bas
				addAnalyzedMove(i, j, i-1, j, 1, 0, &compteur);

				// Analyse du mouvement vers le haut
				addAnalyzedMove(i, j, i+1, j, 1, 9, &compteur);

				// Analyse du mouvement vers la gauche
				addAnalyzedMove(i, j, i, j-1, 0, 0, &compteur);

				// Analyse du mouvement vers la droite
				addAnalyzedMove(i, j, i, j+1, 0, 9, &compteur);
			}
		}
	}

	m_nbMove = compteur;
}

// Sous-fonction de l'analyse du plateau
void addAnalyzedMove(int i, int j, int new_i, int new_j, int is_i, int lim, unsigned int* compteur)
{	
	/* Déclaration des variables internes */
	int temp, val, newVal, dirLine, dirCol;
	SPos start, end;
	EColor enemyColor;

	/* Initialisation des variables contenant les positions
	initiales et finales du mouvement à analyser */
	if (is_i == 1)
	{
		val = i;
		newVal = new_i;
	}		
	else
	{
		val = j;
		newVal = new_j;
	}

	/* Initialisation des variables permettant de 
	sauvegarder la direction du mouvement à analyser */
	dirLine = new_i - i;
	dirCol = new_j - j;
	
	/* Initialisation de la couleur ennemie */
	if (m_color == ECred)
		enemyColor = ECblue;
	else
		enemyColor = ECred;

	/* Condition principale : si la case sur laquelle on veut se déplacer n'est pas hors des limites du plateau,
	et si elle ne contient ni un lac ni un allié, alors on peut s'y déplacer */
	if ((val != lim) && (m_board[new_i][new_j].box.content != EClake) && (m_board[new_i][new_j].box.content != m_color)) 
	{
		/* Affectation des positions */
		start.line = i; start.col = j;
		end.line = new_i; end.col = new_j;

		/* Ajout du mouvement dans le tableau des mouvements */
		m_movements[*compteur].start = start;
		m_movements[*compteur].end = end;
		*compteur++;

		/* Si la pièce étudiée est un éclaireur, on regarde tous les déplacements possibles en ligne */
		if (m_board[i][j].box.piece == EPscout)
		{
			/* On utilise une variable temp pour parcourir toute la ligne */
			temp = newVal;	

			/* Tant qu'on n'a pas atteint la limite du plateau et que la case cible ne contient ni un lac, 
			ni un allié, et que la case actuelle ne contient pas un ennemi, le mouvement est possible */		
			while ((temp != lim) && (m_board[new_i+dirLine][new_j+dirCol].box.content != EClake) && (m_board[new_i+dirLine][new_j+dirCol].box.content != m_color) && (m_board[new_i][new_j].box.content != enemyColor))
			{
				/* Rajout du mouvement dans le tableau des mouvements */
				start.line = i; start.col = j;
				end.line = new_i+dirLine; end.col = new_j+dirCol;
				m_movements[*compteur].start = start;
				m_movements[*compteur].end = end;
				*compteur++;

				/* Parcours de la ligne selon la direction (vers les i ou j négatifs, ou positifs) */
				if ((dirLine < 0)||(dirCol < 0))
					temp--;
				else
					temp++;

				/* Utilisation de temp selon la direction du déplacement
				(en ligne ou en colonne) */
				if (is_i == 1)
					new_i = temp;
				else
					new_j = temp;				
			}
		}						
	}
}

// Décision du mouvement à effectuer
SMove decideMove(const SGameState * const gameState)
{
	// Décision du mouvemennt
	// Penser à mettre m_myMove à true lorsqu'on attaque l'ennemi
}

// Enregistrement du plateau si déplacement simple
void saveMove()
{
	// On vide la case d'où vient la pièce
	m_board[m_decidedMove.start.line][m_decidedMove.start.col].box.piece = EPnone;
	m_board[m_decidedMove.start.line][m_decidedMove.start.col].box.content = ECnone;

	// On met la nouvelle pièce dans sa nouvelle case
	m_board[m_decidedMove.end.line][m_decidedMove.end.col].box.piece = gameState.board[m_decidedMove.start.line][m_decidedMove.start.col].piece;
	m_board[m_decidedMove.end.line][m_decidedMove.end.col].box.content = m_color;
}

// procedure interne a decideMoves
// Classement des mouvements en fonction du risque encouru
void evaluateMoves(SMove riskedMoves[], SMove normalMoves[])
{
	/* Declaration des variables internes a la procedure*/
	int i = r = n = 0;
	EColor enemyColor;

	/* Initialisation de la couleur ennemie */
	if (m_color == ECred)
		enemyColor = ECblue;
	else
		enemyColor = ECred;
	
	while(i<m_nbMove)
	{
		/* si en effectuant le mouvement je peux directement  etre attaqué en haut */
		if( m_movements[i].end.line < 9 &&  m_board[ m_movements[i].end.line + 1 ][m_movements[i].end.col].content != enemyColor)
		{
			riskedMoves[r] = m_movements[i];
			r++;
		}
		/* si en effectuant le mouvement je peux directement  etre attaqué en bas */
		else if (m_movements[i].end.line > 0  &&  m_board[ m_movements[i].end.line - 1 ][m_movements[i].end.col].content != enemyColor)
		{
			riskedMoves[r] = m_movements[i];
			r++;
		}
		/* si en effectuant le mouvement je peux directement  etre attaqué à droite*/
		else if (m_movements[i].end.col < 9 &&  m_board[ m_movements[i].end.line][m_movements[i].end.col + 1 ].content != enemyColor)
		{
			riskedMoves[r] = m_movements[i];
			r++;
		}
		/* si en effectuant le mouvement je peux directement  etre attaqué par le bas */
		else if (m_movements[i].end.col > 0 &&  m_board[ m_movements[i].end.line][m_movements[i].end.col - 1 ].content != enemyColor)
		{
			riskedMoves[r] = m_movements[i];
			r++;
		}
		/* si en effectuant le mouvement je ne risque rien */
		else
		{
			normalMoves[n] = m_movements[i];
			n++;
		}
	}	
}

// permet de savoir si pour une pièce à une position donnée,si on la deplace dans une direction donnée on est hors du tableau de jeu ou pas
bool limiteUnachieved(SPos position, Direction piecedirection)
{
	switch(piecedirection){
		case left: 
			if(SPos.col - 1 < 0)
				return false;
			else return true;
			break;
		case right:
			if(SPos.col + 1 > 9) 
				return false;
			else return true;
			break;
		case top:
			if(SPos.line + 1 > 9) 
				return false;
			else return true;
			break;
		case bottom:
			if(SPos.line - 1 < 0) 
				return false;
			else return true;
			break;
	}
}
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
//#include <math.h>
#include "game.h"

// Tous les tableaux sont comptés avec le 0,0 en bas à gauche
// Tous les indices donnés en commentaires sont comptés à partir de 0 (indice 3 = 4eme ligne du tableau)


/* procédure qui affiche la totalité du gamestate 
 * sous la forme tableau de 10*10 : [couleur | piece]
 * 2 tableaux de 11 cases comptant le nombre de pieces élimninées
 */
void DisplayGS(SGameState gamestate)
{
	int i, j; 
	for(i=0; i<10; i++)
	{
		for(j=0; j<10; j++)
		{  // l'affichage commence par le haut du gamestate
			printf(" [%d|%d] ", gamestate.board[9-i][j].content, gamestate.board[9-i][j].piece);
		}
		printf("\n");
	}

	for(i=0; i<11; i++)
	{
		printf("[%d]       [%d]\n", gamestate.redOut[i], gamestate.blueOut[i]);
	}
	printf("\n");
}

void DisplayPlayerGS(SBox board[10][10])
{
	int i, j; 
	for(i=0; i<10; i++)
	{
		for(j=0; j<10; j++)
		{  // l'affichage commence par le haut du gamestate
			printf(" [%d|%d] ", board[9-i][j].content, board[9-i][j].piece);
		}
		printf("\n");
	}

	printf("\n");
}

/* fonction de detection du mode de jeu
 * @param char *argv[]
 *			nombre de joueurs humains detectés via argv[1]=j j={0,1,2}
 *			si argv[1]==0 : IA vs IA
 *			si argv[1]==1 : P vs IA
 *			si argv[1]==2 : PVP
 * @param int argc : nombre d'arguments.  par défaut le jeu se lance en humain vs humain
 * @return value : retourne un gamemode (entier)
 */
SGameMode DetectGameMode(int argc, char* argv[])
{
	SGameMode gamemode = ERROR;
	if(argc >5 || argc == 1)
		printf("le nombre d'arguments est incorrect\n");
	if(argc > 1)
	{
		if (*argv[2] == '0')
			gamemode = HUMAN_HUMAN;
		else if (*argv[2] == '1')
			gamemode = IA_HUMAN;
		else if (*argv[2] == '2')
			gamemode = IA_IA;
		
	}
	return gamemode;

}


/* procédure d'initialisation des variables de la structure joueur 
 * au début de chaque jeu
 * initialise les deux joueurs en même temps 
 *       Dans le Gamestate des joueurs : 
 *            - Met toutes les cases qui n'appartiennent pas au joueur avec un contenu vide EPnone
 *            - Met les lacs
 *            - Place les couleurs des deux joueurs (couleur du joueur en bas et de l'adversaire en haut)
 * initialise les couleurs des pions dans les plateaux des joueurs mais pas leur valeur
 */
void Game_InitPlayer(EPlayer* player1, EPlayer* player2, SGameConfig* gameconfig, int nbCoups) // possibilité de passer directement *argv[1] pour le nbCoups
{
	player1->nbPenalty = 0;
	player2->nbPenalty = 0;
	//Maj du nombre de coups
	player2->nbCoups = nbCoups;
	player1->nbCoups = nbCoups; 
	int i, j;
	srand(time(NULL)); // initialisation de rand
	if(rand()%2== 0)
	{
		player1->Color = ECred;
		player2->Color = ECblue;
	}
	else
	{
		player1->Color = ECblue;
		player2->Color = ECred;
	}
	// Mise a jour de la couleur du joueur 1 dans la config du jeu
	gameconfig->ColorPlayer1 = player1->Color;
	// Remplissage des tableaux des joueurs
	for (i=0; i<10; i++)
	{
		for (j=0; j<10; j++)
		{
			if (i < 4) // Mise a jour des couleurs des pions dans le gamestate
			{
				// bas des plateaux des joueurs sont de la couleur du joeur
				player1->Pboard[i][j].content = player1->Color;
				player2->Pboard[i][j].content = player2->Color;
				// haut des plateaux des joueurs sont de la couleur de l'adversaire du joueur
				player1->Pboard[9-i][j].content = player2->Color;
				player2->Pboard[9-i][j].content = player1->Color;

			}
			else if (i>3)
			{ 
				// pieces du joueur adverse non renseignées pour ne pas tricher
				player1->Pboard[i][j].piece = EPnone;
				player2->Pboard[i][j].piece = EPnone;

				// lignes vides du milieu  (indices 4 et 5)
				if (i<=5)
				{
					// Couleur des cases vides : Rien
					player1->Pboard[i][j].content = ECnone;
					player2->Pboard[i][j].content = ECnone;
					// Lacs
					if ((j==2)||(j==3)||(j==6)||(j==7))
					{
						player1->Pboard[i][j].content = EClake;
						player2->Pboard[i][j].content = EClake;					
					}
				}
			}
		}
	}
}



/* procédure d'initialisation de l'etat du jeu
 * @param SGameState* gamestate
 *			pointeur vers l'etat du jeu
 */
void Game_InitGameState(SGameState* gamestate)
{
	int i, j; 
	for(i=0; i<10; i++)
	{
		for(j=0; j<10; j++)
		{
			// Toutes les cases sont vides au début du jeu
			gamestate->board[i][j].piece = EPnone;
			if (i < 4)
			{
				// Bas du gamestate (lignes de 0 à 3)
				gamestate->board[i][j].content = ECred;
				// Haut du gamestate (lignes de 6 à 9)
				gamestate->board[9-i][j].content = ECblue; 
			}
			else if ((i>=4) && (i<=5))
			{
				// La case ne contient pas de pion donc la couleur est nulle et le contenu aussi
				gamestate->board[i][j].content = ECnone;
				// Placement des lacs
				if ((j==2)||(j==3)||(j==6)||(j==7))
					gamestate->board[i][j].content = EClake;
			}
		}
	}
	for(i=0; i<11; i++)
	{
		gamestate->redOut[i] = 0; // il y a 0 eclaireurs éliminés etc
		gamestate->blueOut[i] = 0;
	}
	
}

/* fonction pour vérifier si la pièce de départ est bien placée 
 * @param : SPos start
 * 			position de départ du mouvement
 * @return value : entier pour connaitre la validité de la position
 */
int Game_CheckPosition(SPos start, EPlayer player, SGameState gamestate)
{
	int RETURN = 0; // ERROR
	int i = start.line;
	int	j = start.col;
	/* En fonction de la couleur du joueur, on regarde differentes parties du tableau du gamestate
	 * On regarde si la couleur du pion selectionné par le joueur est la bonne
	 * On regarde si sa piece est un scout : si oui, on renvoie 2 car on doit savoir pour la validité du mouvement
	 * On regarde si la piece est une piece mobile qui n'est pas un scout : @return value = 1
	 * si le pion n'est pas une piece mobile, la couleur selectionnée est mauvaise ou si la case est vide : @return value = 0
	 */
	EColor _color = gamestate.board[i][j].content; // couleur de la piece selectionnée sur le gamestate du joueur
	EPiece _piece = gamestate.board[i][j].piece;  // piece selectionnée en vrai sur le gamestate du joueur

	if (_color == player.Color) // Le joueur cherche bien à bouger ses pions
	{
		if (_piece == EPscout) 
			RETURN = 2;  // Le scout se deplace différemment
		else if ((_piece > EPbomb)&&(_piece < EPflag)&&(_piece != EPscout)) // piece != bomb, flag ou none 
				RETURN = 1; // piece valide et n'est pas le scout
	}

	return RETURN;
}

/* fonction retournant le mouvement dans le bon sens pour le gamestate
 */
SMove Game_TranslateMove(SMove move, EPlayer player, int etat)
{ // Par défaut le mouvement est pris du côté des rouges
	switch(etat)
	{
	case 1: // Inversion pour le Gamestate General et pour le test des mouvements
			if(player.Color == ECblue)
			{
				printf("mouvements inversés !!\n");
				move.start.line = 9-move.start.line;
				move.end.line = 9-move.end.line;
				move.start.col = 9-move.start.col; 
				move.end.col = 9-move.end.col; 
			}
			break;
	case 2: // Inversion pour le gamestate des joueurs
				printf("mouvements inversés !!\n");
				move.start.line = 9-move.start.line;
				move.end.line = 9-move.end.line;
				move.start.col = 9-move.start.col; 
				move.end.col = 9-move.end.col; 
				break; 
	}
	return move;
}

/* fonction pour vérifier si un mouvement est valide
 * @param : SMove move
 * 			position de début et fin du mouvement à vérifier
 * @param : EPlayer player
 *			joueur effectuant le mouvement
 * @param : SGameState gamestate
 *			Gamestate du jeu
 * @Return Value : entier pour connaitre la validité du mouvement
 * 			-1 : mouvement invalide
 *			0  : mouvement possible et sans attaque
 *			1  : mouvement possible avec attaque
 */
int Game_CheckMove(SMove move, EPlayer player, SGameState gamestate, int position)
{
	int RETOUR = 0; // par défaut déplacement possible sans attaque
	// on definie des variables au lieu de faire les opérations d'accès à chaque fois
	int StartLine, EndLine, StartCol, EndCol, i;

	// Determination de la couleur de l'adversaire
	EColor Enemy = (player.Color == ECred) ? ECblue : ECred;
		StartLine = move.start.line;
		EndLine = move.end.line;
		StartCol = move.start.col; 
		EndCol = move.end.col; 
	// idem
	unsigned int diffligne, diffcol;
		diffligne = abs(StartLine - EndLine);
		diffcol = abs(StartCol - EndCol);


	// verif de la case de destination
	if ((diffligne > 0 && (diffcol > 0)) || ((StartLine == EndLine)&&(StartCol == EndCol)) || (gamestate.board[EndLine][EndCol].content == player.Color) || (gamestate.board[EndLine][EndCol].content == EClake))
		// si il y a un deplacement en diagonale 
		// ou si le joueur essaie de rester sur la mm case
		// ou si le joueur essaie de se deplacer sur un de ses pions
		// ou si le joueur essaie de se deplacer sur un lac
		return -1; // deplacement invalide

		// Traitement des cas ou on ne peut pas deplacer le pion 
		switch (position)
		{
			case 0 : return -1; // Position de depart invalide
					break;
			case 1 : // cas pion général
					 // Deplacement d'une case seulement
					if (diffligne + diffcol != 1) return -1;// Deplacement illégal
					break;
			case 2 : // cas pion scout
					if ((diffligne > 0)&&(diffcol == 0)) // deplacement en lignes
					{
						for(i = StartLine + 1; i<EndLine-1; i++) 
						{
							if (gamestate.board[i][StartCol].piece != EPnone) return -1; // on ne peut pas déplacer le scout	
						}
					}
					if ((diffligne == 0)&&(diffcol > 0)) // deplacement en colonnes
					{
						for (i = StartCol + 1; i<EndCol-1; i++)
						{
							if (gamestate.board[StartLine][i].piece != EPnone) return -1; // on ne peut pas déplacer le scout
						}
					}
					// si on arrive ici on peut deplacer le scout
					break;
		}
		// Traitement du cas où, si le pion est déplaçable, le mouvement mène à un combat
		if (gamestate.board[EndLine][EndCol].content == Enemy)
			RETOUR = 1;
return RETOUR;
}



/* procédure pour effectuer le mouvement
 * @param SGameState* game
 * 			pointeur vers l'état du jeu afin de modifier les differents tableaux
 * @param SMove move
 * 			mouvement à effectuer (mouvement valide, invalide, ou combat)
 */
 // use int Game_CheckPosition(SPos start, EPlayer player, SGameState gamestate)
 // use int Game_CheckMove(SMove move, EPlayer player, SGameState gamestate, int position)
 /*
 typedef struct
{
	SPos start;
	SPos end;
} SMove;

typedef struct
{
	int line;
	int col;
} SPos;
*/
void Game_RazSBox(SBox *box)
{
	box->piece = EPnone;
	box->content = ECnone; 
}



void Game_DoMove(SGameState* game,SMove move, EPlayer *MovingPlayer, EPlayer *Other)
{
	/* @variable : move 
	 *				mouvement tel qu'il est rentré par le joueur, dans le sens de son tableau personnel
	 * @variable : GSMove
	 *				mouvement move inversé en fonction de la couleur du joueur : si il est bleu, on inverse, sinon on prend move
	 * @variable : AdvMove
	 *				mouvement move inversé pour changer le gamestate de l'adversaire du joueur (qui est dans l'autre sens)
	 */
	SMove AdvMove, GSMove;  
	SBox FightResult;
		printf("move.start.ligne = %d\n", move.start.line);
		printf("move.start.colonne = %d\n", move.start.col);
		printf("move.end.ligne = %d\n", move.end.line);
		printf("move.end.colonne = %d\n", move.end.col);
	GSMove = Game_TranslateMove(move, *MovingPlayer, 1); // Si le joueur est bleu, il faut inverser les indices
	AdvMove = Game_TranslateMove(move, *MovingPlayer, 2); // utile quand on va changer le GS des adversaires
		printf("APRES INVERSION===========================================================================================\n");
		printf("move.start.ligne = %d\n", GSMove.start.line);
		printf("move.start.colonne = %d\n", GSMove.start.col);
		printf("move.end.ligne = %d\n", GSMove.end.line);
		printf("move.end.colonne = %d\n", GSMove.end.col);	

	int pos = Game_CheckPosition(GSMove.start, *MovingPlayer, *game);
		printf("pos retourne : %d\n", pos);
	int mvt = Game_CheckMove(GSMove, *MovingPlayer, *game, pos);
		printf("mvt retourne : %d\n", mvt);
	switch(mvt)
	{
		case -1: printf("Mouvement invalide\n");
				 // Game_AddPenalty(player);
				 break; // On ajoute une pénalité au joueur et on sort

		// Deplacement seul
		case 0 : // Changement du gamestate général avec toutes les valeurs de pions : on utilise move inversé pour EPblue
				 game->board[GSMove.end.line][GSMove.end.col] = game->board[GSMove.start.line][GSMove.start.col]; // la case de destination prend le pion valide
				 Game_RazSBox(&(game->board[GSMove.start.line][GSMove.start.col]));
				 printf("gamestate général modifié\n");

				 //Modification du tableau du joueur : on utilise move tel qu'il a été rentré
				 MovingPlayer->Pboard[move.end.line][move.end.col] = MovingPlayer->Pboard[move.start.line][move.start.col];
				 Game_RazSBox(&(MovingPlayer->Pboard[move.start.line][move.start.col]));
				 printf("gamestate joueur modifié\n");

				 //Modification du tableau de l'adversaire : on utilise AdvMove = move inversé pour chaque GS adverse
				 Other->Pboard[AdvMove.end.line][AdvMove.end.col] = Other->Pboard[AdvMove.start.line][AdvMove.start.col];
				 Game_RazSBox(&(Other->Pboard[AdvMove.start.line][AdvMove.start.col]));
				 printf("gamestate adverse modifié\n");

				 break;
		// Combat
		case 1 : if((game->board[move.start.line][move.start.col].piece) == (game->board[move.end.line][move.end.col].piece)) 
				 {
				 	// On MaJ les deux tableaux de pièces éliminées dans le gamestate
				 	game->redOut[(game->board[move.start.line][move.start.col].piece)]++;
				 	game->blueOut[(game->board[move.start.line][move.start.col].piece)]++;
					// On supprime les deux pions du Gamestate général (GSMove)
					Game_RazSBox(&(game->board[GSMove.end.line][GSMove.end.col]));
					// On supprime les deux pions dans le GS du joueur (move)
					Game_RazSBox(&(MovingPlayer->Pboard[move.end.line][move.end.col]));
					// On supprime les deux pions dans le GS de l'adversaire
					Game_RazSBox(&(Other->Pboard[AdvMove.end.line][AdvMove.end.col]));

				 }
				 else
				 {	
				 	// La case de destination prend le resultat du combat 
				 	FightResult = Game_Fight(game->board[GSMove.start.line][GSMove.start.col], game->board[GSMove.end.line][GSMove.end.col]);
				 	
				 	// MaJ des pièces éliminées
				 	switch(FightResult.content)
				 	{
				 		case ECred: // Le gagnant est rouge, on cherche la piece bleue
				 						if(game->board[GSMove.start.line][GSMove.start.col].content == ECblue) // La piece attaquante et perdante est bleue
				 							game->blueOut[game->board[GSMove.start.line][GSMove.start.col].piece] ++; // La piece a éliminer est l'attaquante
				 						else
				 							game->blueOut[game->board[GSMove.end.line][GSMove.end.col].piece] ++; // La piece à éliminer est l'attaquée
				 						break;
				 		case ECblue: // Le gagnant est bleu, on cherche la piece rouge
				 						if (game->board[GSMove.start.line][GSMove.start.col].content == ECred) // L'attaquant est perdant et rouge
				 							game->redOut[game->board[GSMove.start.line][GSMove.start.col].piece]++; // La piece a eliminer est l'attaquante
				 						else
				 							game->redOut[game->board[GSMove.end.line][GSMove.end.col].piece]++; // La piece a eliminer est l'attaquée
				 		default : break;
				 	}

				 	// Gamestate Général
					game->board[GSMove.end.line][GSMove.end.col] = FightResult ;

					// Changement GS joueur
					if(FightResult.content == MovingPlayer->Color) // Si le pion attaquant est gagnant
					{
						MovingPlayer->Pboard[move.end.line][move.end.col] = FightResult; // On garde la valeur de son pion dans le gamestate
					}
					else
					{
						MovingPlayer->Pboard[move.end.line][move.end.col].piece = EPnone; // On affiche seulement la couleur de la piece gagnante dans le GS du joueur
						MovingPlayer->Pboard[move.end.line][move.end.col].content = FightResult.content; 
					}

					// Changement GS adversaire
					if(FightResult.content == Other->Color) // Si le pion attaqué est gagnant
					{
						Other->Pboard[AdvMove.end.line][AdvMove.end.col] = FightResult; // On garde la valeur de son pion dans son gamestate
					}
					else
					{
						Other->Pboard[AdvMove.end.line][AdvMove.end.col].piece = EPnone; // On affiche seulement la couleur de la piece gagnante dans le GS de l'adversaire
						Other->Pboard[AdvMove.end.line][AdvMove.end.col].content = FightResult.content; 
					}
				 }
				 // On efface la case de départ du mouvement
				 Game_RazSBox(&(game->board[GSMove.start.line][GSMove.start.col]));
				 // On supprime le pion de depart dans le GS du joueur
				 Game_RazSBox(&(MovingPlayer->Pboard[move.start.line][move.start.col]));
				 // On supprime les deux pions dans le GS de l'adversaire
				 Game_RazSBox(&(Other->Pboard[AdvMove.start.line][AdvMove.start.col]));				 
				 break; 
	}
}

/* Fonction de calcul du gagnant d'un combat
 * @param : player1 
 *			Contient la couleur et la valeur du pion du joueur attaquant
 * @param : player2
 *			Contient la couleur et la valeur du pion du joueur attaquant
 */
SBox Game_Fight(SBox player1, SBox player2)
{
	EPiece A, B;
	A = player1.piece;
	B = player2.piece; 
	/* Si la pièce visée est le drapeau,
	la pièce attaquante gagne d'office */
	if (B == EPflag)
		return player1;
	/* Si l'attaquant est un espion */
	else if (A == EPspy)
	{
		/* Il gagne seulement si la pièce 
		cible est le maréchal */
		if (B == EPmarshal)
			return player1;
		else
			return player2;
	}
	/* Sinon si l'attaquant est un démineur */
	else if (A == EPminer)
	{
		/* Il gagne si la pièce attaquée est une 
		bombe, un espion ou un éclaireur */
		if (B < EPminer)
			return player1;
		else
			return player2;
	}
	/* Sinon, dans tous les autres cas */
	else
	{
		/* Si la pièce attaquée est une bombe ou si sa 
		puissance est supérieure à l'attaquant, elle gagne */
		if ((B == EPbomb) || (B > A))
			return player2;
		else
			return player1;
	}
}


/* procédure de vérification de la fin du jeu
 * @param : SGameState gamestate
 * 			permet de vérifier les pions présents sur le plateau
 * @param : const EColor color
 *			permet de savoir quel joueur on doit vérifier (peut etre inutile)
 * condition de fin d'un jeu : un joueur n'a plus de drapeau (perdu) ou plus de pieces mobiles
 * par exemple si plus que des bombes + flag en jeu, ou si toutes les pièces mobiles sont entourées par des bombes
 */
void Game_End(SGameState gamestate, const EColor color); 



void Game_EndMatch(); 	// "Voulez-vous rejouer ?" "O/n" si oui, incrementation d'un compteur de jeux 
					  	//pour savoir à la fin du match qui a gagné, sinon, on arrête et on donne le gagnant du match

void Game_AddPenalty(EPlayer player);	// idée : variable statique ? allouées au début du programme et libérées à la fin

/* procédure de recopie des tableaux des joueurs dans le tableau de l'arbitre à l'initialisation
 * @param SGameState gamestate
 * 				Etat du jeu à modifier
 * @param const EColor color
 * 				couleur du joueur pour lequel on passe le tableau
 * @param EPiece boardInit[4][10]
 *				tableau initialisé du joueur 
 * @param SBox* board[10][10] 
 *				gamestate du joueur, avec le tableau orienté vers le bas
 */
void Game_CpyInitGameState(SGameState* gamestate, EPlayer* player, EPiece boardInit[4][10])
{
	int k, l; 
	// k : lignes du tableau 4*10
	// l : colonnes du tab 4*10
    for (k=0; k<4; k++)
    {
        for (l=0; l<10; l++)
        {
        	// Remplissage du gamestate general
            if (player->Color == ECred) // On remplie le bas du gamestate
                    gamestate->board[k][l].piece = boardInit[k][l];
            else if (player->Color == ECblue) // On remplie le haut du gamestate en retournant le tableau de 4*10 de 180°
                    gamestate->board[9-k][9-l].piece = boardInit[k][l];
            // Remplissage du gamestate du joueur
            player->Pboard[k][l].piece =boardInit[k][l];

        }
    }
}

/* initialisation du jeu pour le joueur humain (placement des pieces)
 * à ce moment là on ne verifie pas la validité des mouvements car il ne peut pas acceder au tableau du joueur adverse
 * @param const EColor color 
 * 				couleur du joueur 
 * @param EPiece boardInit[4][10]
 * 				tableau dans lequel le joueur va placer ses pions (on ne peut pas lui donner le gamestate)
 */
void Game_Begin(const EColor color, EPiece boardInit[4][10]);

/* fonction de demande du prochain mouvement au joueur humain
 * @param const SGameState * const gamestate
 * 			etat du jeu pour donner le mouvement à effectuer
 *			const SGameState* :		constance de la valeur pointée
 *			* conste gamestate :	constance de l'adresse contenue dans le pointeur 
 */
SMove Player_NextMove(const SGameState * const gamestate);
// A changer et envoyer un player_gamestate pour le joueur

void game_PvP();	
void game_IAvsP();
void game_IAvsIA();

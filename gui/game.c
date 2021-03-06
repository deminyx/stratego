#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "game.h"
#include "analyze.c"
#include "initlib.h"

// Tous les tableaux sont comptés avec le 0,0 en bas à gauche
// Tous les indices donnés en commentaires sont comptés à partir de 0 

/* fonction de detection du mode de jeu
 * @param char *argv[]
 *			nombre de joueurs humains detectés via argv[1]=j j={0,1,2}
 *			si argv[1]==0 : IA vs IA
 *			si argv[1]==1 : P vs IA
 *			si argv[1]==2 : PVP
 * @param int argc : nombre d'arguments.  par défaut le jeu se lance en humain vs humain
 * @param StructIA : 
 *			pointeur vers les structures des IA (null si pas d'IA)
 * @return value : retourne un gamemode (entier)
 */
SGameMode DetectGameMode(int argc, char* argv[], StructIA *AIfunctions1, StructIA *AIfunctions2)
{
	SGameMode gamemode = ERROR;
	if(argc < 3)
	{
		printf("le nombre d'arguments est incorrect : appel correct : \n ./prog.exe nbCoupsMax NbIA ./lib1.so ./lib2.so \n");
	}
	if(argc > 2)
	{
		switch(*argv[2])
		{
			case '0': if (argc == 3)  // OK
						gamemode = HUMAN_HUMAN;
						break;
			case '1': if(LoadAI(AIfunctions1, argv[3])) // 1 IA
						gamemode = IA_HUMAN;
						break;
			case '2': if((LoadAI(AIfunctions1, argv[3]))&&(LoadAI(AIfunctions2, argv[4]))) // 2 IA
						gamemode = IA_IA;
						break;
			default : break;
		}
	}
		 
	return gamemode;

}

/* fonction qui verifie le placement des pions du joueur lors de l'initialisation de son gamestate (placement)
 * @param EPiece boardInit[4][10]
 *			tableau de placement des pions du joueur au début d'un jeu
 * @return value : 
 *			0 : Mauvais placement
 *			1 : Bon placement
 */
int Game_CheckTab(EPiece boardInit[4][10])
{
	int ref[12] = {6, 1, 8, 5, 4, 4, 4, 3, 2, 1, 1, 1};
	int check[12], i, j;
	for(i=0; i<12; i++) check[i] = 0; // initialisation des compteurs de pieces

	for(i=0; i<4; i++)
	{
		for(j=0; j<10; j++)
		{
			// Cas où le joueur a placé des pieces de la mauvaise couleur ou des pieces qui n'existent pas
			if((boardInit[i][j]<0)||(boardInit[i][j]>11)) return 0;

			check[boardInit[i][j]]++; // on incrémente le compteur de la piece dans le tableau de compteurs
		}
	}
	for(i=0; i<12; i++) 
	{
		if(check[i] != ref[i]) 
			return 0;
	}
	return 1;

}

/* procédure d'initialisation des variables de la structure joueur au début de chaque jeu
 * initialise les deux joueurs en même temps 
 *       Dans le Gamestate des joueurs : 
 *            - Met toutes les cases qui n'appartiennent pas au joueur avec un contenu vide EPnone
 *            - Met les lacs
 *            - Place les couleurs des deux joueurs (couleur du joueur en bas et de l'adversaire en haut)
 * initialise les couleurs des pions dans les plateaux des joueurs mais pas leur valeur
 * @param gameconfig :
 *			configuration du jeu
 * @param nbCoups :
 *			nombre de coups maximal pour un joueur
 */
void Game_InitPlayer(EPlayer* player1, EPlayer* player2, SGameConfig* gameconfig, int nbCoups)
{
	player1->nbPenalty = 0;
	player2->nbPenalty = 0;
	//Maj du nombre de coups
	player2->nbCoups = nbCoups;
	player1->nbCoups = nbCoups; 

	// MaJ du nombre de mouvements en aller retour
	player1->nbMove = 1;
	player2->nbMove = 1;

	// MaJ du numéro du joueur
	player1->playerNo = 1;
	player2->playerNo = 2;
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
		gamestate->redOut[i] = 0; 
		gamestate->blueOut[i] = 0;
	}
	
}

/* fonction pour vérifier si la pièce de départ est bien placée
 * @param : SPos start
 * 			position de départ du mouvement
 * @return value : 
 *			entier pour connaitre la validité de la position
 */
int Game_CheckPosition(SPos start, EPlayer player, SGameState gamestate)
{
	int RETURN = 0; // ERROR
	int i = start.line;
	int	j = start.col;
	EColor _color = gamestate.board[i][j].content; 
	EPiece _piece = gamestate.board[i][j].piece; 

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
				move.start.line = 9-move.start.line;
				move.end.line = 9-move.end.line;
				move.start.col = 9-move.start.col; 
				move.end.col = 9-move.end.col; 
			}
			break;
	case 2: // Inversion pour le gamestate des joueurs
				move.start.line = 9-move.start.line;
				move.end.line = 9-move.end.line;
				move.start.col = 9-move.start.col; 
				move.end.col = 9-move.end.col; 
				break; 
	}
	return move;
}

/* fonction retournant un pointeur vers une copie du gamestate pour l'IA
 * 		orienté dans le sens du joueur
 *		cache les pièces du joueur adverse
 */
SGameState* Game_SwitchGS(SGameState game, EPlayer player)
{
	int i, j;
	SGameState *newGame = malloc(sizeof(SGameState));
	for(i=0; i<10; i++)
	{
		for(j=0; j<10; j++)
		{
			if(game.board[i][j].content != player.Color)
				game.board[i][j].piece = EPnone;
			if(player.Color == ECblue)
				newGame->board[9-i][9-j] = game.board[i][j];
			else
				newGame->board[i][j] = game.board[i][j];
		}
	}
	return newGame;
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
int Game_CheckMove(SMove move, EPlayer *player, SGameState gamestate, int position)
{
	int RETOUR = 0; // par défaut déplacement possible sans attaque
	// on defini des variables au lieu de faire les opérations d'accès à chaque fois
	int StartLine, EndLine, StartCol, EndCol, i;

	// Determination de la couleur de l'adversaire
	EColor Enemy = (player->Color == ECred) ? ECblue : ECred;
	// Determination de variables pour éviter les accès mémoire trop fréquents
		StartLine = move.start.line;
		EndLine = move.end.line;
		StartCol = move.start.col; 
		EndCol = move.end.col; 
	// idem
	unsigned int diffligne, diffcol;
		diffligne = abs(StartLine - EndLine);
		diffcol = abs(StartCol - EndCol);

	// On stocke le mouvement précédent du joueur
	// sera utile dans le calcul des pénalités
	 Game_AddPenalty(player, move);
	 player->previousMove = move; 
	 if(player->nbMove <=3)
	 {
		// verif de la case de destination
			// si il y a un deplacement en diagonale 
			// ou si le joueur essaie de rester sur la mm case
			// ou si le joueur essaie de se deplacer sur un de ses pions
			// ou si le joueur essaie de se deplacer sur un lac
		if ((diffligne > 0 && (diffcol > 0)) || ((StartLine == EndLine)&&(StartCol == EndCol)) || (gamestate.board[EndLine][EndCol].content == player->Color) || (gamestate.board[EndLine][EndCol].content == EClake))
		{
			RETOUR = -1; // deplacement invalide		
		}
			// Traitement des cas où on ne peut pas deplacer le pion 
			switch (position)
			{
				case 0 :  // On choisi d'incrémenter le compteur de pénalités ici plutot que dans checkpos
						RETOUR = -1; // Position de depart invalide
						break;
				case 1 : // cas pion général
						 // Deplacement d'une case seulement
						if (diffligne + diffcol != 1) 
							RETOUR = -1;// Deplacement invalide
						break;
				case 2 : // cas pion scout
						if (diffcol == 0) // deplacement en lignes
						{
							if ((StartLine - EndLine) < 0)
							{
								for(i = StartLine + 1; i<EndLine; i++) 
									if ((gamestate.board[i][StartCol].piece != EPnone)||(gamestate.board[i][StartCol].content != ECnone)) 
										RETOUR = -1; // on ne peut pas déplacer le scout
							}
							else
							{
								for(i = StartLine - 1; i>EndLine; i--) 
									if ((gamestate.board[i][StartCol].piece != EPnone)||(gamestate.board[i][StartCol].content != ECnone))
										RETOUR = -1; // on ne peut pas déplacer le scout	
							}
						}
						if (diffligne == 0) // deplacement en colonnes
						{
							if((StartCol - EndCol) < 0)
							{
								for (i = StartCol + 1; i<EndCol; i++)
									if ((gamestate.board[StartLine][i].piece != EPnone)||(gamestate.board[StartLine][i].content != ECnone))
										RETOUR = -1; // on ne peut pas déplacer le scout	
							}
							else
							{
								for (i = StartCol -1; i>EndCol; i--)
									if ((gamestate.board[StartLine][i].piece != EPnone)||(gamestate.board[StartLine][i].content != ECnone))
										RETOUR = -1; // on ne peut pas déplacer le scout	
							}
						}
						// si on arrive ici on peut deplacer le scout
						break;
			}
			// Traitement du cas où, si le pion est déplaçable, le mouvement mène à un combat
			if ((gamestate.board[EndLine][EndCol].content == Enemy)&&(RETOUR != -1))
				RETOUR = 1;
	}
	else
		RETOUR = -1; // Pour savoir si on a eu trois aller-retours

	if(RETOUR == -1)
		player->nbPenalty++;

return RETOUR;
}



/* procédure pour remettre à zéro une case du tableau
 */
void Game_RazSBox(SBox *box)
{
	box->piece = EPnone;
	box->content = ECnone; 
}


/* procédure pour effectuer le mouvement
 * @param SGameState* game
 * 			pointeur vers l'état du jeu afin de modifier les differents tableaux
 * @param SMove move
 * 			mouvement à effectuer (mouvement valide, invalide, ou combat)
 * @param EPlayer *MovingPlayer 
 *			joueur effectuant le mouvement
 * @param EPlayer *Other
 *			joueur adverse
 * @param StructIA
 *			Structures de l'IA
 */
int Game_DoMove(SGameState* game,SMove move, EPlayer *MovingPlayer, EPlayer *Other, BoardLayout *layout, SGameConfig config, StructIA IA1, StructIA IA2)
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
	unsigned int RETOUR;
	MovingPlayer->nbCoups --;

	GSMove = Game_TranslateMove(move, *MovingPlayer, 1); // Si le joueur est bleu, il faut inverser les indices
	AdvMove = Game_TranslateMove(move, *MovingPlayer, 2); // utile quand on va changer le GS des adversaires
	// Stockage des accès tableaux dans des variables pour optimiser le tps
	int GSMoveEndLine, GSMoveEndCol, GSMoveStartLine, GSMoveStartCol; 
		GSMoveEndLine = GSMove.end.line;
		GSMoveEndCol = GSMove.end.col;
		GSMoveStartLine = GSMove.start.line;
		GSMoveStartCol = GSMove.start.col;
	int MoveEndLine, MoveEndCol, MoveStartLine, MoveStartCol; 
		MoveEndLine = move.end.line;
		MoveEndCol = move.end.col;
		MoveStartLine = move.start.line;
		MoveStartCol = move.start.col;
	int AdvEndLine, AdvEndCol, AdvStartLine, AdvStartCol;  
		AdvEndLine = AdvMove.end.line;
		AdvEndCol = AdvMove.end.col;
		AdvStartLine = AdvMove.start.line;
		AdvStartCol = AdvMove.start.col;
	
	int pos = Game_CheckPosition(GSMove.start, *MovingPlayer, *game);
	int mvt = Game_CheckMove(GSMove, MovingPlayer, *game, pos);
	switch(mvt)
	{
		case -2: RETOUR = -1;
				 break;
		case -1:
				 RETOUR = 0; // Dans le jeu, le joueur passe son tour
				 break; // On ajoute une pénalité au joueur et on sort

		// Deplacement seul
		case 0 : // Changement du gamestate général avec toutes les valeurs de pions : on utilise move inversé pour EPblue
				 game->board[GSMoveEndLine][GSMoveEndCol] = game->board[GSMoveStartLine][GSMoveStartCol]; // la case de destination prend le pion valide
				 Game_RazSBox(&(game->board[GSMoveStartLine][GSMoveStartCol]));
	
				 //Modification du tableau du joueur : on utilise move tel qu'il a été rentré
				 MovingPlayer->Pboard[MoveEndLine][MoveEndCol] = MovingPlayer->Pboard[MoveStartLine][MoveStartCol];
				 Game_RazSBox(&(MovingPlayer->Pboard[MoveStartLine][MoveStartCol]));

				 //Modification du tableau de l'adversaire : on utilise AdvMove = move inversé pour chaque GS adverse
				 Other->Pboard[AdvEndLine][AdvEndCol] = Other->Pboard[AdvStartLine][AdvStartCol];
				 Game_RazSBox(&(Other->Pboard[AdvStartLine][AdvStartCol]));

				 RETOUR = 1; // le mouvement a été effectué
				 break;
		

		// Combat
		case 1 :
		//void (*pfAttackResult)(SPos,EPiece,SPos,EPiece);
				if((config.Mode == IA_HUMAN)||(config.Mode == IA_IA))
				{
					if(MovingPlayer->playerNo == 2) // Si l'IA1 est l'attaquante
					{
						if(MovingPlayer->Color == ECred) // Si elle est rouge
						{
							IA1.AttackResult(move.start, game->board[MoveStartLine][MoveStartCol].piece, move.end, game->board[MoveEndLine][MoveEndCol].piece );
						}
						else // Sinon elle est bleue
						{
							IA1.AttackResult(move.start, game->board[AdvStartLine][AdvStartCol].piece, move.end, game->board[AdvEndLine][AdvEndCol].piece );
							
						}
					}
					else // Si l'IA est celle qui est attaquée
					{
						if(MovingPlayer->Color == ECred) // Si l'attaquant est rouge, son mouvement est dans le bon sens
						{
							IA1.AttackResult(AdvMove.end, game->board[MoveEndLine][MoveEndCol].piece, AdvMove.start, game->board[MoveStartLine][MoveStartCol].piece);
						}
						else // Sinon l'attaquant est bleu et il faut prendre le mouvement dans l'autre sens sur le gamestate
						{
							IA1.AttackResult(AdvMove.end, game->board[AdvEndLine][AdvEndCol].piece, AdvMove.start, game->board[AdvStartLine][AdvStartCol].piece);
						}
					}
						
					if(config.Mode == IA_IA) // Si on a une deuxième IA qui joue contre la premiere 
					{
						if(MovingPlayer->playerNo == 1) // Si l'IA2 est l'attaquante
						{
							if(MovingPlayer->Color == ECred) // Si elle est rouge
								IA2.AttackResult(move.start, game->board[MoveStartLine][MoveStartCol].piece, move.end, game->board[MoveEndLine][MoveEndCol].piece );
							else // Sinon elle est bleue
								IA2.AttackResult(GSMove.start, game->board[GSMoveStartLine][GSMoveStartCol].piece, GSMove.end, game->board[GSMoveEndLine][GSMoveEndCol].piece );
						}
						else // Si l'IA est celle qui est attaquée
						{
							if(MovingPlayer->Color == ECred) // Si l'attaquant est rouge, son mouvement est dans le bon sens
								IA2.AttackResult(move.end, game->board[MoveEndLine][MoveEndCol].piece, move.start, game->board[MoveStartLine][MoveStartCol].piece );
							else // Sinon l'attaquant est bleu et il faut prendre le mouvement dans l'autre sens sur le gamestate
								IA2.AttackResult(GSMove.end, game->board[GSMoveEndLine][GSMoveEndCol].piece, GSMove.start, game->board[GSMoveStartLine][GSMoveStartCol].piece );
						}
					}						
				}
				 Display_fight(move, *game, MovingPlayer, layout);
				 if((game->board[GSMoveStartLine][GSMoveStartCol].piece) == (game->board[GSMoveEndLine][GSMoveEndCol].piece)) 
				 {
				 	// On MaJ les deux tableaux de pièces éliminées dans le gamestate
				 	game->redOut[(game->board[MoveStartLine][MoveStartCol].piece)]++;
				 	game->blueOut[(game->board[MoveStartLine][MoveStartCol].piece)]++;
					// On supprime les deux pions du Gamestate général (GSMove)
					Game_RazSBox(&(game->board[GSMoveEndLine][GSMoveEndCol]));
					// On supprime les deux pions dans le GS du joueur (move)
					Game_RazSBox(&(MovingPlayer->Pboard[MoveEndLine][MoveEndCol]));
					// On supprime les deux pions dans le GS de l'adversaire
					Game_RazSBox(&(Other->Pboard[AdvEndLine][AdvEndCol]));

				 }
				 else
				 {	
				 	// La case de destination prend le resultat du combat 
				 	FightResult = Game_Fight(game->board[GSMoveStartLine][GSMoveStartCol], game->board[GSMoveEndLine][GSMoveEndCol]);
				 	// MaJ des pièces éliminées
				 	switch(FightResult.content)
				 	{
				 		case ECred: // Le gagnant est rouge, on cherche la piece bleue
				 						if(game->board[GSMoveStartLine][GSMoveStartCol].content == ECblue) // La piece attaquante et perdante est bleue
				 							game->blueOut[game->board[GSMoveStartLine][GSMoveStartCol].piece] ++; // La piece a éliminer est l'attaquante
				 						else
				 							game->blueOut[game->board[GSMoveEndLine][GSMoveEndCol].piece] ++; // La piece à éliminer est l'attaquée
				 						break;
				 		case ECblue: // Le gagnant est bleu, on cherche la piece rouge
				 						if (game->board[GSMoveStartLine][GSMoveStartCol].content == ECred) // L'attaquant est perdant et rouge
				 							game->redOut[game->board[GSMoveStartLine][GSMoveStartCol].piece]++; // La piece a eliminer est l'attaquante
				 						else
				 							game->redOut[game->board[GSMoveEndLine][GSMoveEndCol].piece]++; // La piece a eliminer est l'attaquée
				 		default : break;
				 	}

				 	// Gamestate Général
					game->board[GSMoveEndLine][GSMoveEndCol] = FightResult ;

					// Changement GS joueur
					if(FightResult.content == MovingPlayer->Color) // Si le pion attaquant est gagnant
					{
						MovingPlayer->Pboard[MoveEndLine][MoveEndCol] = FightResult; // On garde la valeur de son pion dans le gamestate
					}
					else
					{
						MovingPlayer->Pboard[MoveEndLine][MoveEndCol].piece = EPnone; // On affiche seulement la couleur de la piece gagnante dans le GS du joueur
						MovingPlayer->Pboard[MoveEndLine][MoveEndCol].content = FightResult.content; 
					}

					// Changement GS adversaire
					if(FightResult.content == Other->Color) // Si le pion attaqué est gagnant
					{
						Other->Pboard[AdvEndLine][AdvEndCol] = FightResult; // On garde la valeur de son pion dans son gamestate
					}
					else
					{
						Other->Pboard[AdvEndLine][AdvEndCol].piece = EPnone; // On affiche seulement la couleur de la piece gagnante dans le GS de l'adversaire
						Other->Pboard[AdvEndLine][AdvEndCol].content = FightResult.content; 
					}
				 }
				 // On efface la case de départ du mouvement
				 Game_RazSBox(&(game->board[GSMoveStartLine][GSMoveStartCol]));
				 // On supprime le pion de depart dans le GS du joueur
				 Game_RazSBox(&(MovingPlayer->Pboard[MoveStartLine][MoveStartCol]));
				 // On supprime les deux pions dans le GS de l'adversaire
				 Game_RazSBox(&(Other->Pboard[AdvStartLine][AdvStartCol]));		
				 RETOUR = 1; // le mouvement a été effectué		 
				 break; 
	}
	return RETOUR;
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



/* initialisation d'un jeu pour deux joueurs (IA ou HUMAIN)
 */
void Game_Begin(EPlayer *player1, EPlayer *player2, SGameState *game, SGameConfig *gameconfig, int nbCoups)
{
	Game_InitGameState(game);
	Game_InitPlayer(player1, player2, gameconfig, nbCoups);
	gameconfig->nbJeux ++; // on a démarré un jeu de plus
}



/* procédure de vérification de la fin du Match
 */
int Game_EndMatch(EPlayer player1, EPlayer player2, SGameConfig config, SGameState gamestate, int nbCoups)
{
	/* Si le joueur 1 a gagné plus de fois ou a le mm nombre de victoires que le joueur 2
	 * 		si ils ont gagné le mm nombre de fois on renvoie null
	 *		sinon on renvoie le joueur1
	 * Sinon on renvoie le joueur 2
	 */
	unsigned int FINI = 0;
	if((config.nbJeux == 2)&&((player1.winnings + player2.winnings)==1))// On a joué tous les jeux
	{
		FINI = (player1.winnings >= player2.winnings)? ((player1.winnings == player2.winnings)? 0 : 1): 2; 
	}
	return FINI; 
}




/* fonction de vérification de fin d'un jeu
 * @return value : 
 *		0 : Pas de gagnant (le jeu a toujours lieu)
 *		1 : Le joueur 1 a gagné le jeu
 *		2 : Le joueur 2 a gagné le jeu
 */
int Game_GotWinner(EPlayer player1, EPlayer player2, SGameState gamestate, int nbCoups)
{
	int GAGNANT = 0; // par défaut pas de gagnant

	// On ne regarde le gagnant que si un coup a déjà été joué par un des joueurs
	if((player1.nbCoups != nbCoups)||(player2.nbCoups != nbCoups))
	{
		// On regarde si le drapeau d'un joueur a été éliminé
		if(player1.Color == ECred) // joueur1 rouge, joueur2 bleu
		{
			if((gamestate.redOut[11] == 1)||(player1.nbPenalty == 3)||(player1.nbCoups == 0)||(analyzeBoard(player1)==0)) // le drapeau rouge est éliminé, player1 a perdu
				GAGNANT = 2; // joueur2 gagne
			else if((gamestate.blueOut[11] == 1)||(player2.nbPenalty == 3)||(player2.nbCoups == 0)||(analyzeBoard(player2)==0))// le drapeau bleu est éliminé, le player1 a perdu
					GAGNANT = 1; // joueur1 gagne

		}
		else // le joueur 1 bleu, joueur2 rouge
		{
			if((gamestate.redOut[11] == 1)||(player2.nbPenalty == 3)||(player2.nbCoups == 0)||(analyzeBoard(player2)==0)) // le drapeau rouge est éliminé, player1 a gagné
				GAGNANT = 1; 
			else if((gamestate.blueOut[11] == 1)||(player1.nbPenalty == 3)||(player1.nbCoups == 0)||(analyzeBoard(player1)==0)) // le drapeau bleu est éliminé, le player1 a perdu
					GAGNANT = 2; 
		}
	}
	return GAGNANT; 
}



/* procédure d'initialisation d'un jeu
 */
void Game_Start(EPlayer *player1, EPlayer *player2, SGameConfig *config)
{
	config->nbJeux = 0;
	player1->winnings = 0;
	player2->winnings = 0; 
}



/* procédure de vérification du mouvement d'aller retour répétitif
 * Si le joueur a manqué son tour à cause d'un quatrième retour, 
 * au tour suivant il peu effectuer le mouvement qu'il voulait faire
 */
void Game_AddPenalty(EPlayer *player, SMove nextmove)	// idée : variable statique ? allouées au début du programme et libérées à la fin
{

	if((player->previousMove.start.line == nextmove.end.line)&&(player->previousMove.end.line == nextmove.end.line)
		&&(player->previousMove.start.col == nextmove.end.col)&&(player->previousMove.end.col == nextmove.start.col))
	{
		if(player->nbMove == 4)
			player->nbMove = 1;
		else
			player->nbMove ++;
	}
	else
		player->nbMove = 1;
}



/* procédure de recopie des tableaux des joueurs dans le tableau de l'arbitre à l'initialisation
 * @param SGameState gamestate
 * 				Etat du jeu à modifier
 * @param EPiece boardInit[4][10]
 *				tableau initialisé du joueur 
 */
void Game_CpyInitGameState(SGameState* gamestate, EPlayer* player, EPiece boardInit[4][10])
{
	if(Game_CheckTab(boardInit))
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
}


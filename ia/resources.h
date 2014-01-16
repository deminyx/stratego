#ifndef RESOURCES_H
#define RESOURCES_H

#include <stdbool.h>
#include "../structure.h"

extern Strategy m_strategy;
extern EColor m_color, m_enemyColor;
extern InfoPiece m_board[10][10];
extern SMove m_movements[172]; 
extern SMove m_decidedMove; 
extern int m_nbMove; 
extern int m_caution; 
extern int m_nbRoundTrips; 
extern SPos m_armyPos, m_enemyPos; 
extern EPiece m_armyPiece, m_enemyPiece; 
extern bool m_myMove; 
extern bool m_hisMove;

#endif

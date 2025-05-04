#include "KitsuneEngine/attacks/attacks.h"

#include "KitsuneEngine/core/board.h"

bool Attacks::IsInCheck( const Board &board ) {
	return IsSquareAttacked( board, board.GetStmKingSquare() );
}

bool Attacks::IsSquareAttacked( const Board &board, const Square square ) {
	return IsSquareAttackedWithOccupancy( board, square, board.GetOccupancy() );
}

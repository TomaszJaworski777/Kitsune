#include "KitsuneEngine/core/board.h"

bool Board::IsInsufficientMaterial() const {
	const Bitboard bishops = GetPieceMask( BISHOP );
	return m_Phase <= 2 && !GetPieceMask( PAWN ) && ( m_Phase != 2 || (
			(bishops & GetOccupancy( WHITE )) != bishops && (bishops & GetOccupancy( BLACK )) != bishops &&
			( ( bishops & Bitboard(0x55AA55AA55AA55AA) ) == bishops || ( bishops & 0xAA55AA55AA55AA55 ) == bishops)
		) );
}

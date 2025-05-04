#include "KitsuneEngine//attacks/pin_mask.h"

#include "KitsuneEngine/attacks/attacks.h"
#include "KitsuneEngine/attacks/rays.h"
#include "KitsuneEngine/core/board.h"

PinMask::PinMask( const Board &board ) {
	const auto kingSquare = board.GetStmKingSquare();
	const auto stm = board.GetSideToMove();
	const auto nstm = ~stm;
	const auto defenderOccupancy = board.GetOccupancy( stm );
	const auto attackerOccupancy = board.GetOccupancy( nstm );

	const auto queens = board.GetPieceMask( QUEEN, nstm );

	auto result = Bitboard::EMPTY;

	auto potentialPinners = Attacks::GetBishopAttacks( kingSquare, attackerOccupancy ) & (
		                        board.GetPieceMask( BISHOP, nstm ) | queens );

	potentialPinners.Map( [defenderOccupancy, kingSquare, &result]( const Square square ) {
		if ( const auto ray = Rays::GetRay( kingSquare, square ); ( ray & defenderOccupancy ).OnlyOneBit() ) {
			result |= ray;
		}
	} );

	m_DiagonalMask = result;

	result = Bitboard::EMPTY;

	potentialPinners = Attacks::GetRookAttacks( kingSquare, attackerOccupancy ) & (
		                   board.GetPieceMask( ROOK, nstm ) | queens );

	potentialPinners.Map( [defenderOccupancy, kingSquare, &result]( const Square square ) {
		if ( const auto ray = Rays::GetRay( kingSquare, square ); ( ray & defenderOccupancy ).OnlyOneBit() ) {
			result |= ray;
		}
	} );

	m_DiagonalMask = result;
}

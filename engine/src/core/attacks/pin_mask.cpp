#include "KitsuneEngine/core/attacks/pin_mask.h"

#include "KitsuneEngine/core/attacks/attacks.h"
#include "KitsuneEngine/core/attacks/rays_arrays.h"
#include "KitsuneEngine/core/board.h"

PinMask::PinMask( const Board &board, const SideToMove defenderSide ) {
	const auto kingSquare = board.GetKingSquare(defenderSide);
	const auto attackerSide = ~defenderSide;
	const auto defenderOccupancy = board.GetOccupancy( defenderSide );
	const auto attackerOccupancy = board.GetOccupancy( attackerSide );

	const auto queens = board.GetPieceMask( QUEEN, attackerSide );

	auto result = Bitboard::EMPTY;

	auto potentialPinners = Attacks::GetBishopAttacks( kingSquare, attackerOccupancy ) & (
		                        board.GetPieceMask( BISHOP, attackerSide ) | queens );

	potentialPinners.Map( [defenderOccupancy, kingSquare, &result]( const Square square ) {
		if ( const auto ray = Rays::GetRay( kingSquare, square ); ( ray & defenderOccupancy ).OnlyOneBit() ) {
			result |= ray;
		}
	} );

	m_DiagonalMask = result;

	result = Bitboard::EMPTY;

	potentialPinners = Attacks::GetRookAttacks( kingSquare, attackerOccupancy ) & (
		                   board.GetPieceMask( ROOK, attackerSide ) | queens );

	potentialPinners.Map( [defenderOccupancy, kingSquare, &result]( const Square square ) {
		if ( const auto ray = Rays::GetRay( kingSquare, square ); ( ray & defenderOccupancy ).OnlyOneBit() ) {
			result |= ray;
		}
	} );

	m_OrthographicMask = result;
}

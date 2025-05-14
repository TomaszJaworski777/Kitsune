#include "KitsuneEngine/core/attacks/pin_mask.h"

#include "KitsuneEngine/core/attacks/attacks.h"
#include "KitsuneEngine/core/attacks/rays.h"
#include "KitsuneEngine/core/board.h"

PinMask::PinMask( const Board &board, const SideToMove defenderSide ) {
	const auto kingSquare = board.GetKingSquare( defenderSide );
	const auto attackerSide = ~defenderSide;
	const auto generalOccupancy = board.GetOccupancy();
	const auto defenderOccupancy = board.GetOccupancy( defenderSide );
	const auto attackerOccupancy = board.GetOccupancy( attackerSide );

	const auto queens = board.GetPieceMask( QUEEN );
	const auto diags = ( board.GetPieceMask( BISHOP ) | queens ) & attackerOccupancy;
	const auto orthos = ( board.GetPieceMask( ROOK ) | queens ) & attackerOccupancy;

	auto result = Bitboard::EMPTY;

	auto potentialPinners = Attacks::GetBishopAttacks( kingSquare, generalOccupancy );
	potentialPinners = potentialPinners ^ Attacks::GetBishopAttacks(
		                   kingSquare, generalOccupancy ^ potentialPinners & defenderOccupancy );
	potentialPinners &= diags;

	potentialPinners.Map( [kingSquare, &result]( const Square square ) {
		result |= Rays::GetRay( kingSquare, square );
	} );

	m_DiagonalMask = result;

	result = Bitboard::EMPTY;

	potentialPinners = Attacks::GetRookAttacks( kingSquare, generalOccupancy );
	potentialPinners = potentialPinners ^ Attacks::GetRookAttacks(
		                   kingSquare, generalOccupancy ^ potentialPinners & defenderOccupancy );
	potentialPinners &= orthos;

	potentialPinners.Map( [kingSquare, &result]( const Square square ) {
		result |= Rays::GetRay( kingSquare, square );
	} );

	m_OrthographicMask = result;
}

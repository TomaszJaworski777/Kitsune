#include "KitsuneEngine/core/attacks/attacks.h"

#include "KitsuneEngine/core/board.h"

bool Attacks::IsInCheck( const Board &board ) {
	return IsSquareAttacked( board, board.GetKingSquare( board.GetSideToMove() ), board.GetSideToMove() );
}

bool Attacks::IsSquareAttacked( const Board &board, const Square square, const SideToMove defenderSide ) {
	return IsSquareAttackedWithOccupancy( board, square, defenderSide, board.GetOccupancy() );
}

bool Attacks::IsSquareAttackedWithOccupancy( const Board &board, const Square square, const SideToMove defenderSide,
                                             const Bitboard occupancy ) {
	return AllAttackersToSquare( board, square, defenderSide, occupancy );
}

Bitboard Attacks::GenerateCheckersMask( const Board &board ) {
	return AllAttackersToSquare( board, board.GetKingSquare( board.GetSideToMove() ), board.GetSideToMove(), board.GetOccupancy() );
}

Bitboard Attacks::AllAttackersToSquare( const Board &board, const Square square, const SideToMove defenderSide,
                                        const Bitboard occupancy ) {
	const auto queens = board.GetPieceMask( QUEEN );
	return ( ( GetKnightAttacks( square ) & board.GetPieceMask( KNIGHT ) ) | (
		         GetKingAttacks( square ) & board.GetPieceMask( KING ) ) | (
		         GetPawnAttacks( square, defenderSide ) & board.GetPieceMask( PAWN ) ) | (
		         GetRookAttacks( square, occupancy ) & ( board.GetPieceMask( ROOK ) | queens ) ) | (
		         GetBishopAttacks( square, occupancy ) & ( board.GetPieceMask( BISHOP ) | queens ) ) ) & board.
	       GetOccupancy( ~defenderSide );
}

Bitboard Attacks::GenerateAttackMap( const Board &board, const SideToMove defenderSide ) {
	auto result = Bitboard::EMPTY;

	const Square kingSquare = board.GetKingSquare( defenderSide );
	const Bitboard occupancy = board.GetOccupancy() ^ Bitboard( kingSquare );

	const SideToMove attackerSide = ~defenderSide;

	const Bitboard attackers = board.GetOccupancy( attackerSide );
	const Bitboard queens = board.GetPieceMask( QUEEN );

	( attackers & ( board.GetPieceMask( ROOK ) | queens ) ).Map( [&result, occupancy]( const Square square ) {
		result |= GetRookAttacks( square, occupancy );
	} );

	( attackers & ( board.GetPieceMask( BISHOP ) | queens ) ).Map( [&result, occupancy]( const Square square ) {
		result |= GetBishopAttacks( square, occupancy );
	} );

	( attackers & board.GetPieceMask( KING ) ).Map( [&result]( const Square square ) {
		result |= GetKingAttacks( square );
	} );

	( attackers & board.GetPieceMask( KNIGHT ) ).Map( [&result]( const Square square ) {
		result |= GetKnightAttacks( square );
	} );

	( attackers & board.GetPieceMask( PAWN ) ).Map( [&result, attackerSide]( const Square square ) {
		result |= GetPawnAttacks( square, attackerSide );
	} );

	return result;
}

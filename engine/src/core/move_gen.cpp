#include "KitsuneEngine/core/move_gen.h"

#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/attacks/attacks.h"
#include "KitsuneEngine/core/attacks/pin_mask.h"

MoveGenerator::MoveGenerator( const Board &board ) : m_Board( board ), m_Side( board.GetSideToMove() ),
                                                     m_OppositeSide( ~m_Side ),
                                                     m_KingSquare( board.GetKingSquare( m_Side ) ),
                                                     m_PinMask( m_Board, m_Side ),
                                                     m_AttackMap( Attacks::GenerateAttackMap( m_Board, m_Side ) ),
                                                     m_Checkers( m_AttackMap.GetBit( m_KingSquare )
	                                                                 ? Attacks::GenerateCheckersMask( m_Board )
	                                                                 : Bitboard( Bitboard::EMPTY ) ),
                                                     m_KingMoveMap(
	                                                     Attacks::GetKingAttacks( m_KingSquare ) & ~m_AttackMap ) {
}

Move* MoveGenerator::GetCastleMoves( Move *moves ) const {
	if ( m_Side == WHITE ) {
		if ( m_Board.CanCastle( CASTLE_WHITE_KING ) && !( 0x60 & m_AttackMap ) && !( 0x60 & m_Board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 4, 6, KING_SIDE_CASTLE_FLAG );
		}
		if ( m_Board.CanCastle( CASTLE_WHITE_QUEEN ) && !( 0xC & m_AttackMap ) && !( 0xE & m_Board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 4, 2, QUEEN_SIDE_CASTLE_FLAG );
		}
	} else {
		if ( m_Board.CanCastle( CASTLE_BLACK_KING ) && !( 0x6000000000000000 & m_AttackMap ) && !(
			     0x6000000000000000 & m_Board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 60, 62, KING_SIDE_CASTLE_FLAG );
		}
		if ( m_Board.CanCastle( CASTLE_BLACK_QUEEN ) && !( 0xC00000000000000 & m_AttackMap ) && !(
			     0xE00000000000000 & m_Board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 60, 58, QUEEN_SIDE_CASTLE_FLAG );
		}
	}

	return moves;
}

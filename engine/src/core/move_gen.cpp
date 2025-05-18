#include "KitsuneEngine/core/move_gen.h"

#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/attacks/attacks.h"
#include "KitsuneEngine/core/attacks/pin_mask.h"

MoveGenerator::MoveGenerator( const Board &board, const CastleMask &castleMask )
	: m_Board( board ), m_CastleMask( castleMask ), m_KingSquare( board.GetKingSquare( m_Board.GetSideToMove() ) ),
	  m_PinMask( m_Board, m_Board.GetSideToMove() ),
	  m_AttackMap( Attacks::GenerateAttackMap( m_Board, m_Board.GetSideToMove() ) ),
	  m_Checkers( m_AttackMap.GetBit( m_KingSquare ) ? Attacks::GenerateCheckersMask( m_Board ) : Bitboard( Bitboard::EMPTY ) ),
	  m_KingMoveMap( Attacks::GetKingAttacks( m_KingSquare ) & ~m_AttackMap ) {
}

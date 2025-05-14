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

Move* MoveGenerator::GetQuietKingMoves( Move *moves, const Bitboard moveMap ) const {
	moveMap.Map( [&moves, this]( const Square square ) {
		*( moves++ ) = Move( m_KingSquare, square, QUIET_MOVE_FLAG );
	} );

	return moves;
}

Move* MoveGenerator::GetNoisyKingMoves( Move *moves, const Bitboard moveMap ) const {
	moveMap.Map( [&moves, this]( const Square square ) {
		*( moves++ ) = Move( m_KingSquare, square, CAPTURE_FLAG );
	} );

	return moves;
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

Move* MoveGenerator::GetQuietPawnMoves( Move *moves, const Bitboard pushMap, const Bitboard diagPins,
                                        const Bitboard orthoPins ) const {
	Bitboard verticalPin = orthoPins & ( orthoPins << 8 );
	verticalPin |= orthoPins >> 8;
	const Bitboard promotionRank = m_Side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
	const Bitboard doublePushRank = m_Side == WHITE ? Bitboard::RANK_2 : Bitboard::RANK_7;
	const Bitboard pawnPool = m_Board.GetPieceMask( PAWN, m_Side ) & ~diagPins & ~promotionRank;
	const Bitboard pinnedPawns = pawnPool & verticalPin;
	const Bitboard movablePawns = ( pawnPool & ~orthoPins ) | pinnedPawns;
	const Bitboard doublePushPawns = movablePawns & doublePushRank;

	const Bitboard singlePushMap = m_Side == WHITE ? pushMap >> 8 : pushMap << 8;
	const Bitboard doublePushMap = m_Side == WHITE ? pushMap >> 16 : pushMap << 16;
	const Bitboard singlePushEmptyMap = m_Side == WHITE ? ~m_Board.GetOccupancy() >> 8 : ~m_Board.GetOccupancy() << 8;

	Bitboard pawns = movablePawns & singlePushMap;
	Bitboard targets = m_Side == WHITE ? pawns << 8 : pawns >> 8;
	auto a = Bitboard( pawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), QUIET_MOVE_FLAG );
	}

	pawns = doublePushPawns & singlePushEmptyMap & doublePushMap;
	targets = m_Side == WHITE ? pawns << 16 : pawns >> 16;
	a = Bitboard( pawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), DOUBLE_PUSH_FLAG );
	}

	return moves;
}

Move* MoveGenerator::GetNoisyPawnMoves( Move *moves, const Bitboard pushMap, const Bitboard captureMap, const Bitboard diagPins,
                                        const Bitboard orthoPins ) const {
	const Bitboard promotionRank = m_Side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
	const Bitboard pawns = m_Board.GetPieceMask( PAWN, m_Side );
	const Bitboard attackPawns = pawns & ~orthoPins;

	uint8_t shift = m_Side == WHITE ? 7 : 9;
	Bitboard leftPin = diagPins & ( diagPins << shift );
	leftPin |= leftPin >> shift;

	shift = m_Side == WHITE ? 9 : 7;
	Bitboard rightPin = diagPins & ( diagPins << shift );
	rightPin |= rightPin >> shift;

	const Bitboard leftShiftedCaptureMap = m_Side == WHITE ? captureMap >> 7 : captureMap << 9;
	const Bitboard rightShiftedCaptureMap = m_Side == WHITE ? captureMap >> 9 : captureMap << 7;

	const Bitboard leftAttackPawns = ( ( attackPawns & ~diagPins ) | ( attackPawns & leftPin ) ) & ~Bitboard::FILE_A;
	const Bitboard rightAttackPawns = ( ( attackPawns & ~diagPins ) | ( attackPawns & rightPin ) ) & ~Bitboard::FILE_H;

	const Bitboard leftPromotionPawns = leftAttackPawns & promotionRank;
	const Bitboard rightPromotionPawns = rightAttackPawns & promotionRank;

	Bitboard capturePawns = leftAttackPawns & ~leftPromotionPawns & leftShiftedCaptureMap;
	Bitboard targets = m_Side == WHITE ? capturePawns << 7 : capturePawns >> 9;
	auto a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), CAPTURE_FLAG );
	}

	capturePawns = rightAttackPawns & ~rightPromotionPawns & rightShiftedCaptureMap;
	targets = m_Side == WHITE ? capturePawns << 9 : capturePawns >> 7;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), CAPTURE_FLAG );
	}

	capturePawns = leftPromotionPawns & leftShiftedCaptureMap;
	targets = m_Side == WHITE ? capturePawns << 7 : capturePawns >> 9;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_CAPTURE_FLAG );
	}

	capturePawns = rightPromotionPawns & rightShiftedCaptureMap;
	targets = m_Side == WHITE ? capturePawns << 9 : capturePawns >> 7;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_CAPTURE_FLAG );
	}

	Bitboard promotionPawns = pawns & promotionRank & ~diagPins;
	targets = ( m_Side == WHITE ? promotionPawns << 8 : promotionPawns >> 8 ) & pushMap;
	promotionPawns = m_Side == WHITE ? targets >> 8 : targets << 8;
	a = Bitboard( promotionPawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_FLAG );
	}

	const Square enPassantSquare = m_Board.GetEnPassantSquare();
	if ( enPassantSquare == NULL_SQUARE )
		return moves;

	const Bitboard enPassantPawns = Attacks::GetPawnAttacks( enPassantSquare, m_OppositeSide ) & attackPawns;
	enPassantPawns.Map( [&moves, enPassantSquare, this]( const Square square ) {
		Board temp = m_Board;
		const auto move = Move( square, enPassantSquare, EN_PASSANT_FLAG );
		temp.MakeMove( move );

		if ( !Attacks::IsSquareAttacked( temp, m_KingSquare, m_Side ) ) {
			*( moves++ ) = move;
		}
	} );

	return moves;
}

Move* MoveGenerator::GetPieceMoves( Move *moves, const PieceType piece, const Bitboard moveMap,
                                    const Bitboard diagPins, const Bitboard orthoPins, const MoveFlag flag ) const {
	Bitboard pieces = Bitboard::EMPTY;
	switch ( piece ) {
		case KNIGHT:
			pieces = m_Board.GetPieceMask( KNIGHT, m_Side ) & ~diagPins & ~orthoPins;
			break;
		case BISHOP:
			pieces = ( m_Board.GetPieceMask( BISHOP, m_Side ) | m_Board.GetPieceMask( QUEEN, m_Side ) ) & ~orthoPins;
			break;
		case ROOK:
			pieces = ( m_Board.GetPieceMask( ROOK, m_Side ) | m_Board.GetPieceMask( QUEEN, m_Side ) ) & ~diagPins;
			break;
		default: break;
	}

	Bitboard pinnedPieces = Bitboard::EMPTY;
	switch ( piece ) {
		case BISHOP:
			pinnedPieces = pieces & diagPins;
			break;
		case ROOK:
			pinnedPieces = pieces & orthoPins;
			break;
		default: break;
	}

	const Bitboard nonPinnedPieces = pieces & ~pinnedPieces;

	nonPinnedPieces.Map( [&moves, piece, moveMap, flag, this]( const Square fromSquare ) {
		Bitboard attacks = Bitboard::EMPTY;
		switch ( piece ) {
			case KNIGHT: attacks = Attacks::GetKnightAttacks( fromSquare );
				break;
			case BISHOP: attacks = Attacks::GetBishopAttacks( fromSquare, m_Board.GetOccupancy() );
				break;
			case ROOK: attacks = Attacks::GetRookAttacks( fromSquare, m_Board.GetOccupancy() );
				break;
			default: break;
		}

		( attacks & moveMap ).Map( [&moves, fromSquare, flag]( const Square toSquare ) {
			*( moves++ ) = Move( fromSquare, toSquare, flag );
		} );
	} );

	pinnedPieces.Map( [&moves, piece, moveMap, diagPins, orthoPins, flag, this]( const Square fromSquare ) {
		Bitboard attacks = Bitboard::EMPTY;
		switch ( piece ) {
			case BISHOP: attacks = Attacks::GetBishopAttacks( fromSquare, m_Board.GetOccupancy() ) & diagPins;
				break;
			case ROOK: attacks = Attacks::GetRookAttacks( fromSquare, m_Board.GetOccupancy() ) & orthoPins;
				break;
			default: break;
		}

		( attacks & moveMap ).Map( [&moves, fromSquare, flag]( const Square toSquare ) {
			*( moves++ ) = Move( fromSquare, toSquare, flag );
		} );
	} );

	return moves;
}

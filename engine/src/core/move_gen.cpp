#include "KitsuneEngine/core/move_gen.h"

#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/attacks/attacks.h"
#include "KitsuneEngine/core/attacks/pin_mask.h"
#include "KitsuneEngine/core/attacks/rays.h"

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

Move* GetQuietKingMoves( Move *moves, Bitboard moveMap, Square kingSquare );

Move* GetNoisyKingMoves( Move *moves, Bitboard moveMap, Square kingSquare );

Move* GetCastleMoves( Move *moves, const Board &board, Bitboard attackMap, SideToMove sideToMove );

Move* GetQuietPawnMoves( Move *moves, const Board &board, Bitboard pushMap, Bitboard diagPins, Bitboard orthoPins,
                         SideToMove side );

Move* GetNoisyPawnMoves( Move *moves, const Board &board, Bitboard pushMap, Bitboard captureMap, Bitboard diagPins,
                         Bitboard orthoPins, SideToMove side, SideToMove oppositeSide, Square kingSquare );

Move* GetPieceMoves( Move *moves, const Board &board, PieceType piece, Bitboard moveMap, Bitboard diagPins,
                     Bitboard orthoPins, MoveFlag flag, SideToMove side );

uint8_t MoveGenerator::GenerateQuietMoves( Move *moves ) const {
	const Move *start = moves;

	const auto diagPins = m_PinMask.GetDiagonalMask();
	const auto orthoPins = m_PinMask.GetOrthographicMask();

	moves = GetQuietKingMoves( moves, m_KingMoveMap & ~m_Board.GetOccupancy(), m_KingSquare );

	if ( !m_Checkers ) {
		moves = GetCastleMoves( moves, m_Board, m_AttackMap, m_Side );

		const auto pushMap = ~m_Board.GetOccupancy();

		moves = GetQuietPawnMoves( moves, m_Board, pushMap, diagPins, orthoPins, m_Side );
		moves = GetPieceMoves( moves, m_Board, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
	} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
		const auto checker = m_Checkers.Ls1bSquare();
		const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

		moves = GetQuietPawnMoves( moves, m_Board, pushMap, diagPins, orthoPins, m_Side );
		moves = GetPieceMoves( moves, m_Board, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
	}

	return static_cast<uint8_t>(moves - start);
}

uint8_t MoveGenerator::GenerateNoisyMoves( Move *moves ) const {
	const Move *start = moves;

	const auto diagPins = m_PinMask.GetDiagonalMask();
	const auto orthoPins = m_PinMask.GetOrthographicMask();
	const auto captureMap = m_Board.GetOccupancy( m_OppositeSide );

	moves = GetNoisyKingMoves( moves, m_KingMoveMap & captureMap, m_KingSquare );

	if ( !m_Checkers ) {
		const auto pushMap = ~m_Board.GetOccupancy();

		moves = GetNoisyPawnMoves( moves, m_Board, pushMap, captureMap, diagPins, orthoPins, m_Side, m_OppositeSide,
		                           m_KingSquare );
		moves = GetPieceMoves( moves, m_Board, KNIGHT, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, BISHOP, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, ROOK, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
	} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
		const auto checker = m_Checkers.Ls1bSquare();
		const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

		moves = GetNoisyPawnMoves( moves, m_Board, pushMap, m_Checkers, diagPins, orthoPins, m_Side, m_OppositeSide,
		                           m_KingSquare );
		moves = GetPieceMoves( moves, m_Board, KNIGHT, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, BISHOP, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
		moves = GetPieceMoves( moves, m_Board, ROOK, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
	}

	return static_cast<uint8_t>(moves - start);
}

Move* GetQuietKingMoves( Move *moves, const Bitboard moveMap, const Square kingSquare ) {
	moveMap.Map( [&moves, kingSquare]( const Square square ) {
		*( moves++ ) = Move( kingSquare, square, QUIET_MOVE_FLAG );
	} );

	return moves;
}

Move* GetNoisyKingMoves( Move *moves, const Bitboard moveMap, const Square kingSquare ) {
	moveMap.Map( [&moves, kingSquare]( const Square square ) {
		*( moves++ ) = Move( kingSquare, square, CAPTURE_FLAG );
	} );

	return moves;
}

Move* GetCastleMoves( Move *moves, const Board &board, const Bitboard attackMap, const SideToMove sideToMove ) {
	if ( sideToMove == WHITE ) {
		if ( board.CanCastle( CASTLE_WHITE_KING ) && !( 0x60 & attackMap ) && !( 0x60 & board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 4, 6, KING_SIDE_CASTLE_FLAG );
		}
		if ( board.CanCastle( CASTLE_WHITE_QUEEN ) && !( 0xC & attackMap ) && !( 0xE & board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 4, 2, QUEEN_SIDE_CASTLE_FLAG );
		}
	} else {
		if ( board.CanCastle( CASTLE_BLACK_KING ) && !( 0x6000000000000000 & attackMap ) && !(
			     0x6000000000000000 & board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 60, 62, KING_SIDE_CASTLE_FLAG );
		}
		if ( board.CanCastle( CASTLE_BLACK_QUEEN ) && !( 0xC00000000000000 & attackMap ) && !(
			     0xE00000000000000 & board.GetOccupancy() ) ) {
			*( moves++ ) = Move( 60, 58, QUEEN_SIDE_CASTLE_FLAG );
		}
	}

	return moves;
}

Move* GetQuietPawnMoves( Move *moves, const Board &board, const Bitboard pushMap, const Bitboard diagPins,
                         const Bitboard orthoPins, const SideToMove side ) {
	Bitboard verticalPin = orthoPins & (orthoPins << 8);
	verticalPin |= orthoPins >> 8;
	const Bitboard promotionRank = side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
	const Bitboard doublePushRank = side == WHITE ? Bitboard::RANK_2 : Bitboard::RANK_7;
	const Bitboard pawnPool = board.GetPieceMask( PAWN, side ) & ~diagPins & ~promotionRank;
	const Bitboard pinnedPawns = pawnPool & verticalPin;
	const Bitboard movablePawns = (pawnPool & ~orthoPins) | pinnedPawns;
	const Bitboard doublePushPawns = movablePawns & doublePushRank;

	const Bitboard singlePushMap = side == WHITE ? pushMap >> 8 : pushMap << 8;
	const Bitboard doublePushMap = side == WHITE ? pushMap >> 16 : pushMap << 16;
	const Bitboard singlePushEmptyMap = side == WHITE ? ~board.GetOccupancy() >> 8 : ~board.GetOccupancy() << 8;

	Bitboard pawns = movablePawns & singlePushMap;
	Bitboard targets = side == WHITE ? pawns << 8 : pawns >> 8 ;
	auto a = Bitboard( pawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), QUIET_MOVE_FLAG );
	}

	pawns = doublePushPawns & singlePushEmptyMap & doublePushMap;
	targets = side == WHITE ? pawns << 16 : pawns >> 16 ;
	a = Bitboard( pawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), DOUBLE_PUSH_FLAG );
	}

	return moves;
}

Move* GetNoisyPawnMoves( Move *moves, const Board &board, const Bitboard pushMap, const Bitboard captureMap,
                         const Bitboard diagPins, const Bitboard orthoPins, const SideToMove side,
                         const SideToMove oppositeSide, const Square kingSquare ) {
	const Bitboard promotionRank = side == WHITE ? Bitboard::RANK_7 : Bitboard::RANK_2;
	const Bitboard pawns = board.GetPieceMask( PAWN, side );
	const Bitboard attackPawns = pawns & ~orthoPins;

	uint8_t shift = side == WHITE ? 7 : 9;
	Bitboard leftPin = diagPins & ( diagPins << shift );
	leftPin |= leftPin >> shift;

	shift = side == WHITE ? 9 : 7;
	Bitboard rightPin = diagPins & ( diagPins << shift );
	rightPin |= rightPin >> shift;

	const Bitboard leftShiftedCaptureMap = side == WHITE ? captureMap >> 7 : captureMap << 9;
	const Bitboard rightShiftedCaptureMap = side == WHITE ? captureMap >> 9 : captureMap << 7;

	const Bitboard leftAttackPawns = ( ( attackPawns & ~diagPins ) | ( attackPawns & leftPin ) ) & ~Bitboard::FILE_A;
	const Bitboard rightAttackPawns = ( ( attackPawns & ~diagPins ) | ( attackPawns & rightPin ) ) & ~Bitboard::FILE_H;

	const Bitboard leftPromotionPawns = leftAttackPawns & promotionRank;
	const Bitboard rightPromotionPawns = rightAttackPawns & promotionRank;

	Bitboard capturePawns = leftAttackPawns & ~leftPromotionPawns & leftShiftedCaptureMap;
	Bitboard targets = side == WHITE ? capturePawns << 7 : capturePawns >> 9;
	auto a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), CAPTURE_FLAG );
	}

	capturePawns = rightAttackPawns & ~rightPromotionPawns & rightShiftedCaptureMap;
	targets = side == WHITE ? capturePawns << 9 : capturePawns >> 7;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		*( moves++ ) = Move( a.PopLs1bBit(), b.PopLs1bBit(), CAPTURE_FLAG );
	}

	capturePawns = leftPromotionPawns & leftShiftedCaptureMap;
	targets = side == WHITE ? capturePawns << 7 : capturePawns >> 9;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_CAPTURE_FLAG );
	}

	capturePawns = rightPromotionPawns & rightShiftedCaptureMap;
	targets = side == WHITE ? capturePawns << 9 : capturePawns >> 7;
	a = Bitboard( capturePawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_CAPTURE_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_CAPTURE_FLAG );
	}

	Bitboard promotionPawns = pawns & promotionRank & ~diagPins;
	targets = ( side == WHITE ? promotionPawns << 8 : promotionPawns >> 8 ) & pushMap;
	promotionPawns = side == WHITE ? targets >> 8 : targets << 8;
	a = Bitboard( promotionPawns ), b = Bitboard( targets );
	while ( a ) {
		const Square from = a.PopLs1bBit(), to = b.PopLs1bBit();
		*( moves++ ) = Move( from, to, KNIGHT_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, BISHOP_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, ROOK_PROMOTION_FLAG );
		*( moves++ ) = Move( from, to, QUEEN_PROMOTION_FLAG );
	}

	const Square enPassantSquare = board.GetEnPassantSquare();
	if ( enPassantSquare == NULL_SQUARE )
		return moves;

	const Bitboard enPassantPawns = Attacks::GetPawnAttacks( enPassantSquare, oppositeSide ) & attackPawns;
	enPassantPawns.Map( [&moves, &board, enPassantSquare, kingSquare, side]( const Square square ) {
		Board temp = board;
		const auto move = Move( square, enPassantSquare, EN_PASSANT_FLAG );
		temp.MakeMove( move );

		if ( !Attacks::IsSquareAttacked( temp, kingSquare, side ) ) {
			*( moves++ ) = move;
		}
	} );

	return moves;
}

Move* GetPieceMoves( Move *moves, const Board &board, const PieceType piece, const Bitboard moveMap,
                     const Bitboard diagPins, const Bitboard orthoPins, const MoveFlag flag, const SideToMove side ) {
	Bitboard pieces = Bitboard::EMPTY;
	switch ( piece ) {
		case KNIGHT:
			pieces = board.GetPieceMask( KNIGHT, side ) & ~diagPins & ~orthoPins;
			break;
		case BISHOP:
			pieces = ( board.GetPieceMask( BISHOP, side ) | board.GetPieceMask( QUEEN, side ) ) & ~orthoPins;
			break;
		case ROOK:
			pieces = ( board.GetPieceMask( ROOK, side ) | board.GetPieceMask( QUEEN, side ) ) & ~diagPins;
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

	nonPinnedPieces.Map( [&moves, &board, piece, moveMap, flag]( const Square fromSquare ) {
		Bitboard attacks = Bitboard::EMPTY;
		switch ( piece ) {
			case KNIGHT: attacks = Attacks::GetKnightAttacks( fromSquare );
				break;
			case BISHOP: attacks = Attacks::GetBishopAttacks( fromSquare, board.GetOccupancy() );
				break;
			case ROOK: attacks = Attacks::GetRookAttacks( fromSquare, board.GetOccupancy() );
				break;
			default: break;
		}

		( attacks & moveMap ).Map( [&moves, fromSquare, flag]( const Square toSquare ) {
			*( moves++ ) = Move( fromSquare, toSquare, flag );
		} );
	} );

	pinnedPieces.Map( [&moves, &board, piece, moveMap, diagPins, orthoPins, flag]( const Square fromSquare ) {
		Bitboard attacks = Bitboard::EMPTY;
		switch ( piece ) {
			case BISHOP: attacks = Attacks::GetBishopAttacks( fromSquare, board.GetOccupancy() ) & diagPins;
				break;
			case ROOK: attacks = Attacks::GetRookAttacks( fromSquare, board.GetOccupancy() ) & orthoPins;
				break;
			default: break;
		}

		( attacks & moveMap ).Map( [&moves, fromSquare, flag]( const Square toSquare ) {
			*( moves++ ) = Move( fromSquare, toSquare, flag );
		} );
	} );

	return moves;
}

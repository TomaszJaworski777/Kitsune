#pragma once

#include "bitboard.h"
#include "board.h"
#include "move.h"
#include "attacks/pin_mask.h"
#include "attacks/rays.h"

struct MoveGenerator {
	private:
		const Board &m_Board;
		const SideToMove m_Side;
		const SideToMove m_OppositeSide;
		const Square m_KingSquare;
		const PinMask m_PinMask;
		const Bitboard m_AttackMap;
		const Bitboard m_Checkers;
		const Bitboard m_KingMoveMap;

	public:
		MoveGenerator( const Board &board );

		template<MoveGenMode MODE>
		uint8_t GenerateMoves( Move *moves ) const {
			return GenerateMoves_Internal<MODE>( moves );
		}

	private:
		template<MoveGenMode MODE>
		uint8_t GenerateMoves_Internal( Move *moves ) const {
			const Move *start = moves;

			const auto diagPins = m_PinMask.GetDiagonalMask();
			const auto orthoPins = m_PinMask.GetOrthographicMask();
			const auto captureMap = m_Board.GetOccupancy( m_OppositeSide );

			const auto flippedOccupancy = ~m_Board.GetOccupancy();

			if constexpr ( MODE & MoveGenMode::QUIET ) {
				moves = GetQuietKingMoves( moves, m_KingMoveMap & flippedOccupancy, m_KingSquare );
			}

			if constexpr ( MODE & MoveGenMode::NOISY ) {
				moves = GetNoisyKingMoves( moves, m_KingMoveMap & captureMap, m_KingSquare );
			}

			if ( !m_Checkers ) {
				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetCastleMoves( moves, m_Board, m_AttackMap, m_Side );
				}

				const auto pushMap = ~m_Board.GetOccupancy();

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetQuietPawnMoves( moves, m_Board, pushMap, diagPins, orthoPins, m_Side );
					moves = GetPieceMoves( moves, m_Board, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					moves = GetNoisyPawnMoves( moves, m_Board, pushMap, captureMap, diagPins, orthoPins, m_Side, m_OppositeSide,
					                           m_KingSquare );
					moves = GetPieceMoves( moves, m_Board, KNIGHT, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, BISHOP, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, ROOK, captureMap, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
				}
			} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
				const auto checker = m_Checkers.Ls1bSquare();
				const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetQuietPawnMoves( moves, m_Board, pushMap, diagPins, orthoPins, m_Side );
					moves = GetPieceMoves( moves, m_Board, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG, m_Side );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					moves = GetNoisyPawnMoves( moves, m_Board, pushMap, m_Checkers, diagPins, orthoPins, m_Side, m_OppositeSide,
					                           m_KingSquare );
					moves = GetPieceMoves( moves, m_Board, KNIGHT, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, BISHOP, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
					moves = GetPieceMoves( moves, m_Board, ROOK, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG, m_Side );
				}
			}

			return static_cast<uint8_t>(moves - start);
		}

		static Move* GetQuietKingMoves( Move *moves, Bitboard moveMap, Square kingSquare );

		static Move* GetNoisyKingMoves( Move *moves, Bitboard moveMap, Square kingSquare );

		static Move* GetCastleMoves( Move *moves, const Board &board, Bitboard attackMap, SideToMove sideToMove );

		static Move* GetQuietPawnMoves( Move *moves, const Board &board, Bitboard pushMap, Bitboard diagPins, Bitboard orthoPins,
		                                SideToMove side );

		static Move* GetNoisyPawnMoves( Move *moves, const Board &board, Bitboard pushMap, Bitboard captureMap, Bitboard diagPins,
		                                Bitboard orthoPins, SideToMove side, SideToMove oppositeSide, Square kingSquare );

		static Move* GetPieceMoves( Move *moves, const Board &board, PieceType piece, Bitboard moveMap, Bitboard diagPins,
		                            Bitboard orthoPins, MoveFlag flag, SideToMove side );
};

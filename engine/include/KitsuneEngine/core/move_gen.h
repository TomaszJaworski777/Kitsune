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
				moves = GetQuietKingMoves( moves, m_KingMoveMap & flippedOccupancy );
			}

			if constexpr ( MODE & MoveGenMode::NOISY ) {
				moves = GetNoisyKingMoves( moves, m_KingMoveMap & captureMap );
			}

			if ( !m_Checkers ) {
				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetCastleMoves( moves );
				}

				const auto pushMap = ~m_Board.GetOccupancy();

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetQuietPawnMoves( moves, pushMap, diagPins, orthoPins );
					moves = GetPieceMoves( moves, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
					moves = GetPieceMoves( moves, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
					moves = GetPieceMoves( moves, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					moves = GetNoisyPawnMoves( moves, pushMap, captureMap, diagPins, orthoPins );
					moves = GetPieceMoves( moves, KNIGHT, captureMap, diagPins, orthoPins, CAPTURE_FLAG );
					moves = GetPieceMoves( moves, BISHOP, captureMap, diagPins, orthoPins, CAPTURE_FLAG );
					moves = GetPieceMoves( moves, ROOK, captureMap, diagPins, orthoPins, CAPTURE_FLAG );
				}
			} else if ( !( m_Checkers & ( m_Checkers - 1 ) ) ) {
				const auto checker = m_Checkers.Ls1bSquare();
				const auto pushMap = Rays::GetRayExcludeDestination( m_KingSquare, checker );

				if constexpr ( MODE & MoveGenMode::QUIET ) {
					moves = GetQuietPawnMoves( moves, pushMap, diagPins, orthoPins );
					moves = GetPieceMoves( moves, KNIGHT, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
					moves = GetPieceMoves( moves, BISHOP, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
					moves = GetPieceMoves( moves, ROOK, pushMap, diagPins, orthoPins, QUIET_MOVE_FLAG );
				}

				if constexpr ( MODE & MoveGenMode::NOISY ) {
					moves = GetNoisyPawnMoves( moves, pushMap, m_Checkers, diagPins, orthoPins );
					moves = GetPieceMoves( moves, KNIGHT, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG );
					moves = GetPieceMoves( moves, BISHOP, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG );
					moves = GetPieceMoves( moves, ROOK, m_Checkers, diagPins, orthoPins, CAPTURE_FLAG );
				}
			}

			return static_cast<uint8_t>(moves - start);
		}

		Move* GetQuietKingMoves( Move *moves, Bitboard moveMap ) const;

		Move* GetNoisyKingMoves( Move *moves, Bitboard moveMap ) const;

		Move* GetCastleMoves( Move *moves ) const;

		Move* GetQuietPawnMoves( Move *moves, Bitboard pushMap, Bitboard diagPins, Bitboard orthoPins ) const;

		Move* GetNoisyPawnMoves( Move *moves, Bitboard pushMap, Bitboard captureMap, Bitboard diagPins,
		                         Bitboard orthoPins ) const;

		Move* GetPieceMoves( Move *moves, PieceType piece, Bitboard moveMap, Bitboard diagPins,
		                     Bitboard orthoPins, MoveFlag flag ) const;
};

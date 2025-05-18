#pragma  once

#include <string>
#include <vector>

struct FEN {
	private:
		std::string m_Board[8];
		std::string m_SideToMove;
		std::string m_CastleRights;
		std::string m_EnPassantSquare;
		std::string m_HalfMoveCounter;
		std::string m_FullMoveCounter;
		bool m_Chess960;

	public:
		FEN( const std::string &str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );

		[[nodiscard]]
		static bool IsFenValid( const std::string &str );

		[[nodiscard]]
		std::string GetBoardRow( const uint8_t index ) const {
			return m_Board[index];
		}

		[[nodiscard]]
		std::string GetSideToMove() const {
			return m_SideToMove;
		}

		[[nodiscard]]
		std::string GetCastleRights() const {
			return m_CastleRights;
		}

		[[nodiscard]]
		std::string GetEnPassantSquare() const {
			return m_EnPassantSquare;
		}

		[[nodiscard]]
		std::string GetHalfMoveCounter() const {
			return m_HalfMoveCounter;
		}

		[[nodiscard]]
		std::string GetFullMoveCounter() const {
			return m_FullMoveCounter;
		}

		[[nodiscard]]
		bool IsChess960() const {
			return m_Chess960;
		}

		[[nodiscard]]
		std::string ToString() const;

		FEN operator=( const std::string &value ) const {
			return FEN( value );
		}

	private:
		[[nodiscard]]
		static std::vector<std::string> Split( const std::string &str, char delimiter );
		[[nodiscard]]
		std::string NormalizeCastleRights( const std::string &rights );
};

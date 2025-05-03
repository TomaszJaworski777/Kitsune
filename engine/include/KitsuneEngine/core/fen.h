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
	public:
		FEN( const std::string& str = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1" );

		[[nodiscard]]
		static bool IsFenValid( const std::string& str );

		[[nodiscard]]
		std::string ToString() const;

		FEN operator=(const std::string& value) const {
			return FEN(value);
		}
	private:
		[[nodiscard]]
		static std::vector<std::string> Split( const std::string& str, char delimiter );
};
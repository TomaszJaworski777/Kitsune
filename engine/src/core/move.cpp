#include "KitsuneEngine/core/move.h"

#include <format>

static constexpr char PROMOTION_PIECE[4] = {'n', 'b', 'r', 'q'};

std::string Move::ToString( const bool chess960 ) const {
	if ( !m_Value )
		return "null_move";

	const auto fromSquare = GetFromSquare();
	const auto toSquare = GetToSquare();

	if ( !chess960 && IsCastle() ) {
		const auto flag = GetFlag();
		const auto side = fromSquare < 32 ? WHITE : BLACK;
		const Square destinationSquare = (flag == QUEEN_SIDE_CASTLE_FLAG ? C1 : G1) + 56 * side;
		return std::format("{}{}", fromSquare.ToString(), destinationSquare.ToString());
	}

	std::string result = std::format("{}{}", fromSquare.ToString(), toSquare.ToString());
	if (IsPromotion())
		result += PROMOTION_PIECE[GetPromotionPieceType() - 1];
	return result;
}

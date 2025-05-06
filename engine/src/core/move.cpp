#include "KitsuneEngine/core/move.h"

#include <format>

static constexpr char PROMOTION_PIECE[4] = {'n', 'b', 'r', 'q'};

std::string Move::ToString() const {
	if ( !m_Value )
		return "null_move";
	std::string result = std::format("{}{}", GetFromSquare().ToString(), GetToSquare().ToString());
	if (IsPromotion())
		result += PROMOTION_PIECE[GetPromotionPieceType() - 1];
	return result;
}

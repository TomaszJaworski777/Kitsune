#pragma once

#include <cstdint>

#include "square.h"
#include "KitsuneEngine/types.h"

static constexpr uint16_t FLAG_MASK = 15 << 6;

struct Move {
	private:
		uint16_t m_Value;

	public:
		constexpr Move( const uint16_t value = 0 ) {
			m_Value = value;
		}

		constexpr Move( const Square from, const Square to, const MoveFlag flag ) {
			m_Value = to << 10 | flag | from;
		}

		[[nodiscard]]
		constexpr Square GetFromSquare() const {
			return m_Value & 63;
		}

		[[nodiscard]]
		constexpr Square GetToSquare() const {
			return m_Value >> 10;
		}

		[[nodiscard]]
		constexpr MoveFlag GetFlag() const {
			return static_cast<MoveFlag>(m_Value & FLAG_MASK);
		}

		[[nodiscard]]
		constexpr bool IsCapture() const {
			return m_Value & CAPTURE_FLAG;
		}

		[[nodiscard]]
		constexpr bool IsEnPassant() const {
			return ( m_Value & EN_PASSANT_FLAG ) == EN_PASSANT_FLAG;
		}

		[[nodiscard]]
		constexpr bool IsPromotion() const {
			return m_Value & KNIGHT_PROMOTION_FLAG;
		}

		[[nodiscard]]
		constexpr bool IsCastle() const {
			const auto flag = GetFlag();
			return flag == KING_SIDE_CASTLE_FLAG || flag == QUEEN_SIDE_CASTLE_FLAG;
		}

		[[nodiscard]]
		constexpr PieceType GetPromotionPieceType() const {
			return static_cast<PieceType>(( GetFlag() >> 6 & 3 ) + 1);
		}

		[[nodiscard]]
		std::string ToString() const;

		constexpr operator uint16_t() const {
			return m_Value;
		}

		constexpr Move& operator=( const uint16_t value ) {
			m_Value = value;
			return *this;
		}
};

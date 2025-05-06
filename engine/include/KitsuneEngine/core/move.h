#pragma once

#include <cstdint>

#include "square.h"
#include "KitsuneEngine/types.h"

static constexpr uint16_t FLAG_MASK = 15 << 6;

struct Move {
	private:
		uint16_t m_Value;
	public:
		Move( const uint16_t value = 0 ) {
			m_Value = value;
		}

		Move( const Square from, const Square to, const MoveFlag flag ) {
			m_Value = to << 10 | flag | from;
		}

		[[nodiscard]]
		Square GetFromSquare() const {
			return m_Value & 63;
		}

		[[nodiscard]]
		Square GetToSquare() const {
			return m_Value >> 10;
		}

		[[nodiscard]]
		MoveFlag GetFlag() const {
			return static_cast<MoveFlag>(m_Value & FLAG_MASK);
		}

		[[nodiscard]]
		bool IsCapture() const {
			return m_Value & CAPTURE_FLAG;
		}

		[[nodiscard]]
		bool IsEnPassant() const {
			return (m_Value & EN_PASSANT_FLAG) == EN_PASSANT_FLAG;
		}

		[[nodiscard]]
		bool IsPromotion() const {
			return m_Value & KNIGHT_PROMOTION_FLAG;
		}

		[[nodiscard]]
		PieceType GetPromotionPieceType() const {
			return static_cast<PieceType>( ( GetFlag() >> 6 & 3 ) + 1 );
		}

		[[nodiscard]]
		std::string ToString() const;

		operator uint16_t() const {
			return m_Value;
		}

		Move& operator=(const uint16_t value) {
			m_Value = value;
			return *this;
		}
};

#pragma once

#include <algorithm>
#include <cstdint>
#include <string>

enum SquareIndex : uint8_t {
	A1, B1, C1, D1, E1, F1, G1, H1,
	A2, B2, C2, D2, E2, F2, G2, H2,
	A3, B3, C3, D3, E3, F3, G3, H3,
	A4, B4, C4, D4, E4, F4, G4, H4,
	A5, B5, C5, D5, E5, F5, G5, H5,
	A6, B6, C6, D6, E6, F6, G6, H6,
	A7, B7, C7, D7, E7, F7, G7, H7,
	A8, B8, C8, D8, E8, F8, G8, H8,
	NULL_SQUARE
};

struct Square {
	private:
		uint8_t m_Value;

	public:
		Square( const uint8_t value = NULL_SQUARE ) {
			m_Value = value;
		}

		Square( const uint8_t rank, const uint8_t file ) {
			m_Value = rank * 8 + file;
		}

		Square( const std::string_view &value ) {
			const uint8_t file = std::ranges::clamp( value[0] - 'a', 0, 7 );
			const uint8_t rank = std::ranges::clamp( value[1] - '0' - 1, 0, 7 );
			m_Value = rank * 8 + file;
		}

		[[nodiscard]]
		uint8_t GetRank() const {
			return m_Value / 8;
		}

		[[nodiscard]]
		uint8_t GetFile() const {
			return m_Value % 8;
		}

		[[nodiscard]]
		uint8_t Flipped() const {
			return m_Value ^ 56;
		}

		[[nodiscard]]
		std::string ToString() const {
			if ( m_Value == NULL_SQUARE ) {
				return "-";
			}

			std::string result;
			result += static_cast<char>('a' + GetFile());
			result += static_cast<char>('0' + GetRank() + 1);
			return result;
		}

		operator uint8_t() const {
			return m_Value;
		}

		Square &operator=( const uint8_t value ) {
			m_Value = value;
			return *this;
		}

		bool operator==( const Square value ) const {
			return m_Value == value.m_Value;
		}
};

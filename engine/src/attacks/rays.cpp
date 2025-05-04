#include "../../include/KitsuneEngine/attacks/rays.h"

#include <iostream>

struct Rays_Internal {
	private:
		Bitboard m_Rays[64][64]; //TODO: Test 1-dim
	public:
		Rays_Internal() {
			for (int from_square = 0; from_square < 64; from_square++) {
				for (int to_square = 0; to_square < 64; to_square++) {
					m_Rays[from_square][to_square] = GenerateRay( from_square, to_square );
				}
			}
		}

		[[nodiscard]]
		Bitboard GetRay( const Square from, const Square to ) const {
			return m_Rays[from][to];
		}
	private:
		static Bitboard GenerateRay( const Square from, const Square to  ) {
			const int8_t rank_inc = GetSign( to.GetRank() - from.GetRank() );
			const int8_t file_inc = GetSign( to.GetFile() - from.GetFile() );

			if (!rank_inc && !file_inc) {
				return Bitboard::EMPTY;
			}

			Bitboard result = Bitboard::EMPTY;
			int8_t rank = from.GetRank() + rank_inc;
			int8_t file = from.GetFile() + file_inc;
			while ( rank >= 0 && rank <= 7 && file >= 0 && file <= 7 ) {
				const auto square = Square( rank, file );
				result |= Bitboard(square);
				if ( square == to ) {
					return result;
				}

				rank += rank_inc;
				file += file_inc;
			}

			return Bitboard::EMPTY;
		}

		static int8_t GetSign( const int8_t value ) {
			if (value == 0 ) {
				return 0;
			}

			return value & 0x80 ? -1 : 1;
		}
};

static Rays_Internal RAYS;

Bitboard Rays::GetRay( const Square from, const Square to ) {
	return RAYS.GetRay( from, to );
}
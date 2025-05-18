#include "KitsuneEngine/core/bitboard.h"

#include <format>
#include "KitsuneEngine/console_colors.h"

std::string Bitboard::ToString() const {
	std::string result = " -----------------\n";
	for ( int rank = 7; rank >= 0; rank-- ) {
		result += "|";
		for ( int file = 0; file < 8; file++ ) {
			if ( const auto square = Square( rank, file ); GetBit( square ) ) {
				result += ColorText( " 1", GREEN );
			} else {
				result += ColorText( " 0", RED );
			}
		}
		result += " |\n";
	}

	result += " -----------------\n";
	result += std::format( "  Bitboard: {}\n", m_Value );
	return result;
}

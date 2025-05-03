#include "KitsuneEngine/core/fen.h"

#include <format>
#include <sstream>

#include "KitsuneEngine/core/square.h"

FEN::FEN( const std::string& str ) {
	const auto fenSplit = Split( str, ' ' );

	const auto positionParts = Split( fenSplit[0], '/' );
	for ( int i = 0; i < 8; i++ ) {
		m_Board[i] = positionParts[i];
	}

	m_SideToMove = fenSplit[1];
	m_CastleRights = fenSplit[2];
	m_EnPassantSquare = fenSplit[3];

	if (fenSplit.size() > 4) {
		m_HalfMoveCounter = fenSplit[4];
	} else {
		m_HalfMoveCounter = "0";
	}

	if (fenSplit.size() > 5) {
		m_FullMoveCounter = fenSplit[5];
	} else {
		m_FullMoveCounter = "1";
	}
}

bool FEN::IsFenValid( const std::string& str ) {
	const auto fenSplit = Split( str, ' ' );

	if ( fenSplit.size() < 4 ) {
		return false;
	}

	const auto positionParts = Split( fenSplit[0], '/' );
	if ( positionParts.size() != 8 ) {
		return false;
	}

	if ( fenSplit[1] != "w" && fenSplit[1] != "b" ) {
		return false;
	}

	if( fenSplit[2].size() == 1 && fenSplit[2] != "-" ) {
		return false;
	}

	const bool correctCastleRights = fenSplit[2].contains( 'K' )
		|| fenSplit[2].contains( 'Q' )
		|| fenSplit[2].contains( 'k' )
		|| fenSplit[2].contains( 'q' );

	if ( fenSplit[2].size() > 4 || !correctCastleRights ) {
		return false;
	}

	bool validateEnPassant = true;
	if ( fenSplit[3] != "-" ) {
		validateEnPassant = Square( fenSplit[3] ) < NULL_SQUARE;
	}

	if ( fenSplit[3].size() > 2 || !validateEnPassant ) {
		return false;
	}

	if ( fenSplit.size() > 4 ) {
		std::stringstream ss( fenSplit[4] );
		if ( int _; !(ss >> _) ) {
			return false;
		}
	}

	if ( fenSplit.size() > 5 ) {
		std::stringstream ss( fenSplit[5] );
		if ( int _; !(ss >> _) ) {
			return false;
		}
	}

	return true;
}

std::string FEN::ToString() const {
	return std::format("{}/{}/{}/{}/{}/{}/{}/{} {} {} {} {} {}",
		m_Board[0],
		m_Board[1],
		m_Board[2],
		m_Board[3],
		m_Board[4],
		m_Board[5],
		m_Board[6],
		m_Board[7],
		m_SideToMove,
		m_CastleRights,
		m_EnPassantSquare,
		m_HalfMoveCounter,
		m_FullMoveCounter
	);
}

std::vector<std::string> FEN::Split( const std::string& str, const char delimiter ) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find(delimiter);

	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + 1;
		end = str.find(delimiter, start);
	}

	tokens.push_back(str.substr(start));
	return tokens;
}


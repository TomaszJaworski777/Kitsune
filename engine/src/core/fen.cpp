#include "KitsuneEngine/core/fen.h"

#include <format>
#include <sstream>
#include <algorithm>

#include "KitsuneEngine/core/square.h"

FEN::FEN( const std::string &str ) {
	const auto fenSplit = Split( str, ' ' );

	const auto positionParts = Split( fenSplit[0], '/' );
	for ( int i = 0; i < 8; i++ ) {
		m_Board[i] = positionParts[i];
	}

	m_SideToMove = fenSplit[1];

	m_CastleRights = NormalizeCastleRights( positionParts, fenSplit[2] );

	m_EnPassantSquare = fenSplit[3];

	if ( fenSplit.size() > 4 ) {
		m_HalfMoveCounter = fenSplit[4];
	} else {
		m_HalfMoveCounter = "0";
	}

	if ( fenSplit.size() > 5 ) {
		m_FullMoveCounter = fenSplit[5];
	} else {
		m_FullMoveCounter = "1";
	}
}

bool FEN::IsFenValid( const std::string &str ) {
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

	bool validateEnPassant = true;
	if ( fenSplit[3] != "-" ) {
		validateEnPassant = Square( fenSplit[3] ) < NULL_SQUARE;
	}

	if ( fenSplit[3].size() > 2 || !validateEnPassant ) {
		return false;
	}

	if ( fenSplit.size() > 4 ) {
		std::stringstream ss( fenSplit[4] );
		if ( int _; !( ss >> _ ) ) {
			return false;
		}
	}

	if ( fenSplit.size() > 5 ) {
		std::stringstream ss( fenSplit[5] );
		if ( int _; !( ss >> _ ) ) {
			return false;
		}
	}

	return true;
}

std::string FEN::ToString() const {
	return std::format( "{}/{}/{}/{}/{}/{}/{}/{} {} {} {} {} {}",
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

std::vector<std::string> FEN::Split( const std::string &str, const char delimiter ) {
	std::vector<std::string> tokens;
	size_t start = 0;
	size_t end = str.find( delimiter );

	while ( end != std::string::npos ) {
		tokens.push_back( str.substr( start, end - start ) );
		start = end + 1;
		end = str.find( delimiter, start );
	}

	tokens.push_back( str.substr( start ) );
	return tokens;
}

std::string FEN::NormalizeCastleRights(
	const std::vector<std::string> &position,
	const std::string &rights
) {
	auto findFiles = []( const std::string &fenRank, const char target ) {
		std::vector<uint8_t> files;
		uint8_t file = 0;
		for ( const char character: fenRank ) {
			if ( std::isdigit( character ) ) {
				file += character - '0';
			} else {
				if ( character == target )
					files.push_back( file );
				file++;
			}
		}
		return files;
	};

	const uint8_t whiteKingFile = findFiles( position[7], 'K' )[0];
	const uint8_t blackKingFile = findFiles( position[0], 'k' )[0];

	const auto whiteRooks = findFiles( position[7], 'R' );
	const auto blackRooks = findFiles( position[0], 'r' );

	m_Chess960 = false;

	if ( rights == "-" ) {
		return "-";
	}

	std::string result;
	result.reserve( rights.size() );

	for ( const char character: rights ) {
		if ( character == 'K' ) {
			const auto it = std::find_if( whiteRooks.rbegin(), whiteRooks.rend(), [&]( auto f ) { return f > whiteKingFile; } );
			const uint8_t file = *it;
			result.push_back( static_cast<char>('A' + file) );
			if ( file != 0 && file != 7 )
				m_Chess960 = true;
		} else if ( character == 'Q' ) {
			auto it = std::ranges::find_if( whiteRooks, [&]( auto f ) { return f < whiteKingFile; } );
			const uint8_t file = *it;
			result.push_back( static_cast<char>('A' + file) );
			if ( file != 0 && file != 7 )
				m_Chess960 = true;
		} else if ( character == 'k' ) {
			auto it = std::find_if( blackRooks.rbegin(), blackRooks.rend(), [&]( auto f ) { return f > blackKingFile; } );
			const uint8_t file = *it;
			result.push_back( static_cast<char>('a' + file) );
			if ( file != 0 && file != 7 )
				m_Chess960 = true;
		} else if ( character == 'q' ) {
			auto it = std::ranges::find_if( blackRooks, [&]( auto f ) { return f < blackKingFile; } );
			const uint8_t file = *it;
			result.push_back( static_cast<char>('a' + file) );
			if ( file != 0 && file != 7 )
				m_Chess960 = true;
		} else if ( std::isupper( character ) ) {
			result.push_back( character );
			if ( character != 'A' && character != 'H' )
				m_Chess960 = true;
		} else if ( std::islower( character ) ) {
			result.push_back( character );
			if ( character != 'a' && character != 'h' )
				m_Chess960 = true;
		}
	}

	return result.empty() ? "-" : result;
}

#include "KitsuneEngine/core/board.h"

#include "KitsuneEngine/console_colors.h"
#include "KitsuneEngine/core/fen.h"

#include <format>

#include "KitsuneEngine/core/attacks/attacks.h"

Board::Board() {
	m_Occupancy[WHITE] = Bitboard::RANK_1 | Bitboard::RANK_2;
	m_Occupancy[BLACK] = Bitboard::RANK_7 | Bitboard::RANK_8;

	m_Pieces[PAWN] = Bitboard::RANK_2 | Bitboard::RANK_7;
	m_Pieces[KNIGHT] = 0x4200000000000042;
	m_Pieces[BISHOP] = 0x2400000000000024;
	m_Pieces[ROOK] = 0x8100000000000081;

	m_Pieces[QUEEN].SetBit( D1 );
	m_Pieces[QUEEN].SetBit( D8 );

	m_Pieces[KING].SetBit( E1 );
	m_Pieces[KING].SetBit( E8 );

	m_enPassantSquare = NULL_SQUARE;

	m_Side = WHITE;
	m_CastleRights = 0b1111;
	m_HalfMoves = 0;
	m_Phase = 24;

	m_Chess960 = false;

	m_Rooks[0] = 0b1000;
	m_Rooks[1] = 0b0100;
	m_Rooks[2] = 0b0010;
	m_Rooks[3] = 0b0001;

	m_Hash = 0x325BF1EB13D84627;
}

Board::Board( const FEN &fen ) {
	for ( int rankIndex = 0; rankIndex < 8; ++rankIndex ) {
		std::string rank = fen.GetBoardRow( rankIndex );
		for ( uint8_t file = 0, index = 0; file < 8; file++, index++ ) {
			const auto square = Square( 7 - rankIndex, file );
			const char pieceChar = rank[index];
			if ( std::isdigit( pieceChar ) ) {
				file += pieceChar - '0' - 1;
				continue;
			}

			const auto pieceColor = static_cast<SideToMove>(pieceChar > 90);

			switch ( std::tolower( pieceChar ) ) {
				case 'p': SetPieceOnSquare( square, PAWN, pieceColor );
					break;
				case 'n': SetPieceOnSquare( square, KNIGHT, pieceColor );
					break;
				case 'b': SetPieceOnSquare( square, BISHOP, pieceColor );
					break;
				case 'r': SetPieceOnSquare( square, ROOK, pieceColor );
					break;
				case 'q': SetPieceOnSquare( square, QUEEN, pieceColor );
					break;
				case 'k': SetPieceOnSquare( square, KING, pieceColor );
					break;
				default: break;
			}
		}
	}

	m_Chess960 = fen.IsChess960();

	if ( fen.GetSideToMove() == "w" ) {
		m_Side = WHITE;
	} else {
		m_Side = BLACK;
	}

	if ( Attacks::IsSquareAttacked( *this, GetKingSquare( ~m_Side ), ~m_Side ) ) {
		printf( "Tried to parse illegal position. Defaulting to starting position instead." );
		*this = Board();
		return;
	}

	m_Rooks[0] = 0b1000;
	m_Rooks[1] = 0b0100;
	m_Rooks[2] = 0b0010;
	m_Rooks[3] = 0b0001;

	m_CastleRights = 0;
	for ( const char character : fen.GetCastleRights() ) {
		const auto side = std::isupper( character ) ? WHITE : BLACK;
		const auto kingSquare = GetKingSquare( side );
		const uint8_t file = std::toupper( character ) - 'A';
		const auto index = 2 * side + (file < kingSquare.GetFile() ? 0 : 1);
		m_CastleRights |= 0b1000 >> index;
		m_Rooks[index] = Square( kingSquare.GetRank(), file );
	}

	m_enPassantSquare = NULL_SQUARE;
	if ( fen.GetEnPassantSquare() != "-" ) {
		m_enPassantSquare = Square( fen.GetEnPassantSquare() );
	}

	m_HalfMoves = std::stoi( fen.GetHalfMoveCounter() );
}

bool Board::IsInsufficientMaterial() const {
	const Bitboard bishops = GetPieceMask( BISHOP );
	return m_Phase <= 2 && !GetPieceMask( PAWN ) && (
		       m_Phase != 2 || ( ( bishops & GetOccupancy( WHITE ) ) != bishops && ( bishops & GetOccupancy( BLACK ) )
		                         != bishops && ( ( bishops & Bitboard( 0x55AA55AA55AA55AA ) ) == bishops || (
			                                         bishops & 0xAA55AA55AA55AA55 ) == bishops ) ) );
}

constexpr char PIECE_ICONS[2][6]{
	{ 'P', 'N', 'B', 'R', 'Q', 'K' },
	{ 'p', 'n', 'b', 'r', 'q', 'k' }
};

std::string Board::ToString() const {
	std::string result = "";

	std::string castleRights = "";

	if ( CanCastle( CASTLE_WHITE_KING ) ) {
		castleRights += "K";
	}
	if ( CanCastle( CASTLE_WHITE_QUEEN ) ) {
		castleRights += "Q";
	}
	if ( CanCastle( CASTLE_BLACK_KING ) ) {
		castleRights += "k";
	}
	if ( CanCastle( CASTLE_BLACK_QUEEN ) ) {
		castleRights += "q";
	}
	if ( castleRights == "" ) {
		castleRights = "-";
	}

	std::string stm = "b";
	if ( m_Side == WHITE ) {
		stm = "w";
	}

	std::string info[8]{
		std::format( "FEN: {}", ToFEN() ),
		std::format( "Chess960: {}", GetChess960() ),
		std::format( "Zobrist Hash: {:#x}", GetHash() ),
		std::format( "Castle Rights: {}", castleRights ),
		std::format( "Side To Move: {}", stm ),
		std::format( "En Passant: {}", GetEnPassantSquare().ToString() ),
		std::format( "Half Moves: {}", m_HalfMoves ),
		std::format( "Phase: {}", static_cast<int>(m_Phase) ),
	};

	result += " -----------------\n";
	for ( int rank = 7; rank >= 0; rank-- ) {
		result += '|';
		for ( int file = 0; file < 8; file++ ) {
			const auto square = Square( rank, file );
			if ( square == m_enPassantSquare ) {
				result += " x";
				continue;
			}

			const PieceType piece = GetPieceOnSquare( square );
			const SideToMove pieceColor = GetPieceColorOnSquare( square );
			if ( piece == NULL_PIECE ) {
				result += " .";
				continue;
			}

			const auto pieceIcon = std::format( " {}", PIECE_ICONS[pieceColor][piece] );
			result += ColorText( pieceIcon, pieceColor == WHITE ? YELLOW : BLUE );
		}
		result += std::format( " | {}\n", info[7 - rank] );
	}

	result += " -----------------\n";
	return result;
}

std::string Board::ToFEN() const {
	return "";
}

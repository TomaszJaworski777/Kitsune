#include "KitsuneEngine/core/board.h"

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
	m_Phase = 32; //TODO: This is not correct phase value

	m_Hash = 0xA75635CA4CDECB48; //TODO: Figure out what's the start position hash
}

Board::Board( const FEN &fen ) {
	//TODO: Position built from fen
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

	if ( fen.GetSideToMove() == "w" ) {
		m_Side = WHITE;
	} else {
		m_Side = BLACK;
	}
}

bool Board::IsInsufficientMaterial() const {
	const Bitboard bishops = GetPieceMask( BISHOP );
	return m_Phase <= 2 && !GetPieceMask( PAWN ) && ( m_Phase != 2 || (
		                                                  ( bishops & GetOccupancy( WHITE ) ) != bishops && (
			                                                  bishops & GetOccupancy( BLACK ) ) != bishops &&
		                                                  ( ( bishops & Bitboard( 0x55AA55AA55AA55AA ) ) == bishops || (
			                                                    bishops & 0xAA55AA55AA55AA55 ) == bishops )
	                                                  ) );
}

std::string Board::ToString() const {
	return "x";
}


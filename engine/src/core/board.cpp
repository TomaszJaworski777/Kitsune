#include "KitsuneEngine/core/board.h"

#include "KitsuneEngine/console_colors.h"
#include "KitsuneEngine/core/fen.h"

#include <format>

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

	m_Hash = 0x325bf1eb13d84627;

	m_PinMask = PinMask( *this, m_Side );
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

	if ( fen.GetSideToMove() == "w" ) {
		m_Side = WHITE;
	} else {
		m_Side = BLACK;
	}

	//TODO: If king opposite king is attacked, mark as illegal position

	m_CastleRights = 0;
	if ( fen.GetCastleRights().contains( 'k' ) ) {
		m_CastleRights |= CASTLE_BLACK_KING;
	}
	if ( fen.GetCastleRights().contains( 'q' ) ) {
		m_CastleRights |= CASTLE_BLACK_QUEEN;
	}
	if ( fen.GetCastleRights().contains( 'K' ) ) {
		m_CastleRights |= CASTLE_WHITE_KING;
	}
	if ( fen.GetCastleRights().contains( 'Q' ) ) {
		m_CastleRights |= CASTLE_WHITE_QUEEN;
	}

	m_enPassantSquare = NULL_SQUARE;
	if ( fen.GetEnPassantSquare() != "-" ) {
		m_enPassantSquare = Square( fen.GetEnPassantSquare() );
	}

	m_HalfMoves = std::stoi( fen.GetHalfMoveCounter() );

	m_PinMask = PinMask( *this, m_Side );
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
		std::format( "FEN: TBD" ),
		std::format( "Zobrist Hash: {:#x}", GetHash() ),
		std::format( "Castle Rights: {}", castleRights ),
		std::format( "Side To Move: {}", stm ),
		std::format( "En Passant: {}", GetEnPassantSquare().ToString() ),
		std::format( "Half Moves: {}", m_HalfMoves ),
		std::format( "Phase: {}", static_cast<int>(m_Phase) ),
		std::format( "Insufficient Material: {}", IsInsufficientMaterial() ),
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

void Board::MakeMove( const Move &move ) {
	const Square fromSquare = move.GetFromSquare();
	const Square toSquare = move.GetToSquare();
	const MoveFlag moveFlag = move.GetFlag();
	const bool isPromotion = move.IsPromotion();
	const bool isCapture = move.IsCapture();
	const SideToMove oppositeSideToMove = ~m_Side;
	const PieceType movedPiece = GetPieceOnSquare( fromSquare );
	const PieceType capturedPiece = GetPieceOnSquare( toSquare );

	if ( capturedPiece != NULL_PIECE ) {
		RemovePieceOnSquare( toSquare, capturedPiece, oppositeSideToMove );
	}

	RemovePieceOnSquare( fromSquare, movedPiece, m_Side );
	if ( !isPromotion ) {
		SetPieceOnSquare( toSquare, movedPiece, m_Side );
	}

	if ( movedPiece == PAWN || isCapture ) {
		m_HalfMoves = 0;
	} else {
		m_HalfMoves++;
	}

	m_CastleRights &= ~( CASTLE_MASK[fromSquare] | CASTLE_MASK[toSquare] );
	m_enPassantSquare = NULL_SQUARE;

	const uint8_t sideFlip = 56 * m_Side;
	switch ( moveFlag ) {
		case DOUBLE_PUSH_FLAG:
			m_enPassantSquare = toSquare ^ 8;
			break;
		case QUEEN_SIDE_CASTLE_FLAG:
			RemovePieceOnSquare( sideFlip, ROOK, m_Side );
			SetPieceOnSquare( sideFlip + 3, ROOK, m_Side );
			break;
		case KING_SIDE_CASTLE_FLAG:
			RemovePieceOnSquare( sideFlip + 7, ROOK, m_Side );
			SetPieceOnSquare( sideFlip + 5, ROOK, m_Side );
			break;
		case EN_PASSANT_FLAG:
			RemovePieceOnSquare( toSquare ^ 8, PAWN, oppositeSideToMove );
			break;
		case KNIGHT_PROMOTION_FLAG:
		case BISHOP_PROMOTION_FLAG:
		case ROOK_PROMOTION_FLAG:
		case QUEEN_PROMOTION_FLAG:
		case KNIGHT_PROMOTION_CAPTURE_FLAG:
		case BISHOP_PROMOTION_CAPTURE_FLAG:
		case ROOK_PROMOTION_CAPTURE_FLAG:
		case QUEEN_PROMOTION_CAPTURE_FLAG:
			SetPieceOnSquare( toSquare, move.GetPromotionPieceType(), m_Side );
			break;
		default:
			break;
	}

	m_Side = ~m_Side;
}

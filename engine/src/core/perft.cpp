#include "KitsuneEngine/core/perft.h"

#include <format>

#include "KitsuneEngine/core/move.h"
#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/move_gen.h"

uint64_t Perft( const Board &board, uint8_t depth, bool bulk, bool printSplit, bool isFirst ) {
	if ( depth == 0 ) {
		return 1;
	}

	Move moves[MAX_MOVES] {};
	uint8_t movesCount = 0;
	const auto moveGenerator = MoveGenerator( board );
	movesCount = moveGenerator.GenerateNoisyMoves( moves );
	movesCount += moveGenerator.GenerateQuietMoves( moves + movesCount );

	if ( depth == 1 && bulk ) {
		return movesCount;
	}

	uint64_t result = 0;

	for ( uint8_t i = 0; i < movesCount; ++i ) {
		Board newBoard = board;
		Move move = moves[i];
		newBoard.MakeMove( move );
		const uint64_t split = Perft( newBoard, depth - 1, bulk, printSplit, false );

		result += split;

		if ( printSplit && isFirst ) {
			printf( std::format( "{} - {}\n", move.ToString(), split ).c_str() );
		}
	}

	return result;
}


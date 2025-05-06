#include <iostream>
#include <thread>

#include "logo.h"
#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/fen.h"
#include "KitsuneEngine/core/move.h"
#include "KitsuneEngine/core/move_gen.h"
#include "KitsuneEngine/core/attacks/attacks.h"

int main() {
	const auto infos = new std::string[27] {};
	infos[3] = "Kitsune Chess Engine";
	infos[5] = "by Tomasz Jaworski";
	infos[6] = "and Dorian Kernel";
	infos[8] = "Version: 0.1";
	infos[11] = "Supported Non-UCI Commands:";
	infos[12] = "	perft <depth>";
	infos[13] = "	bulk <depth>";
	infos[14] = "	bench <depth>";
	infos[15] = "	eval";
	infos[16] = "	draw";
	std::cout << "\n" << GetASCIILogo(infos) << std::endl << std::endl;

	auto board = Board( FEN( "1r5r/P7/3k4/5pP1/8/8/P7/R3K2R w KQ f6 0 1" ) );
	std::cout << board.ToString() << std::endl;

	Move moves[218] {};
	uint8_t size = 0;

	auto thread = std::jthread( [&moves, &board, &size](std::stop_token token) {
		const auto moveGenerator = MoveGenerator( board );
		size = moveGenerator.GenerateNoisyMoves( moves );
		size += moveGenerator.GenerateQuietMoves( moves + size );
	} );

	thread.join();

	std::cout << "Size: " << static_cast<uint32_t>(size) << std::endl;
	for (uint8_t i = 0; i < size; i++) {
		std::cout << moves[i].ToString() << " - 1" << std::endl;
	}

	return 0;
}

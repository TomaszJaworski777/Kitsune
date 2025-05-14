#include <chrono>
#include <iostream>
#include <thread>

#include "logo.h"
#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/fen.h"
#include "KitsuneEngine/core/perft.h"

int main() {
	const auto infos = new std::string[27]{ };
	infos[3] = "Kitsune Chess Engine";
	infos[5] = "by Tomasz Jaworski";
	infos[6] = "and Dorian Kernel";
	infos[8] = "Version: 0.1";
	infos[11] = "Supported Non-UCI Commands:";
	infos[12] = "   perft <depth>";
	infos[13] = "   bulk <depth>";
	infos[14] = "   bench <depth>";
	infos[15] = "   eval";
	infos[16] = "   draw";
	std::cout << "\n" << GetASCIILogo( infos ) << std::endl << std::endl;

	auto board = Board( FEN( ) );
	std::cout << board.ToString() << std::endl;

	const auto castleRules = board.GetCastleRules();

	for ( int x = 0; x < 64; x++ ) {
		std::cout << static_cast<uint32_t>(CASTLE_MASK[x]) << " --> " << static_cast<uint32_t>(castleRules.GetMask( x )) << std::endl;
	}

	auto thread = std::jthread( [&board, &castleRules]( std::stop_token token ) {
		auto t = std::chrono::high_resolution_clock::now();
		uint64_t result = Perft( board, castleRules, 6, false, true, true );
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - t );
		std::cout << "Result: " << result << std::endl;
		std::cout << "Time: " << duration << std::endl;
		std::cout << "Speed: " << ( result * 1000 / duration.count() ) << "nps" << std::endl << std::endl << std::endl;

		t = std::chrono::high_resolution_clock::now();
		result = Perft( board, castleRules, 7, true, true, true );
		duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - t );
		std::cout << "Result: " << result << std::endl;
		std::cout << "Time: " << duration << std::endl;
		std::cout << "Speed: " << ( result * 1000 / duration.count() ) << "nps" << std::endl << std::endl << std::endl;
	} );

	thread.join();

	return 0;
}

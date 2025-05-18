#include <chrono>
#include <fstream>
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

	const auto fen = FEN( "b1q1rrkb/pppppppp/3nn3/8/P7/1PPP4/4PPPP/BQNNRKRB w GE - 1 9" );
	std::cout << fen.ToString() << std::endl;

	auto board = Board( fen );
	const auto castleMask = board.GenerateCastleMask();

	board.MakeMove( Move( F1, G1, KING_SIDE_CASTLE_FLAG ), castleMask );

	board.MakeMove( Move( A7, A6, QUIET_MOVE_FLAG ), castleMask );

	std::cout << board.ToString() << std::endl;
	std::cout << board.GetOccupancy().ToString() << std::endl;

	std::cout << board.GetRookSquare( 0 ).ToString() << std::endl;
	std::cout << board.GetRookSquare( 1 ).ToString() << std::endl;
	std::cout << board.GetRookSquare( 2 ).ToString() << std::endl;
	std::cout << board.GetRookSquare( 3 ).ToString() << std::endl;

	// for ( int i = 0; i < 64; ++i ) {
	// 	std::cout << static_cast<uint32_t>(castleRules.GetMask( i )) << std::endl;
	// }

	auto thread = std::jthread( [&board, &castleMask]( std::stop_token token ) {
		auto t = std::chrono::high_resolution_clock::now();
		uint64_t result = Perft( board, castleMask, 1, false, true, true );
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
			std::chrono::high_resolution_clock::now() - t );
		std::cout << "Result: " << result << std::endl;
		std::cout << "Time: " << duration << std::endl;
		std::cout << "Speed: " << ( result * 1000 / ( duration.count() + 1 ) ) << "nps" << std::endl << std::endl << std::endl;

		// t = std::chrono::high_resolution_clock::now();
		// result = Perft( board, castleRules, 7, true, true, true );
		// duration = std::chrono::duration_cast<std::chrono::milliseconds>(
		// 	std::chrono::high_resolution_clock::now() - t );
		// std::cout << "Result: " << result << std::endl;
		// std::cout << "Time: " << duration << std::endl;
		// std::cout << "Speed: " << ( result * 1000 / ( duration.count() + 1 ) ) << "nps" << std::endl << std::endl << std::endl;
	} );

	thread.join();

	return 0;
}

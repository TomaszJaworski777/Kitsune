#include <catch2/catch_test_macros.hpp>

#include "KitsuneEngine/core/fen.h"

TEST_CASE( "Castle Convertion", "[FenTests]" ) {
	SECTION( "HAha" ) {
		const auto fenString = "rbbqn1kr/pp2p1pp/6n1/2pp1p2/2P4P/P7/BP1PPPP1/R1BQNNKR w HAha - 0 9";
		const auto fen = FEN( fenString );
		CHECK( fen.GetCastleRights() == "HAha" );
	}
	SECTION( "KQgq" ) {
		const auto fenString = "brnr1krr/pp3ppp/3ppn2/2p5/5P2/P2P4/NPP1P1PP/BQ1BRRKR w KQgq - 2 9";
		const auto fen = FEN( fenString );
		CHECK( fen.GetCastleRights() == "HEgb" );
	}
	SECTION( "KFkq" ) {
		const auto fenString = "brnr1krr/pp3ppp/3ppn2/2p5/5P2/P2P4/NPP1P1PP/BQ1BRRKR w KFkq - 2 9";
		const auto fen = FEN( fenString );
		CHECK( fen.GetCastleRights() == "HFhb" );
	}
	SECTION( "KQkq" ) {
		const auto fenString = "brkr2rr/pp3ppp/3ppn2/2p5/5P2/P2P4/NPP1P1PP/BQRBKR1R w KQkq - 2 9";
		const auto fen = FEN( fenString );
		CHECK( fen.GetCastleRights() == "HChb" );
	}
	SECTION( "FBdq" ) {
		const auto fenString = "rrkrrrrr/pp3ppp/3ppn2/2p5/5P2/P2P4/NPP1P1PP/RRRRKRRR w FBdq - 2 9";
		const auto fen = FEN( fenString );
		CHECK( fen.GetCastleRights() == "FBda" );
	}
}

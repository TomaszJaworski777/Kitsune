#include <iostream>

#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/board.h"
#include "KitsuneEngine/core/move.h"
#include "KitsuneEngine/core/fen.h"

int main() {
    std::cout << "Square: (" << Square( E4 ).ToString() << ")" << std::endl;

    std::cout << Bitboard( Square( E4 ) ).ToString() << std::endl;

    std::cout << Move( Square( E4 ), Square( E5 ), QUIET_MOVE ).GetFromSquare().ToString() << std::endl;
    std::cout << Move( Square( E5 ), Square( D6 ), CAPTURE ).IsEnPassant() << std::endl;
    std::cout << Move( Square( E5 ), Square( D6 ), EN_PASSANT ).GetFlag() << std::endl;
    std::cout << Move( Square( A7 ), Square( A8 ), ROOK_PROMOTION ).IsPromotion() << std::endl;
    std::cout << Move( Square( A7 ), Square( A8 ), ROOK_PROMOTION ).ToString() << std::endl << std::endl;

    const auto board = Board( FEN() );
    std::cout << board.ToString() << std::endl;

    return 0;
}

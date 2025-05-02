#include <iostream>

#include "KitsuneEngine/core/bitboard.h"
#include "KitsuneEngine/core/move.h"

int main() {
    std::cout << "Square: (" << Square(E4).ToString() << ")" << std::endl;

    std::cout << Bitboard(Square(E4)).ToString() << std::endl;

    std::cout << Move(Square(E4), Square(E5), QUIET_MOVE).GetFromSquare().ToString() << std::endl;
    std::cout << Move(Square(E5), Square(D6), CAPTURE).IsEnPassant() << std::endl;
    std::cout << Move(Square(E5), Square(D6), EN_PASSANT).GetFlag() << std::endl;
    std::cout << Move(Square(A7), Square(A8), ROOK_PROMOTION).IsPromotion() << std::endl;
    std::cout << Move(Square(A7), Square(A8), ROOK_PROMOTION).ToString() << std::endl;

    return 0;
}

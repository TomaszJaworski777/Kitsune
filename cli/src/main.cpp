#include <iostream>

#include "KitsuneEngine/core/bitboard.h"

int main() {
    std::cout << "Square: (" << Square(E4).ToString() << ")" << std::endl;

    std::cout << Bitboard(Square(E4)).ToString() << std::endl;

    return 0;
}
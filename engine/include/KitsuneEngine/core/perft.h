#pragma once

#include <cstdint>

class Board;

uint64_t Perft( const Board &board, uint8_t depth, bool bulk, bool printSplit, bool isFirst );
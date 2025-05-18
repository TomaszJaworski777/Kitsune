#pragma once

#include <cstdint>

#include "castle_mask.h"

class Board;

uint64_t Perft( const Board &board, const CastleMask &castleMask, uint8_t depth, bool bulk, bool printSplit, bool isFirst );

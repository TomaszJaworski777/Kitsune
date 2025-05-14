#pragma once

#include <cstdint>

#include "castle_rules.h"

class Board;

uint64_t Perft( const Board &board, const CastleRules &castleRules, uint8_t depth, bool bulk, bool printSplit, bool isFirst );
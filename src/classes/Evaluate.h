#pragma once

#include <array>

/* piece/sq tables */

// piece square tables for every piece (from chess programming wiki)
const std::array<int, 64> pawnTable = {
    +00, +00, +00, +00, +00, +00, +00, +00,
    +50, +50, +50, +50, +50, +50, +50, +50,
    +10, +10, +20, +30, +30, +20, +10, +10,
    +05, +05, +10, +25, +25, +10, +05, +05,
    +00, +00, +00, +20, +20, +00, +00, +00,
    +05, -05, -10, +00, +00, -10, -05, +05,
    +05, +10, +10, -20, -20, +10, +10, +05,
    +00, +00, +00, +00, +00, +00, +00, +00};

const std::array<int, 64> knightTable = {
    -50, -40, -30, -30, -30, -30, -40, -50,
    -40, -20, +00, +00, +00, +00, -20, -40,
    -30, +00, +10, +15, +15, +10, +00, -30,
    -30, +05, +15, +20, +20, +15, +05, -30,
    -30, +00, +15, +20, +20, +15, +00, -30,
    -30, +05, +10, +15, +15, +10, +05, -30,
    -40, -20, +00, +05, +05, +00, -20, -40,
    -50, -40, -30, -30, -30, -30, -40, -50};

const std::array<int, 64> rookTable = {
    +00, +00, +00, +05, +05, +00, +00, +00,
    -05, +00, +00, +00, +00, +00, +00, -05,
    -05, +00, +00, +00, +00, +00, +00, -05,
    -05, +00, +00, +00, +00, +00, +00, -05,
    -05, +00, +00, +00, +00, +00, +00, -05,
    -05, +00, +00, +00, +00, +00, +00, -05,
    +05, +10, +10, +10, +10, +10, +10, +05,
    +00, +00, +00, +00, +00, +00, +00, +00};

const std::array<int, 64> queenTable = {
    -20, -10, -10, -05, -05, -10, -10, -20,
    -10, +00, +00, +00, +00, +00, +00, -10,
    -10, +00, +05, +05, +05, +05, +00, -10,
    -05, +00, +05, +05, +05, +05, +00, -05,
    +00, +00, +05, +05, +05, +05, +00, -05,
    -10, +05, +05, +05, +05, +05, +00, -10,
    -10, +00, +05, +00, +00, +00, +00, -10,
    -20, -10, -10, -05, -05, -10, -10, -20};

const std::array<int, 64> kingTable = {
    +20, +30, +10, +00, +00, +10, +30, +20,
    +20, +20, +00, +00, +00, +00, +20, +20,
    -10, -20, -20, -20, -20, -20, -20, -10,
    -20, -30, -30, -40, -40, -30, -30, -20,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30,
    -30, -40, -40, -50, -50, -40, -40, -30};

const std::array<int, 64> bishopTable = {
    -20, -10, -10, -10, -10, -10, -10, -20,
    -10, +00, +00, +00, +00, +00, +00, -10,
    -10, +00, +05, +10, +10, +05, +00, -10,
    -10, +05, +05, +10, +10, +05, +05, -10,
    -10, +00, +10, +10, +10, +10, +00, -10,
    -10, +10, +10, +10, +10, +10, +10, -10,
    -10, +05, +00, +00, +00, +00, +05, -10,
    -20, -10, -10, -10, -10, -10, -10, -20};
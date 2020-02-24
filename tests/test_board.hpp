
#ifndef TEST_BOARD_H
#define TEST_BOARD_H

#include <string>
#include <iostream>

#include "assert.hpp"
#include "board.hpp"
#include "move.hpp"

inline int test_fen(const std::string& fen) {
  Board board{fen}, board2{fen};
  // NOTE: If fail due to missing en passant, suspect en passant square elision
  WASSERT_MSG(board.fen() == fen,
    "Parsed\n\t(%s)\nand generated\n\t(%s)\ndo not match.\n"
    "This could be due to en passant elision.",
      fen.c_str(), board.fen().c_str());

  ASSERT(board.fen() == board2.fen());
  ASSERT(board.hash() == board2.hash());
  ASSERT(board.to_string() == board2.to_string());
  return 0;
}

const static std::string testFENs[] = {
  Board::startFEN,
  "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1",
  "rnbqkbnr/pppppppp/8/8/4P3/8/PPPP1PPP/RNBQKBNR b KQkq e3 0 1",
  "rnbqkbnr/pp1ppppp/8/2p5/4P3/8/PPPP1PPP/RNBQKBNR w KQkq c6 0 2",
  "rnbqkbnr/pp1ppppp/8/2p5/4P3/5N2/PPPP1PPP/RNBQKB1R b KQkq - 1 2",
  "5k2/ppp5/4P3/3R3p/6P1/1K2Nr2/PP3P2/8 b - - 1 32",
  "1r2r2k/1p1n3R/p1qp2pB/6Pn/P1Pp4/3B4/1P2PQ1K/5R2 b - - 0 1",
  "R6R/3Q4/1Q4Q1/4Q3/2Q4Q/Q4Q2/pp1Q4/kBNN1KB1 w - - 0 1",
  "3Q4/1Q4Q1/4Q3/2Q4R/Q4Q2/3Q4/1Q4Rp/1K1BBNNk w - - 0 1",
  "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
  "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
};

inline int test_board() {
  int fail_flag = 0;
  for (const auto &fen : testFENs) {
    fail_flag |= test_fen(fen);
  }
  return fail_flag;
}

#endif /* end of include guard: TEST_BOARD_H */

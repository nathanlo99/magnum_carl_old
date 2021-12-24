
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <map>

#include "../tests/runtests.hpp"

#include "assert.hpp"
#include "board.hpp"
#include "evaluate.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "opening_book.hpp"
#include "perf_counter.hpp"
#include "piece_values.hpp"
#include "simulate.hpp"
#include "transposition_table.hpp"
#include "uci_protocol.hpp"

const std::string kqk_minus_8 = "6k1/8/8/8/8/5Q2/8/K7 b - - 0 1";
const std::string kqk_minus_7 = "8/6k1/8/5Q2/8/8/8/K7 b - - 2 2";
const std::string kqk_minus_6 = "7k/8/8/6Q1/8/8/8/K7 b - - 4 3";
const std::string kqk_minus_5 = "8/7k/8/6Q1/8/8/1K6/8 b - - 6 4";
const std::string kqk_minus_4 = "7k/8/8/6Q1/8/2K5/8/8 b - - 8 5";
const std::string kqk_minus_3 = "8/7k/8/6Q1/3K4/8/8/8 b - - 10 6";
const std::string kqk_minus_2 = "7k/8/8/4K1Q1/8/8/8/8 b - - 12 7";
const std::string kqk_minus_1 = "8/7k/5K2/6Q1/8/8/8/8 b - - 14 8";

const std::string krk = "8/8/8/8/4k3/8/8/6RK w - - 0 1";

const std::string explode =
    "q2k2q1/2nqn2b/1n1P1n1b/2rnr2Q/1NQ1QN1Q/3Q3B/2RQR2B/Q2K2Q1 w - - 0 1";
const std::string mate_in_5 =
    "6k1/3b3r/1p1p4/p1n2p2/1PPNpP1q/P3Q1p1/1R1RB1P1/5K2 b - - 0 1";
const std::string mate_in_6 = "q7/n2BNp2/5k1P/1p5P/1p2RP2/1K6/8/8 w - - 0 1";
const std::string puzzle =
    "r3kb1r/ppp1qpp1/3p4/4n2p/2Bp2P1/3P2Q1/PPP2PP1/RNB1R1K1 b - - 0 1";
const std::string behting = "8/8/7p/3KNN1k/2p4p/8/3P2p1/8 w - - 0 1";

void test_position(const std::string &fen) {
  transposition_table.clear();
  Board board(fen);
  SearchInfo info;
  info.depth = 20;
  info.seconds_to_search = 10.0;
  const move_t best_move = get_best_move(info, board);
  std::cout << board << std::endl;
  std::cout << "Best move is " << board.algebraic_notation(best_move)
            << std::endl;
}

int main(int argc, char *argv[]) {
  // std::cout << "Initializing playchess..." << std::endl;
  init_hash();
  init_piece_values();

  const auto book_ns = timeit(
      [&] { opening_book.read_book("references/book/opening_book.txt"); });
  // std::cout << "Reading processed opening book took " << (book_ns / 1e9)
  //           << " seconds" << std::endl;

  std::string perft_file =
      (argc > 1) ? argv[1] : "tests/perft_files/skip.perft";
  const int test_error = run_tests(perft_file, 6);
  ASSERT_MSG(!test_error, "Tests did not complete successfully");
  // printf("Done testing!\n"
  //        "====================================================================="
  //        "===\n");

  UCIProtocol::start_loop();

  if (false) {
    test_position(kqk_minus_1);
    test_position(kqk_minus_2);
    test_position(kqk_minus_3);
    test_position(kqk_minus_4);
    test_position(kqk_minus_5);
    test_position(kqk_minus_6);
    test_position(kqk_minus_7);
    test_position(kqk_minus_8);
  }

  if (false) {
    test_position(krk);
  }

  // test_position("8/8/3n1k2/1p3r2/8/2KQ2b1/4q3/8 w - - 0 1");
  // test_position(mate_in_5);
  // test_position(mate_in_6);
  // test_position(behting);
  // test_search_set("tests/search_tests/kaufman.search");

  // manual_play_white(50, 8.0);
}

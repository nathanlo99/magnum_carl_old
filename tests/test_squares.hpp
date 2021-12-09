
#pragma once

#include "../src/assert.hpp"
#include "../src/square.hpp"

inline int test_squares() {
  { /* valid_square */
   {/* valid_square true positives */
    ASSERT(valid_square(A1));
  ASSERT(valid_square(A2));
  ASSERT(valid_square(A3));
  ASSERT(valid_square(A4));
  ASSERT(valid_square(A5));
  ASSERT(valid_square(A6));
  ASSERT(valid_square(A7));
  ASSERT(valid_square(A8));
  ASSERT(valid_square(B1));
  ASSERT(valid_square(B2));
  ASSERT(valid_square(B3));
  ASSERT(valid_square(B4));
  ASSERT(valid_square(B5));
  ASSERT(valid_square(B6));
  ASSERT(valid_square(B7));
  ASSERT(valid_square(B8));
  ASSERT(valid_square(C1));
  ASSERT(valid_square(C2));
  ASSERT(valid_square(C3));
  ASSERT(valid_square(C4));
  ASSERT(valid_square(C5));
  ASSERT(valid_square(C6));
  ASSERT(valid_square(C7));
  ASSERT(valid_square(C8));
  ASSERT(valid_square(D1));
  ASSERT(valid_square(D2));
  ASSERT(valid_square(D3));
  ASSERT(valid_square(D4));
  ASSERT(valid_square(D5));
  ASSERT(valid_square(D6));
  ASSERT(valid_square(D7));
  ASSERT(valid_square(D8));
  ASSERT(valid_square(E1));
  ASSERT(valid_square(E2));
  ASSERT(valid_square(E3));
  ASSERT(valid_square(E4));
  ASSERT(valid_square(E5));
  ASSERT(valid_square(E6));
  ASSERT(valid_square(E7));
  ASSERT(valid_square(E8));
  ASSERT(valid_square(F1));
  ASSERT(valid_square(F2));
  ASSERT(valid_square(F3));
  ASSERT(valid_square(F4));
  ASSERT(valid_square(F5));
  ASSERT(valid_square(F6));
  ASSERT(valid_square(F7));
  ASSERT(valid_square(F8));
  ASSERT(valid_square(G1));
  ASSERT(valid_square(G2));
  ASSERT(valid_square(G3));
  ASSERT(valid_square(G4));
  ASSERT(valid_square(G5));
  ASSERT(valid_square(G6));
  ASSERT(valid_square(G7));
  ASSERT(valid_square(G8));
  ASSERT(valid_square(H1));
  ASSERT(valid_square(H2));
  ASSERT(valid_square(H3));
  ASSERT(valid_square(H4));
  ASSERT(valid_square(H5));
  ASSERT(valid_square(H6));
  ASSERT(valid_square(H7));
  ASSERT(valid_square(H8));
}
{ /* valid_square true negatives */
  for (int i = 0; i < 20; ++i) {
    ASSERT(!valid_square(i));
    ASSERT(!valid_square(i + 100));
  }
  for (int i = 0; i < 12; ++i) {
    ASSERT(!valid_square(10 * i + 0));
    ASSERT(!valid_square(10 * i + 9));
  }
  ASSERT(!valid_square(INVALID_SQUARE));
}
}

{ /* get_square_row */
  ASSERT(get_square_row(A1) == 0);
  ASSERT(get_square_row(A2) == 1);
  ASSERT(get_square_row(A3) == 2);
  ASSERT(get_square_row(A4) == 3);
  ASSERT(get_square_row(A5) == 4);
  ASSERT(get_square_row(A6) == 5);
  ASSERT(get_square_row(A7) == 6);
  ASSERT(get_square_row(A8) == 7);
  ASSERT(get_square_row(B1) == 0);
  ASSERT(get_square_row(B2) == 1);
  ASSERT(get_square_row(B3) == 2);
  ASSERT(get_square_row(B4) == 3);
  ASSERT(get_square_row(B5) == 4);
  ASSERT(get_square_row(B6) == 5);
  ASSERT(get_square_row(B7) == 6);
  ASSERT(get_square_row(B8) == 7);
  ASSERT(get_square_row(C1) == 0);
  ASSERT(get_square_row(C2) == 1);
  ASSERT(get_square_row(C3) == 2);
  ASSERT(get_square_row(C4) == 3);
  ASSERT(get_square_row(C5) == 4);
  ASSERT(get_square_row(C6) == 5);
  ASSERT(get_square_row(C7) == 6);
  ASSERT(get_square_row(C8) == 7);
  ASSERT(get_square_row(D1) == 0);
  ASSERT(get_square_row(D2) == 1);
  ASSERT(get_square_row(D3) == 2);
  ASSERT(get_square_row(D4) == 3);
  ASSERT(get_square_row(D5) == 4);
  ASSERT(get_square_row(D6) == 5);
  ASSERT(get_square_row(D7) == 6);
  ASSERT(get_square_row(D8) == 7);
  ASSERT(get_square_row(E1) == 0);
  ASSERT(get_square_row(E2) == 1);
  ASSERT(get_square_row(E3) == 2);
  ASSERT(get_square_row(E4) == 3);
  ASSERT(get_square_row(E5) == 4);
  ASSERT(get_square_row(E6) == 5);
  ASSERT(get_square_row(E7) == 6);
  ASSERT(get_square_row(E8) == 7);
  ASSERT(get_square_row(F1) == 0);
  ASSERT(get_square_row(F2) == 1);
  ASSERT(get_square_row(F3) == 2);
  ASSERT(get_square_row(F4) == 3);
  ASSERT(get_square_row(F5) == 4);
  ASSERT(get_square_row(F6) == 5);
  ASSERT(get_square_row(F7) == 6);
  ASSERT(get_square_row(F8) == 7);
  ASSERT(get_square_row(G1) == 0);
  ASSERT(get_square_row(G2) == 1);
  ASSERT(get_square_row(G3) == 2);
  ASSERT(get_square_row(G4) == 3);
  ASSERT(get_square_row(G5) == 4);
  ASSERT(get_square_row(G6) == 5);
  ASSERT(get_square_row(G7) == 6);
  ASSERT(get_square_row(G8) == 7);
  ASSERT(get_square_row(H1) == 0);
  ASSERT(get_square_row(H2) == 1);
  ASSERT(get_square_row(H3) == 2);
  ASSERT(get_square_row(H4) == 3);
  ASSERT(get_square_row(H5) == 4);
  ASSERT(get_square_row(H6) == 5);
  ASSERT(get_square_row(H7) == 6);
  ASSERT(get_square_row(H8) == 7);
}

{ /* get_square_col */
  ASSERT(get_square_col(A1) == 0);
  ASSERT(get_square_col(A2) == 0);
  ASSERT(get_square_col(A3) == 0);
  ASSERT(get_square_col(A4) == 0);
  ASSERT(get_square_col(A5) == 0);
  ASSERT(get_square_col(A6) == 0);
  ASSERT(get_square_col(A7) == 0);
  ASSERT(get_square_col(A8) == 0);
  ASSERT(get_square_col(B1) == 1);
  ASSERT(get_square_col(B2) == 1);
  ASSERT(get_square_col(B3) == 1);
  ASSERT(get_square_col(B4) == 1);
  ASSERT(get_square_col(B5) == 1);
  ASSERT(get_square_col(B6) == 1);
  ASSERT(get_square_col(B7) == 1);
  ASSERT(get_square_col(B8) == 1);
  ASSERT(get_square_col(C1) == 2);
  ASSERT(get_square_col(C2) == 2);
  ASSERT(get_square_col(C3) == 2);
  ASSERT(get_square_col(C4) == 2);
  ASSERT(get_square_col(C5) == 2);
  ASSERT(get_square_col(C6) == 2);
  ASSERT(get_square_col(C7) == 2);
  ASSERT(get_square_col(C8) == 2);
  ASSERT(get_square_col(D1) == 3);
  ASSERT(get_square_col(D2) == 3);
  ASSERT(get_square_col(D3) == 3);
  ASSERT(get_square_col(D4) == 3);
  ASSERT(get_square_col(D5) == 3);
  ASSERT(get_square_col(D6) == 3);
  ASSERT(get_square_col(D7) == 3);
  ASSERT(get_square_col(D8) == 3);
  ASSERT(get_square_col(E1) == 4);
  ASSERT(get_square_col(E2) == 4);
  ASSERT(get_square_col(E3) == 4);
  ASSERT(get_square_col(E4) == 4);
  ASSERT(get_square_col(E5) == 4);
  ASSERT(get_square_col(E6) == 4);
  ASSERT(get_square_col(E7) == 4);
  ASSERT(get_square_col(E8) == 4);
  ASSERT(get_square_col(F1) == 5);
  ASSERT(get_square_col(F2) == 5);
  ASSERT(get_square_col(F3) == 5);
  ASSERT(get_square_col(F4) == 5);
  ASSERT(get_square_col(F5) == 5);
  ASSERT(get_square_col(F6) == 5);
  ASSERT(get_square_col(F7) == 5);
  ASSERT(get_square_col(F8) == 5);
  ASSERT(get_square_col(G1) == 6);
  ASSERT(get_square_col(G2) == 6);
  ASSERT(get_square_col(G3) == 6);
  ASSERT(get_square_col(G4) == 6);
  ASSERT(get_square_col(G5) == 6);
  ASSERT(get_square_col(G6) == 6);
  ASSERT(get_square_col(G7) == 6);
  ASSERT(get_square_col(G8) == 6);
  ASSERT(get_square_col(H1) == 7);
  ASSERT(get_square_col(H2) == 7);
  ASSERT(get_square_col(H3) == 7);
  ASSERT(get_square_col(H4) == 7);
  ASSERT(get_square_col(H5) == 7);
  ASSERT(get_square_col(H6) == 7);
  ASSERT(get_square_col(H7) == 7);
  ASSERT(get_square_col(H8) == 7);
}

{ /* get_square_64 */
  ASSERT(get_square_64(A1) == 0);
  ASSERT(get_square_64(B1) == 1);
  ASSERT(get_square_64(C1) == 2);
  ASSERT(get_square_64(D1) == 3);
  ASSERT(get_square_64(E1) == 4);
  ASSERT(get_square_64(F1) == 5);
  ASSERT(get_square_64(G1) == 6);
  ASSERT(get_square_64(H1) == 7);
  ASSERT(get_square_64(A2) == 8);
  ASSERT(get_square_64(B2) == 9);
  ASSERT(get_square_64(C2) == 10);
  ASSERT(get_square_64(D2) == 11);
  ASSERT(get_square_64(E2) == 12);
  ASSERT(get_square_64(F2) == 13);
  ASSERT(get_square_64(G2) == 14);
  ASSERT(get_square_64(H2) == 15);
  ASSERT(get_square_64(A3) == 16);
  ASSERT(get_square_64(B3) == 17);
  ASSERT(get_square_64(C3) == 18);
  ASSERT(get_square_64(D3) == 19);
  ASSERT(get_square_64(E3) == 20);
  ASSERT(get_square_64(F3) == 21);
  ASSERT(get_square_64(G3) == 22);
  ASSERT(get_square_64(H3) == 23);
  ASSERT(get_square_64(A4) == 24);
  ASSERT(get_square_64(B4) == 25);
  ASSERT(get_square_64(C4) == 26);
  ASSERT(get_square_64(D4) == 27);
  ASSERT(get_square_64(E4) == 28);
  ASSERT(get_square_64(F4) == 29);
  ASSERT(get_square_64(G4) == 30);
  ASSERT(get_square_64(H4) == 31);
  ASSERT(get_square_64(A5) == 32);
  ASSERT(get_square_64(B5) == 33);
  ASSERT(get_square_64(C5) == 34);
  ASSERT(get_square_64(D5) == 35);
  ASSERT(get_square_64(E5) == 36);
  ASSERT(get_square_64(F5) == 37);
  ASSERT(get_square_64(G5) == 38);
  ASSERT(get_square_64(H5) == 39);
  ASSERT(get_square_64(A6) == 40);
  ASSERT(get_square_64(B6) == 41);
  ASSERT(get_square_64(C6) == 42);
  ASSERT(get_square_64(D6) == 43);
  ASSERT(get_square_64(E6) == 44);
  ASSERT(get_square_64(F6) == 45);
  ASSERT(get_square_64(G6) == 46);
  ASSERT(get_square_64(H6) == 47);
  ASSERT(get_square_64(A7) == 48);
  ASSERT(get_square_64(B7) == 49);
  ASSERT(get_square_64(C7) == 50);
  ASSERT(get_square_64(D7) == 51);
  ASSERT(get_square_64(E7) == 52);
  ASSERT(get_square_64(F7) == 53);
  ASSERT(get_square_64(G7) == 54);
  ASSERT(get_square_64(H7) == 55);
  ASSERT(get_square_64(A8) == 56);
  ASSERT(get_square_64(B8) == 57);
  ASSERT(get_square_64(C8) == 58);
  ASSERT(get_square_64(D8) == 59);
  ASSERT(get_square_64(E8) == 60);
  ASSERT(get_square_64(F8) == 61);
  ASSERT(get_square_64(G8) == 62);
  ASSERT(get_square_64(H8) == 63);
}

{ /* get_square_120 */
  ASSERT(get_square_120(0) == A1);
  ASSERT(get_square_120(1) == B1);
  ASSERT(get_square_120(2) == C1);
  ASSERT(get_square_120(3) == D1);
  ASSERT(get_square_120(4) == E1);
  ASSERT(get_square_120(5) == F1);
  ASSERT(get_square_120(6) == G1);
  ASSERT(get_square_120(7) == H1);
  ASSERT(get_square_120(8) == A2);
  ASSERT(get_square_120(9) == B2);
  ASSERT(get_square_120(10) == C2);
  ASSERT(get_square_120(11) == D2);
  ASSERT(get_square_120(12) == E2);
  ASSERT(get_square_120(13) == F2);
  ASSERT(get_square_120(14) == G2);
  ASSERT(get_square_120(15) == H2);
  ASSERT(get_square_120(16) == A3);
  ASSERT(get_square_120(17) == B3);
  ASSERT(get_square_120(18) == C3);
  ASSERT(get_square_120(19) == D3);
  ASSERT(get_square_120(20) == E3);
  ASSERT(get_square_120(21) == F3);
  ASSERT(get_square_120(22) == G3);
  ASSERT(get_square_120(23) == H3);
  ASSERT(get_square_120(24) == A4);
  ASSERT(get_square_120(25) == B4);
  ASSERT(get_square_120(26) == C4);
  ASSERT(get_square_120(27) == D4);
  ASSERT(get_square_120(28) == E4);
  ASSERT(get_square_120(29) == F4);
  ASSERT(get_square_120(30) == G4);
  ASSERT(get_square_120(31) == H4);
  ASSERT(get_square_120(32) == A5);
  ASSERT(get_square_120(33) == B5);
  ASSERT(get_square_120(34) == C5);
  ASSERT(get_square_120(35) == D5);
  ASSERT(get_square_120(36) == E5);
  ASSERT(get_square_120(37) == F5);
  ASSERT(get_square_120(38) == G5);
  ASSERT(get_square_120(39) == H5);
  ASSERT(get_square_120(40) == A6);
  ASSERT(get_square_120(41) == B6);
  ASSERT(get_square_120(42) == C6);
  ASSERT(get_square_120(43) == D6);
  ASSERT(get_square_120(44) == E6);
  ASSERT(get_square_120(45) == F6);
  ASSERT(get_square_120(46) == G6);
  ASSERT(get_square_120(47) == H6);
  ASSERT(get_square_120(48) == A7);
  ASSERT(get_square_120(49) == B7);
  ASSERT(get_square_120(50) == C7);
  ASSERT(get_square_120(51) == D7);
  ASSERT(get_square_120(52) == E7);
  ASSERT(get_square_120(53) == F7);
  ASSERT(get_square_120(54) == G7);
  ASSERT(get_square_120(55) == H7);
  ASSERT(get_square_120(56) == A8);
  ASSERT(get_square_120(57) == B8);
  ASSERT(get_square_120(58) == C8);
  ASSERT(get_square_120(59) == D8);
  ASSERT(get_square_120(60) == E8);
  ASSERT(get_square_120(61) == F8);
  ASSERT(get_square_120(62) == G8);
  ASSERT(get_square_120(63) == H8);
}

{ /* string_from_square */
  ASSERT(string_from_square(A1) == "a1");
  ASSERT(string_from_square(A2) == "a2");
  ASSERT(string_from_square(A3) == "a3");
  ASSERT(string_from_square(A4) == "a4");
  ASSERT(string_from_square(A5) == "a5");
  ASSERT(string_from_square(A6) == "a6");
  ASSERT(string_from_square(A7) == "a7");
  ASSERT(string_from_square(A8) == "a8");
  ASSERT(string_from_square(B1) == "b1");
  ASSERT(string_from_square(B2) == "b2");
  ASSERT(string_from_square(B3) == "b3");
  ASSERT(string_from_square(B4) == "b4");
  ASSERT(string_from_square(B5) == "b5");
  ASSERT(string_from_square(B6) == "b6");
  ASSERT(string_from_square(B7) == "b7");
  ASSERT(string_from_square(B8) == "b8");
  ASSERT(string_from_square(C1) == "c1");
  ASSERT(string_from_square(C2) == "c2");
  ASSERT(string_from_square(C3) == "c3");
  ASSERT(string_from_square(C4) == "c4");
  ASSERT(string_from_square(C5) == "c5");
  ASSERT(string_from_square(C6) == "c6");
  ASSERT(string_from_square(C7) == "c7");
  ASSERT(string_from_square(C8) == "c8");
  ASSERT(string_from_square(D1) == "d1");
  ASSERT(string_from_square(D2) == "d2");
  ASSERT(string_from_square(D3) == "d3");
  ASSERT(string_from_square(D4) == "d4");
  ASSERT(string_from_square(D5) == "d5");
  ASSERT(string_from_square(D6) == "d6");
  ASSERT(string_from_square(D7) == "d7");
  ASSERT(string_from_square(D8) == "d8");
  ASSERT(string_from_square(E1) == "e1");
  ASSERT(string_from_square(E2) == "e2");
  ASSERT(string_from_square(E3) == "e3");
  ASSERT(string_from_square(E4) == "e4");
  ASSERT(string_from_square(E5) == "e5");
  ASSERT(string_from_square(E6) == "e6");
  ASSERT(string_from_square(E7) == "e7");
  ASSERT(string_from_square(E8) == "e8");
  ASSERT(string_from_square(F1) == "f1");
  ASSERT(string_from_square(F2) == "f2");
  ASSERT(string_from_square(F3) == "f3");
  ASSERT(string_from_square(F4) == "f4");
  ASSERT(string_from_square(F5) == "f5");
  ASSERT(string_from_square(F6) == "f6");
  ASSERT(string_from_square(F7) == "f7");
  ASSERT(string_from_square(F8) == "f8");
  ASSERT(string_from_square(G1) == "g1");
  ASSERT(string_from_square(G2) == "g2");
  ASSERT(string_from_square(G3) == "g3");
  ASSERT(string_from_square(G4) == "g4");
  ASSERT(string_from_square(G5) == "g5");
  ASSERT(string_from_square(G6) == "g6");
  ASSERT(string_from_square(G7) == "g7");
  ASSERT(string_from_square(G8) == "g8");
  ASSERT(string_from_square(H1) == "h1");
  ASSERT(string_from_square(H2) == "h2");
  ASSERT(string_from_square(H3) == "h3");
  ASSERT(string_from_square(H4) == "h4");
  ASSERT(string_from_square(H5) == "h5");
  ASSERT(string_from_square(H6) == "h6");
  ASSERT(string_from_square(H7) == "h7");
  ASSERT(string_from_square(H8) == "h8");
  ASSERT(string_from_square(INVALID_SQUARE) == "-");
}
return 0;
}

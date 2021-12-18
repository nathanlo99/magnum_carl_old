
#include "board.hpp"
#include "evaluate.hpp"
#include "hash.hpp"
#include "move.hpp"
#include "opening_book.hpp"
#include "piece.hpp"

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <sstream>
#include <string>

Board::Board(const std::string &fen) noexcept {
  m_pieces.fill(INVALID_PIECE);
  m_num_pieces.fill(0);
  for (unsigned piece = 0; piece < 16; ++piece) {
    m_positions[piece].fill(INVALID_SQUARE);
  }

  // Part 1: The board state
  unsigned square_idx = A8; // 91
  const char *next_chr = fen.data();
  const char *const end_ptr = fen.data() + fen.size();
  while (*next_chr != ' ') {
    const char chr = *next_chr;
    if (chr == '/') {
      // To the next row
      ASSERT(square_idx % 10 == 9);
      square_idx -= 18;
    } else if ('1' <= chr && chr <= '8') {
      // A number indicates that number of empty squares
      const unsigned num_spaces = chr - '0';
      for (unsigned i = 0; i < num_spaces; ++i)
        m_pieces[square_idx + i] = INVALID_PIECE;
      square_idx += num_spaces;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    } else {
      // Otherwise, it better be a character corresponding to a valid piece
      const piece_t piece_idx = piece_from_char(chr);
      ASSERT(valid_square(square_idx));
      ASSERT(valid_piece(piece_idx));
      m_pieces[square_idx] = piece_idx;
      ASSERT_MSG(m_num_pieces[piece_idx] < MAX_PIECE_FREQ,
                 "Too many (%u) pieces of type %u", m_num_pieces[piece_idx],
                 piece_idx);
      m_positions[piece_idx][m_num_pieces[piece_idx]] = square_idx;
      m_num_pieces[piece_idx]++;
      square_idx++;
      ASSERT(square_idx % 10 == 9 || valid_square(square_idx));
    }
    next_chr++;
  }
  ASSERT_MSG(square_idx == 29,
             "Unexpected last square (%u) after parsing FEN board", square_idx);

  // Part 2: Side to move
  next_chr++;
  ASSERT_MSG(*next_chr == 'w' || *next_chr == 'b', "Invalid FEN side (%c)",
             *next_chr);
  m_side_to_move = (*next_chr == 'w') ? WHITE : BLACK;

  // Part 3: Castle state
  next_chr += 2;
  m_castle_state = 0;
  if (*next_chr == '-') {
    next_chr++;
  } else {
    while (*next_chr != ' ') {
      switch (*next_chr) {
      case 'K':
        m_castle_state |= WHITE_SHORT;
        break;
      case 'Q':
        m_castle_state |= WHITE_LONG;
        break;
      case 'k':
        m_castle_state |= BLACK_SHORT;
        break;
      case 'q':
        m_castle_state |= BLACK_LONG;
        break;
      default:
        ASSERT_MSG(
            0, "Invalid character (%c) in castling permission specifications",
            *next_chr);
      }
      next_chr++;
    }
  }
  ASSERT(*next_chr == ' ');

  // Part 4: En passant square
  next_chr++;
  if (*next_chr == '-') {
    m_en_passant = INVALID_SQUARE;
    next_chr += 1;
  } else {
    const unsigned row = *(next_chr + 1) - '1', col = *next_chr - 'a';
    ASSERT_IF(m_side_to_move == WHITE, row == RANK_6);
    ASSERT_IF(m_side_to_move == BLACK, row == RANK_3);
    m_en_passant = get_square_120_rc(row, col);
    const piece_t my_pawn = (m_side_to_move == WHITE) ? WHITE_PAWN : BLACK_PAWN;
    const int row_offset = (m_side_to_move == WHITE) ? -10 : +10;
    if (m_pieces[m_en_passant + row_offset - 1] != my_pawn &&
        m_pieces[m_en_passant + row_offset + 1] != my_pawn) {
      WARN("Elided en passant square");
      m_en_passant = INVALID_SQUARE;
    }
    next_chr += 2;
  }
  ASSERT(*next_chr == ' ');

  // Part 5: Half move counter
  m_fifty_move = 0;
  next_chr++;
  while (*next_chr != ' ') {
    ASSERT_MSG('0' <= *next_chr && *next_chr <= '9',
               "Invalid digit (%c) in half move counter", *next_chr);
    m_fifty_move = 10 * m_fifty_move + (*next_chr - '0');
    next_chr++;
  }
  ASSERT(*next_chr == ' ');

  // Part 6: Full move counter
  size_t full_move = 0;
  next_chr++;
  while (next_chr != end_ptr) {
    ASSERT_MSG('0' <= *next_chr && *next_chr <= '9',
               "Invalid digit (%c) in full move counter", *next_chr);
    full_move = 10 * full_move + (*next_chr - '0');
    next_chr++;
  }
  m_half_move = 2 * (full_move - 1) + m_side_to_move;
  ASSERT_MSG(next_chr == end_ptr, "FEN string too long");

  m_hash = compute_hash();
  validate_board();
}

void Board::validate_board() const noexcept {
#if defined(DEBUG)
  std::array<unsigned, 16> piece_count = {0};
  for (unsigned sq = 0; sq < 120; ++sq) {
    ASSERT_MSG(valid_piece(m_pieces[sq]) || m_pieces[sq] == INVALID_PIECE,
               "Piece %u at %u is neither valid nor INVALID_PIECE",
               m_pieces[sq], sq);
    piece_count[m_pieces[sq]]++;
  }
  ASSERT_MSG(m_num_pieces[WHITE_KING] == 1, "White has too few/many (%u) kings",
             m_num_pieces[WHITE_KING]);
  ASSERT_MSG(m_num_pieces[BLACK_KING] == 1, "Black has too few/many (%u) kings",
             m_num_pieces[BLACK_KING]);
  for (unsigned piece = 0; piece < 16; ++piece) {
    ASSERT_IF_MSG(!valid_piece(piece), m_num_pieces[piece] == 0,
                  "Invalid piece %u has non-zero count %u", piece,
                  m_num_pieces[piece]);
    const unsigned end = m_num_pieces[piece];
    ASSERT_IF_MSG(valid_piece(piece), m_num_pieces[piece] == piece_count[piece],
                  "Too few/many (%u) pieces of type %u, expected %u",
                  m_num_pieces[piece], piece, piece_count[piece]);
    ASSERT_MSG(end <= MAX_PIECE_FREQ, "Too many (%u) pieces of type %u", end,
               piece);
    for (unsigned num = 0; num < end; ++num) {
      const square_t sq = m_positions[piece][num];
      ASSERT_MSG(m_pieces[sq] == piece,
                 "m_positions[%u][%u] inconsistent with m_pieces[%u]", piece,
                 num, sq);
      for (unsigned compare_idx = num + 1; compare_idx < end; ++compare_idx) {
        ASSERT_MSG(m_positions[piece][num] != m_positions[piece][compare_idx],
                   "Repeated position of piece %u at indices %u and %u", piece,
                   num, compare_idx);
      }
    }
  }
  ASSERT_MSG(0 <= m_castle_state && m_castle_state < 16,
             "Castle state (%u) out of range", m_castle_state);
  ASSERT_MSG(valid_square(m_en_passant) || m_en_passant == INVALID_SQUARE,
             "En passant square (%u) not valid nor INVALID_SQUARE",
             m_en_passant);
  ASSERT_IF_MSG(m_side_to_move == BLACK && m_en_passant != INVALID_SQUARE,
                get_square_row(m_en_passant) == RANK_3,
                "En passant square (%s - %u) not on row 3 on black's turn",
                string_from_square(m_en_passant).c_str(), m_en_passant);
  ASSERT_IF_MSG(m_side_to_move == WHITE && m_en_passant != INVALID_SQUARE,
                get_square_row(m_en_passant) == RANK_6,
                "En passant square (%s - %u) not on row 6 on white's turn",
                string_from_square(m_en_passant).c_str(), m_en_passant);

  // Assert other king is not in check
  const piece_t king_piece =
      (m_side_to_move == BLACK) ? WHITE_KING : BLACK_KING;
  const square_t king_square = m_positions[king_piece][0];
  ASSERT_MSG(!square_attacked(king_square, m_side_to_move),
             "Other king on (%s) did not avoid check",
             string_from_square(king_square).c_str());

  // Assert that the total number of positions seen (including repetitions) is
  // the number of half moves played
  ASSERT_MSG(m_position_freq.size() == m_history.size(),
             "The total number of positions seen (%lu) did not match the "
             "number of half moves played (%lu)",
             m_position_freq.size(), m_history.size());
  std::unordered_multiset<hash_t> expected_position_freq;
  for (const history_t &entry : m_history) {
    expected_position_freq.insert(entry.hash);
  }
  ASSERT_MSG(expected_position_freq.size() == m_position_freq.size(),
             "Size of frequency map (%lu) did not match expected size (%lu)",
             m_position_freq.size(), expected_position_freq.size());
  ASSERT_MSG(expected_position_freq == m_position_freq,
             "Position frequency map did not match expected frequency map");
#endif
}

std::string Board::fen() const noexcept {
  std::stringstream result;
  validate_board();

  // Part 1. The board state
  unsigned square_idx = A8;
  unsigned blank_count = 0;
  while (square_idx != 29) {
    if (square_idx % 10 == 9) {
      if (blank_count != 0) {
        ASSERT_MSG(blank_count <= 8, "Too many (%u) blank squares in a row",
                   blank_count);
        result << (char)('0' + blank_count);
        blank_count = 0;
      }
      result << '/';
      square_idx -= 18;
    }
    const piece_t piece = m_pieces[square_idx];
    if (piece == INVALID_PIECE) {
      blank_count++;
    } else {
      // Piece will be valid as board was validated
      if (blank_count != 0) {
        result << (char)('0' + blank_count);
        blank_count = 0;
      }
      result << char_from_piece(piece);
    }
    square_idx++;
  }
  if (blank_count != 0) {
    result << (char)('0' + blank_count);
  }
  result << ' ';

  // Part 2: Side to move
  result << (m_side_to_move == WHITE ? 'w' : 'b') << ' ';

  // Part 3: Castle state
  if (m_castle_state == 0) {
    result << '-';
  } else {
    if (m_castle_state & WHITE_SHORT)
      result << 'K';
    if (m_castle_state & WHITE_LONG)
      result << 'Q';
    if (m_castle_state & BLACK_SHORT)
      result << 'k';
    if (m_castle_state & BLACK_LONG)
      result << 'q';
  }
  result << ' ';

  // Part 4: En passant square
  result << string_from_square(m_en_passant) << ' ';

  // Part 5: Half move counter
  result << m_fifty_move << ' ';

  // Part 6: Full move counter
  result << (m_half_move / 2 + 1);

  return result.str();
}

hash_t Board::compute_hash() const noexcept {
  validate_board();
  hash_t res = 0;
  for (unsigned sq = 0; sq < 120; ++sq) {
    const piece_t piece = m_pieces[sq];
    ASSERT_MSG(0 <= piece && piece < 16, "Out of range piece (%u) in square",
               piece);
    ASSERT_MSG(valid_piece(piece) || piece_hash[sq][piece] == 0,
               "Invalid piece (%u) had non-zero hash (%llu)", piece,
               piece_hash[sq][piece]);
    res ^= piece_hash[sq][piece];
  }
  res ^= castle_hash[m_castle_state];
  ASSERT_MSG(enpas_hash[INVALID_SQUARE] == 0,
             "Invalid square had non-zero hash (%llu)",
             enpas_hash[INVALID_SQUARE]);
  res ^= enpas_hash[m_en_passant];
  res ^= (m_side_to_move * side_hash);
  return res;
}

std::string Board::to_string(const int side) const noexcept {
  validate_board();
  std::stringstream result;
  result << "\n";
  if (side == WHITE) {
    result << "   +---+---+---+---+---+---+---+---+\n";
    for (int row = 7; row >= 0; --row) {
      const char row_name = '1' + row;
      result << " " << row_name << " |";
      for (int col = 0; col < 8; ++col) {
        const square_t square = get_square_120_rc(row, col);
        const piece_t piece_idx = m_pieces[square];
        result << " " << char_from_piece(piece_idx) << " |";
      }
      result << "\n";
      result << "   +---+---+---+---+---+---+---+---+\n";
    }
    result << "     a   b   c   d   e   f   g   h\n\n";
  } else {
    result << "   +---+---+---+---+---+---+---+---+\n";
    for (int row = 0; row <= 7; ++row) {
      const char row_name = '1' + row;
      result << " " << row_name << " |";
      for (int col = 7; col >= 0; --col) {
        const square_t square = get_square_120_rc(row, col);
        const piece_t piece_idx = m_pieces[square];
        result << " " << char_from_piece(piece_idx) << " |";
      }
      result << "\n";
      result << "   +---+---+---+---+---+---+---+---+\n";
    }
    result << "     h   g   f   e   d   c   b   a\n\n";
  }
  result << "TO MOVE : ";
  result << ((m_side_to_move == WHITE) ? "WHITE" : "BLACK") << "\n";
  result << "EN PASS : " << string_from_square(m_en_passant) << "\n";
  result << "FIFTY   : " << m_fifty_move << "\n";
  result << "MOVE#   : " << (m_half_move / 2) << "\n";
  result << "HALF#   : " << m_half_move << "\n";
  result << "HASH    : ";
  result << std::setw(16) << std::setfill('0') << std::hex << hash() << std::dec
         << "\n";
  result << "FEN     : " << fen() << "\n";
  // if (opening_book.query_all(*this).size() > 0) {
  //   result << "BOOK    : " << opening_book.book_moves_string(*this) << "\n";
  // }
  if (!m_history.empty()) {
    result << "LAST MV : " << algebraic_notation(m_history.back().move) << "\n";
  }
  // const int evaluation = static_evaluate_board(*this, WHITE);
  // result << "EVAL    : " << evaluation << "\n";
  return result.str();
}

std::ostream &operator<<(std::ostream &os, const Board &board) noexcept {
  return os << board.to_string();
}

bool Board::square_attacked(const square_t sq, const bool side) const noexcept {
  const piece_t king_piece = (side == WHITE) ? WHITE_KING : BLACK_KING,
                knight_piece = (side == WHITE) ? WHITE_KNIGHT : BLACK_KNIGHT,
                pawn_piece = (side == WHITE) ? WHITE_PAWN : BLACK_PAWN;
  const square_t king_square = m_positions[king_piece][0];

  if (valid_piece(m_pieces[sq]) && get_side(m_pieces[sq]) == side) {
    // WASSERT_MSG(get_side(m_pieces[sq]) != side,
    //             "Querying square attacked of own piece");
    return false;
  }

  // Diagonals
  const auto &diagonal_offsets = {-11, -9, 9, 11};
  for (const int offset : diagonal_offsets) {
    square_t cur_square = sq + offset;
    if (king_square == cur_square)
      return true;
    while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
      cur_square += offset;
    const piece_t cur_piece = m_pieces[cur_square];
    // If the square is invalid, then the piece is invalid thus not on our side
    if (get_side(cur_piece) == side && is_diag(cur_piece))
      return true;
  }

  // Orthogonals
  const auto &orthogonal_offsets = {-10, -1, 1, 10};
  for (const int offset : orthogonal_offsets) {
    square_t cur_square = sq + offset;
    if (king_square == cur_square)
      return true;
    while (valid_square(cur_square) && m_pieces[cur_square] == INVALID_PIECE)
      cur_square += offset;
    const piece_t cur_piece = m_pieces[cur_square];
    // If the square is invalid, then the piece is invalid thus not on our side
    if (get_side(cur_piece) == side && is_ortho(cur_piece))
      return true;
  }

  // Knights
  const auto &knight_offsets = {-21, -19, -12, -8, 8, 12, 19, 21};
  for (const int offset : knight_offsets)
    if (m_pieces[sq + offset] == knight_piece)
      return true;

  // Pawns
  const int offset1 = (side == WHITE) ? -9 : 9;
  const int offset2 = (side == WHITE) ? -11 : 11;
  if (m_pieces[sq + offset1] == pawn_piece ||
      m_pieces[sq + offset2] == pawn_piece)
    return true;

  return false;
}

bool Board::king_in_check() const noexcept {
  const piece_t king_piece =
      (m_side_to_move == WHITE) ? WHITE_KING : BLACK_KING;
  return square_attacked(m_positions[king_piece][0], !m_side_to_move);
}

bool Board::is_endgame() const noexcept {
  // Return true if neither side has a queen
  if (m_num_pieces[WHITE_QUEEN] == 0 && m_num_pieces[BLACK_QUEEN] == 0)
    return true;
  // Otherwise, if every side which has a queen
  const bool white_has_no_queen_or_one_minor =
      m_num_pieces[WHITE_QUEEN] == 0 ||
      (m_num_pieces[WHITE_ROOK] == 0 &&
       m_num_pieces[WHITE_KNIGHT] + m_num_pieces[WHITE_BISHOP] <= 1);
  const bool black_has_no_queen_or_one_minor =
      m_num_pieces[BLACK_QUEEN] == 0 ||
      (m_num_pieces[BLACK_ROOK] == 0 &&
       m_num_pieces[BLACK_KNIGHT] + m_num_pieces[BLACK_BISHOP] <= 1);
  return white_has_no_queen_or_one_minor && black_has_no_queen_or_one_minor;
}

inline void Board::remove_piece(const square_t sq) noexcept {
  INFO("Removing piece on square %s (%u)", string_from_square(sq).c_str(), sq);
  const piece_t piece = m_pieces[sq];
  ASSERT_MSG(valid_piece(piece), "Removing invalid piece (%u)!", piece);
  m_pieces[sq] = INVALID_PIECE;
  auto &piece_list = m_positions[piece];
  const auto &last_idx = piece_list.begin() + m_num_pieces[piece];
  const auto &this_idx = std::find(piece_list.begin(), last_idx, sq);
  ASSERT_MSG(this_idx != last_idx, "Removed piece (%d) not in piece_list",
             piece);
  m_num_pieces[piece]--;
  std::swap(*this_idx, *(last_idx - 1));
  m_hash ^= piece_hash[sq][piece];
}

inline void Board::add_piece(const square_t sq, const piece_t piece) noexcept {
  INFO("Adding piece (%c) to square %s", char_from_piece(piece),
       string_from_square(sq).c_str());
  ASSERT_MSG(valid_piece(piece), "Adding invalid piece!");
  ASSERT_MSG(m_pieces[sq] == INVALID_PIECE,
             "Adding piece would overwrite existing piece (%d)!", m_pieces[sq]);
  m_pieces[sq] = piece;
  m_positions[piece][m_num_pieces[piece]] = sq;
  m_num_pieces[piece]++;
  m_hash ^= piece_hash[sq][piece];
}

inline void Board::set_castle_state(const castle_t state) noexcept {
  INFO("Setting castle state to %u", state);
  m_hash ^= castle_hash[m_castle_state] ^ castle_hash[state];
  m_castle_state = state;
}

inline void Board::set_en_passant(const square_t sq) noexcept {
  INFO("Setting en passant to %s", string_from_square(sq).c_str());
  m_hash ^= enpas_hash[m_en_passant] ^ enpas_hash[sq];
  m_en_passant = sq;
}

inline void Board::move_piece(const square_t from, const square_t to) noexcept {
  INFO("Moving piece from %s to %s", string_from_square(from).c_str(),
       string_from_square(to).c_str());
  ASSERT_MSG(m_pieces[to] == INVALID_PIECE,
             "Attempted to move to occupied square");
  const piece_t piece = m_pieces[from];
  m_pieces[from] = INVALID_PIECE;
  m_pieces[to] = piece;
  auto &piece_list = m_positions[piece];
  const auto &last_idx = piece_list.begin() + m_num_pieces[piece];
  const auto &this_idx = std::find(piece_list.begin(), last_idx, from);
  ASSERT_MSG(this_idx != last_idx, "Moved piece not in piece_list");
  *this_idx = to;
  m_hash ^= piece_hash[from][piece] ^ piece_hash[to][piece];
}

inline void Board::update_castling(const square_t from,
                                   const square_t to) noexcept {
  INFO("Updating castling which affected %s", string_from_square(sq).c_str());
  m_hash ^= castle_hash[m_castle_state];
  if (from == E1 || from == A1 || to == E1 || to == A1)
    m_castle_state &= ~WHITE_LONG;
  if (from == E1 || from == H1 || to == E1 || to == H1)
    m_castle_state &= ~WHITE_SHORT;
  if (from == E8 || from == A8 || to == E8 || to == A8)
    m_castle_state &= ~BLACK_LONG;
  if (from == E8 || from == H8 || to == E8 || to == H8)
    m_castle_state &= ~BLACK_SHORT;
  m_hash ^= castle_hash[m_castle_state];
}

inline void Board::switch_colours() noexcept {
  INFO("Switching colours");
  m_side_to_move ^= 1;
  m_hash ^= side_hash;
}

bool Board::make_move(const move_t move) noexcept {
  INFO("======================================================================="
       "==============");
  const MoveFlag flag = move_flag(move);
  const square_t from = move_from(move), to = move_to(move);
  const piece_t moved = moved_piece(move);
  // INFO("%s", to_string().c_str());
  INFO("Making move from %s to %s", string_from_square(from).c_str(),
       string_from_square(to).c_str());
  INFO("Move flag is %s", string_from_flag(flag).c_str());
  INFO("Promoted: %d, Captured: %d", move_promoted(move), move_captured(move));
  const bool cur_side = m_side_to_move, other_side = !cur_side;

  ASSERT_MSG(get_side(moved) == cur_side,
             "Attempted to move other player's pieces: %s\n%s\n",
             string_from_move(move).c_str(), to_string().c_str());

  // Bookkeeping
  history_t entry;
  entry.move = move;
  entry.castle_state = m_castle_state;
  entry.en_passant = m_en_passant;
  entry.fifty_move = m_fifty_move;
  entry.hash = m_hash;
  m_history.push_back(entry);
  m_position_freq.insert(entry.hash);
  m_half_move++;

  update_castling(from, to);
  if (move_promoted(move)) {
    if (move_captured(move))
      remove_piece(to);
    add_piece(to, promoted_piece(move));
    remove_piece(from);
    set_en_passant(INVALID_SQUARE);
  } else if (move_castled(move)) {
    if (cur_side == WHITE) {
      if (flag == SHORT_CASTLE_MOVE) {
        move_piece(E1, G1);
        move_piece(H1, F1);
      } else {
        move_piece(E1, C1);
        move_piece(A1, D1);
      }
    } else {
      if (flag == SHORT_CASTLE_MOVE) {
        move_piece(E8, G8);
        move_piece(H8, F8);
      } else {
        move_piece(E8, C8);
        move_piece(A8, D8);
      }
    }
    set_en_passant(INVALID_SQUARE);
  } else {
    const square_t enpas_square = (cur_side == WHITE) ? (to - 10) : (to + 10);
    set_en_passant(INVALID_SQUARE);
    if (flag == QUIET_MOVE) {
      INFO("Handling quiet move");
      move_piece(from, to);
    } else if (flag == DOUBLE_PAWN_MOVE) {
      INFO("Handling double pawn move");
      move_piece(from, to);
      set_en_passant(enpas_square);
    } else if (flag == CAPTURE_MOVE) {
      INFO("Handling capture move");
      remove_piece(to);
      move_piece(from, to);
    } else if (flag == EN_PASSANT_MOVE) {
      INFO("Handling en-passant move");
      remove_piece(enpas_square);
      move_piece(from, to);
    }
  }
  if (move_captured(move) || is_pawn(moved_piece(move))) {
    m_fifty_move = 0;
    // TODO: Could clear the position_freq table here so long as we can get it
    // back when unmaking a move
  } else {
    m_fifty_move++;
  }

  switch_colours();
  const piece_t king_piece = (cur_side == WHITE) ? WHITE_KING : BLACK_KING;
  INFO("Is %s king attacked by %s?", (cur_side == WHITE) ? "white" : "black",
       (other_side == WHITE) ? "white" : "black");
  const bool valid = !square_attacked(m_positions[king_piece][0], other_side);
  INFO("%s king %s attacked", (cur_side == WHITE) ? "White" : "Black",
       valid ? "is not" : "is");
  if (valid) {
    validate_board();
    INFO("====================================================================="
         "================");
    return true;
  }
  INFO("======================================================================="
       "==============");
  return false;
}

void Board::unmake_move() noexcept {
  INFO("======================================================================="
       "==============");
  ASSERT_MSG(!m_history.empty(),
             "Trying to unmake move from starting position");
  const history_t entry = m_history.back();
  m_history.pop_back();
  const move_t move = entry.move;
  const hash_t last_hash = entry.hash;
  set_castle_state(entry.castle_state);
  set_en_passant(entry.en_passant);
  m_fifty_move = entry.fifty_move;
  ASSERT_MSG(m_half_move > 0, "Unmaking first move");
  m_half_move--;
  // We have to beat around the bush a little to avoid removing _all_ instances
  // of the position and to instead only remove one of them.
  m_position_freq.erase(m_position_freq.find(entry.hash));
  switch_colours();
  const bool cur_side = m_side_to_move;

  const MoveFlag flag = move_flag(move);
  const square_t from = move_from(move), to = move_to(move);
  INFO("Unmaking move from %s to %s", string_from_square(from).c_str(),
       string_from_square(to).c_str());
  INFO("Move flag is %s", string_from_flag(flag).c_str());
  INFO("Promoted: %d, Captured: %d", move_promoted(move), move_captured(move));

  if (move_promoted(move)) {
    remove_piece(to);
    add_piece(from, moved_piece(move));
    if (move_captured(move))
      add_piece(to, captured_piece(move));
  } else if (move_castled(move)) {
    if (cur_side == WHITE) {
      if (flag == SHORT_CASTLE_MOVE) {
        move_piece(G1, E1);
        move_piece(F1, H1);
      } else {
        move_piece(C1, E1);
        move_piece(D1, A1);
      }
    } else {
      if (flag == SHORT_CASTLE_MOVE) {
        move_piece(G8, E8);
        move_piece(F8, H8);
      } else {
        move_piece(C8, E8);
        move_piece(D8, A8);
      }
    }
  } else {
    move_piece(to, from);
    if (move_captured(move)) {
      const square_t en_pas_sq =
          (cur_side == WHITE) ? m_en_passant - 10 : m_en_passant + 10;
      const square_t captured_sq = (flag == CAPTURE_MOVE) ? to : en_pas_sq;
      add_piece(captured_sq, captured_piece(move));
    }
  }

  ASSERT_MSG(m_hash == last_hash, "Hash did not match history entry's hash");
  validate_board();
  INFO("======================================================================="
       "==============");
}

// Algebraic notation for a move
std::string Board::algebraic_notation(const move_t move) const {
  if (move_flag(move) == SHORT_CASTLE_MOVE)
    return "O-O";
  if (move_flag(move) == LONG_CASTLE_MOVE)
    return "O-O-O";

  std::string result;
  const piece_t moved_piece = ::moved_piece(move);
  const square_t from_square = move_from(move);
  const square_t to_square = move_to(move);

  const std::vector<move_t> move_list = legal_moves();
  std::vector<move_t> same_piece_and_target;
  std::copy_if(move_list.begin(), move_list.end(),
               std::back_inserter(same_piece_and_target),
               [to_square, moved_piece, move](const move_t list_move) {
                 return move != list_move && move_to(list_move) == to_square &&
                        ::moved_piece(list_move) == moved_piece;
               });

  const bool ambiguous = same_piece_and_target.size() > 0;
  bool ambiguous_file = false;
  bool ambiguous_rank = false;
  for (const move_t list_move : same_piece_and_target) {
    if (get_square_row(move_from(list_move)) == get_square_row(from_square))
      ambiguous_rank = true;
    if (get_square_col(move_from(list_move)) == get_square_col(from_square))
      ambiguous_file = true;
  }

  if (!is_pawn(moved_piece))
    result.push_back(char_from_piece(to_white(moved_piece)));

  if (!is_pawn(moved_piece) && ambiguous) {
    if (!ambiguous_file) {
      result.push_back('a' + get_square_col(from_square));
    } else if (!ambiguous_rank) {
      result.push_back('1' + get_square_row(from_square));
    } else {
      result.push_back('a' + get_square_row(from_square));
      result.push_back('1' + get_square_col(from_square));
    }
  }

  if (move_captured(move)) {
    if (is_pawn(moved_piece))
      result.push_back('a' + get_square_col(from_square));
    result.push_back('x');
  }

  result += string_from_square(to_square);

  if (move_promoted(move)) {
    result.push_back('=');
    result.push_back(char_from_piece(to_white(::promoted_piece(move))));
  }

  return result;
}

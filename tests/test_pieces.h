
#ifndef TEST_PIECES_H
#define TEST_PIECES_H

#include "assert.h"
#include "piece.h"

inline void test_pieces() {
  { /* is_valid_piece */
    { /* is_valid_piece true positives */
      ASSERT(is_valid_piece(WHITE_KING));
      ASSERT(is_valid_piece(WHITE_QUEEN));
      ASSERT(is_valid_piece(WHITE_ROOK));
      ASSERT(is_valid_piece(WHITE_BISHOP));
      ASSERT(is_valid_piece(WHITE_KNIGHT));
      ASSERT(is_valid_piece(WHITE_PAWN));
      ASSERT(is_valid_piece(BLACK_KING));
      ASSERT(is_valid_piece(BLACK_QUEEN));
      ASSERT(is_valid_piece(BLACK_ROOK));
      ASSERT(is_valid_piece(BLACK_BISHOP));
      ASSERT(is_valid_piece(BLACK_KNIGHT));
      ASSERT(is_valid_piece(BLACK_PAWN));
    }
    { /* is_valid_piece true negatives */
      ASSERT(!is_valid_piece(3));
      ASSERT(!is_valid_piece(7));
      ASSERT(!is_valid_piece(11));
      ASSERT(!is_valid_piece(15));
      ASSERT(!is_valid_piece(INVALID_PIECE));
    }
  }

  {
    { /* is_king true positives */
      ASSERT(is_king(WHITE_KING));
      ASSERT(is_king(BLACK_KING));
    }
    { /* is_king true negatives */
      ASSERT(!is_king(WHITE_QUEEN));
      ASSERT(!is_king(WHITE_ROOK));
      ASSERT(!is_king(WHITE_BISHOP));
      ASSERT(!is_king(WHITE_KNIGHT));
      ASSERT(!is_king(WHITE_PAWN));
      ASSERT(!is_king(BLACK_QUEEN));
      ASSERT(!is_king(BLACK_ROOK));
      ASSERT(!is_king(BLACK_BISHOP));
      ASSERT(!is_king(BLACK_KNIGHT));
      ASSERT(!is_king(BLACK_PAWN));
    }
  }

  { /* is_major */
    { /* is_major true positives */
      ASSERT(is_major(WHITE_QUEEN));
      ASSERT(is_major(WHITE_ROOK));
      ASSERT(is_major(BLACK_QUEEN));
      ASSERT(is_major(BLACK_ROOK));
    }
    { /* is_major true negatives */
      ASSERT(!is_major(WHITE_KING));
      ASSERT(!is_major(WHITE_BISHOP));
      ASSERT(!is_major(WHITE_KNIGHT));
      ASSERT(!is_major(WHITE_PAWN));
      ASSERT(!is_major(BLACK_KING));
      ASSERT(!is_major(BLACK_BISHOP));
      ASSERT(!is_major(BLACK_KNIGHT));
      ASSERT(!is_major(BLACK_PAWN));
    }
  }

  { /* is_minor */
    { /* is_minor true positives */
      ASSERT(is_minor(WHITE_BISHOP));
      ASSERT(is_minor(WHITE_KNIGHT));
      ASSERT(is_minor(BLACK_BISHOP));
      ASSERT(is_minor(BLACK_KNIGHT));
    }
    { /* is_minor true negatives */
      ASSERT(!is_minor(WHITE_KING));
      ASSERT(!is_minor(WHITE_QUEEN));
      ASSERT(!is_minor(WHITE_ROOK));
      ASSERT(!is_minor(WHITE_PAWN));
      ASSERT(!is_minor(BLACK_KING));
      ASSERT(!is_minor(BLACK_QUEEN));
      ASSERT(!is_minor(BLACK_ROOK));
      ASSERT(!is_minor(BLACK_PAWN));
    }
  }

  { /* is_pawn */
    { /* is_pawn true positives */
      ASSERT(is_pawn(WHITE_PAWN));
      ASSERT(is_pawn(BLACK_PAWN));
    }
    { /* is_pawn true negatives */
      ASSERT(!is_pawn(WHITE_KING));
      ASSERT(!is_pawn(WHITE_QUEEN));
      ASSERT(!is_pawn(WHITE_ROOK));
      ASSERT(!is_pawn(WHITE_BISHOP));
      ASSERT(!is_pawn(WHITE_KNIGHT));
      ASSERT(!is_pawn(BLACK_KING));
      ASSERT(!is_pawn(BLACK_QUEEN));
      ASSERT(!is_pawn(BLACK_ROOK));
      ASSERT(!is_pawn(BLACK_BISHOP));
      ASSERT(!is_pawn(BLACK_KNIGHT));
    }
  }

  { /* get_side */
    ASSERT(get_side(WHITE_KING) == 0);
    ASSERT(get_side(WHITE_QUEEN) == 0);
    ASSERT(get_side(WHITE_ROOK) == 0);
    ASSERT(get_side(WHITE_BISHOP) == 0);
    ASSERT(get_side(WHITE_KNIGHT) == 0);
    ASSERT(get_side(WHITE_PAWN) == 0);
    ASSERT(get_side(BLACK_KING) == 1);
    ASSERT(get_side(BLACK_QUEEN) == 1);
    ASSERT(get_side(BLACK_ROOK) == 1);
    ASSERT(get_side(BLACK_BISHOP) == 1);
    ASSERT(get_side(BLACK_KNIGHT) == 1);
    ASSERT(get_side(BLACK_PAWN) == 1);
  }
}

#endif /* end of include guard: TEST_PIECES_H */

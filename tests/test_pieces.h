
#ifndef TEST_PIECES_H
#define TEST_PIECES_H

#include "assert.h"
#include "piece.h"

inline void test_pieces() {
  { /* is_valid_piece */
    { /* is_valid_piece true positives */
      ASSERT(is_valid_piece(WK));
      ASSERT(is_valid_piece(WQ));
      ASSERT(is_valid_piece(WR));
      ASSERT(is_valid_piece(WB));
      ASSERT(is_valid_piece(WN));
      ASSERT(is_valid_piece(WP));
      ASSERT(is_valid_piece(BK));
      ASSERT(is_valid_piece(BQ));
      ASSERT(is_valid_piece(BR));
      ASSERT(is_valid_piece(BB));
      ASSERT(is_valid_piece(BN));
      ASSERT(is_valid_piece(BP));
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
      ASSERT(is_king(WK));
      ASSERT(is_king(BK));
    }
    { /* is_king true negatives */
      ASSERT(!is_king(WQ));
      ASSERT(!is_king(WR));
      ASSERT(!is_king(WB));
      ASSERT(!is_king(WN));
      ASSERT(!is_king(WP));
      ASSERT(!is_king(BQ));
      ASSERT(!is_king(BR));
      ASSERT(!is_king(BB));
      ASSERT(!is_king(BN));
      ASSERT(!is_king(BP));
    }
  }

  { /* is_major */
    { /* is_major true positives */
      ASSERT(is_major(WQ));
      ASSERT(is_major(WR));
      ASSERT(is_major(BQ));
      ASSERT(is_major(BR));
    }
    { /* is_major true negatives */
      ASSERT(!is_major(WK));
      ASSERT(!is_major(WB));
      ASSERT(!is_major(WN));
      ASSERT(!is_major(WP));
      ASSERT(!is_major(BK));
      ASSERT(!is_major(BB));
      ASSERT(!is_major(BN));
      ASSERT(!is_major(BP));
    }
  }

  { /* is_minor */
    { /* is_minor true positives */
      ASSERT(is_minor(WB));
      ASSERT(is_minor(WN));
      ASSERT(is_minor(BB));
      ASSERT(is_minor(BN));
    }
    { /* is_minor true negatives */
      ASSERT(!is_minor(WK));
      ASSERT(!is_minor(WQ));
      ASSERT(!is_minor(WR));
      ASSERT(!is_minor(WP));
      ASSERT(!is_minor(BK));
      ASSERT(!is_minor(BQ));
      ASSERT(!is_minor(BR));
      ASSERT(!is_minor(BP));
    }
  }

  { /* is_pawn */
    { /* is_pawn true positives */
      ASSERT(is_pawn(WP));
      ASSERT(is_pawn(BP));
    }
    { /* is_pawn true negatives */
      ASSERT(!is_pawn(WK));
      ASSERT(!is_pawn(WQ));
      ASSERT(!is_pawn(WR));
      ASSERT(!is_pawn(WB));
      ASSERT(!is_pawn(WN));
      ASSERT(!is_pawn(BK));
      ASSERT(!is_pawn(BQ));
      ASSERT(!is_pawn(BR));
      ASSERT(!is_pawn(BB));
      ASSERT(!is_pawn(BN));
    }
  }

  { /* get_side */
    ASSERT(get_side(WK) == 0);
    ASSERT(get_side(WQ) == 0);
    ASSERT(get_side(WR) == 0);
    ASSERT(get_side(WB) == 0);
    ASSERT(get_side(WN) == 0);
    ASSERT(get_side(WP) == 0);
    ASSERT(get_side(BK) == 1);
    ASSERT(get_side(BQ) == 1);
    ASSERT(get_side(BR) == 1);
    ASSERT(get_side(BB) == 1);
    ASSERT(get_side(BN) == 1);
    ASSERT(get_side(BP) == 1);
  }
}

#endif /* end of include guard: TEST_PIECES_H */

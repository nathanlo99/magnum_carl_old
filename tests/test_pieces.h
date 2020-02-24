
#ifndef TEST_PIECES_H
#define TEST_PIECES_H

#include "assert.h"
#include "piece.h"

inline constexpr int test_pieces() {
  { /* valid_piece */
    { /* valid_piece true positives */
      ASSERT(valid_piece(WHITE_KING));
      ASSERT(valid_piece(WHITE_QUEEN));
      ASSERT(valid_piece(WHITE_ROOK));
      ASSERT(valid_piece(WHITE_BISHOP));
      ASSERT(valid_piece(WHITE_KNIGHT));
      ASSERT(valid_piece(WHITE_PAWN));
      ASSERT(valid_piece(BLACK_KING));
      ASSERT(valid_piece(BLACK_QUEEN));
      ASSERT(valid_piece(BLACK_ROOK));
      ASSERT(valid_piece(BLACK_BISHOP));
      ASSERT(valid_piece(BLACK_KNIGHT));
      ASSERT(valid_piece(BLACK_PAWN));
    }
    { /* valid_piece true negatives */
      ASSERT(!valid_piece(3));
      ASSERT(!valid_piece(7));
      ASSERT(!valid_piece(11));
      ASSERT(!valid_piece(15));
      ASSERT(!valid_piece(INVALID_PIECE));
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

  { /* is_diag */
    { /* is_diag true positives */
      ASSERT(is_diag(WHITE_QUEEN));
      ASSERT(is_diag(WHITE_BISHOP));
      ASSERT(is_diag(BLACK_QUEEN));
      ASSERT(is_diag(BLACK_BISHOP));
    }
    { /* is_diag true negatives */
      ASSERT(!is_diag(WHITE_KING));
      ASSERT(!is_diag(WHITE_ROOK));
      ASSERT(!is_diag(WHITE_KNIGHT));
      ASSERT(!is_diag(WHITE_PAWN));
      ASSERT(!is_diag(BLACK_KING));
      ASSERT(!is_diag(BLACK_ROOK));
      ASSERT(!is_diag(BLACK_KNIGHT));
      ASSERT(!is_diag(BLACK_PAWN));
    }
  }

  { /* is_ortho */
    { /* is_ortho true positives */
      ASSERT(is_ortho(WHITE_QUEEN));
      ASSERT(is_ortho(WHITE_ROOK));
      ASSERT(is_ortho(BLACK_QUEEN));
      ASSERT(is_ortho(BLACK_ROOK));
    }
    { /* is_ortho true negatives */
      ASSERT(!is_ortho(WHITE_KING));
      ASSERT(!is_ortho(WHITE_BISHOP));
      ASSERT(!is_ortho(WHITE_KNIGHT));
      ASSERT(!is_ortho(WHITE_PAWN));
      ASSERT(!is_ortho(BLACK_KING));
      ASSERT(!is_ortho(BLACK_BISHOP));
      ASSERT(!is_ortho(BLACK_KNIGHT));
      ASSERT(!is_ortho(BLACK_PAWN));
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
  return 0;
}

#endif /* end of include guard: TEST_PIECES_H */

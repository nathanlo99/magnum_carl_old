
from collections import defaultdict

exceptions = defaultdict(list)

with open("references/book/exceptions.txt", "r") as f:
    tokens = [line.split() for line in f.readlines()]
    for game_num, move_idx, move in tokens:
        exceptions[int(game_num)].append((int(move_idx), move))

with open("references/book/processed_games.txt", "r") as f:
    games = f.readlines()
    game_idx = 0
    for game in games:
        game_idx += 1
        # Remove the last token: this is the result
        moves = game.split()[:-1]
        # Remove trailing pluses for checks
        moves = [move.replace("+", "").replace("#", "") for move in moves]

        # Handle the exceptions
        for idx, move in exceptions[game_idx]:
            if moves[idx] != move:
                print("Game", game_idx, "move", idx)
                print("Expected", move, "got", moves[idx])
                raise Exception
                break
            moves[idx] = move[0] + move[2:]
            # print("Replaced", move, "with", moves[idx])

        print(" ".join(moves))

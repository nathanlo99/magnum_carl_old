
with open("references/book/games.txt", "r") as f:
    # Remove the first line: this is the credit to Sebastian Lague
    games = f.readlines()[1:]
    for game in games:
        # Remove the last token: this is the result
        moves = game.split()[:-1]
        # Remove trailing pluses for checks
        moves = [move.replace("+", "") for move in moves]
        print(" ".join(moves))


import sys
lines = sys.stdin.readlines()

for line in lines:
    fen, *results = line.split("; ")
    for idx, result in enumerate(results):
        depth = idx + 1
        print(fen + "; " + str(depth) + "; " + result.strip())

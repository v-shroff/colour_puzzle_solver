#include <cstddef>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>

class puzzle_state {
public:
  void encode() {
    for (int x : state) {
      encoded += std::to_string(x) + ",";
    }
  }

  size_t tubeDepth;
  size_t numTubes;
  size_t cost = 0;
  size_t moves;
  std::vector<int> state;
  std::string encoded;

  explicit puzzle_state(size_t tubeDepth, size_t numTubes)
      : tubeDepth(tubeDepth), numTubes(numTubes),
        state(tubeDepth * numTubes, -1) {}

  bool operator<(const puzzle_state &rhs) const {
    return this->cost > rhs.cost;
  }
};

bool isSolved(puzzle_state &stateToCheck) {
  for (size_t c = 0; c < stateToCheck.numTubes; ++c) {
    int firstInTube = stateToCheck.state[c * stateToCheck.tubeDepth];
    for (size_t i = c * stateToCheck.tubeDepth;
         i < (c + 1) * stateToCheck.tubeDepth; ++i) {
      if (stateToCheck.state[i] != firstInTube)
        return false;
    }
  }
  return true;
}

int getHscore(puzzle_state &state) {
  int hScore = 0;
  for (size_t t = 0; t < state.numTubes; ++t) {
    for (size_t d = 1; d < state.tubeDepth; ++d) {
      int above = state.state[t * state.tubeDepth + d - 1];
      int curr = state.state[t * state.tubeDepth + d];
      if (curr != above && above != -1) {
        ++hScore;
      }
    }
  }
  return hScore;
}

int main(int argc, char *argv[]) {
  std::priority_queue<puzzle_state> nodes;

  puzzle_state startingState(4, 8);
  startingState.state = {
      1,  2,  3,  4,  // tube 0
      5,  6,  1,  2,  // tube 1
      3,  4,  5,  6,  // tube 2
      6,  5,  4,  3,  // tube 3
      2,  1,  6,  5,  // tube 4
      4,  3,  2,  1,  // tube 5
      -1, -1, -1, -1, // tube 6
      -1, -1, -1, -1  // tube 7
  };
  startingState.moves = 0;
  startingState.cost = startingState.moves + getHscore(startingState);
  startingState.encode();
  nodes.push(startingState);

  std::unordered_map<std::string, std::string> parents;
  bool solved = false;

  while (!solved) {
    puzzle_state currentBest = nodes.top();
    nodes.pop();

    for (size_t tube = 0; tube < currentBest.numTubes; ++tube) {
      int toMove = -9;
      size_t moveIdx = 0;

      for (size_t d = 0; d < currentBest.tubeDepth; ++d) {
        size_t i = tube * currentBest.tubeDepth + d;
        if (currentBest.state[i] != -1) {
          toMove = currentBest.state[i];
          moveIdx = i;
          break;
        }
      }

      if (toMove == -9)
        continue;

      for (size_t dstTube = 0; dstTube < currentBest.numTubes; ++dstTube) {
        if (dstTube == tube)
          continue;

        int dstIdx = -1;
        for (size_t d = 0; d < currentBest.tubeDepth; ++d) {
          size_t i = dstTube * currentBest.tubeDepth + d;
          if (currentBest.state[i] == -1)
            dstIdx = i;
          else
            break;
        }

        if (dstIdx == -1)
          continue;

        bool tubeEmpty = true;
        for (size_t d = 0; d < currentBest.tubeDepth; ++d) {
          if (currentBest.state[dstTube * currentBest.tubeDepth + d] != -1) {
            tubeEmpty = false;
            break;
          }
        }

        size_t belowIdx = dstIdx + 1;
        bool colorMatch = (belowIdx < (dstTube + 1) * currentBest.tubeDepth) &&
                          (currentBest.state[belowIdx] == toMove);

        if (!tubeEmpty && !colorMatch)
          continue;

        puzzle_state move(currentBest.tubeDepth, currentBest.numTubes);
        move.state = currentBest.state;
        move.state[dstIdx] = toMove;
        move.state[moveIdx] = -1;

        if (isSolved(move)) {
          for (size_t d = 0; d < move.tubeDepth; ++d) {
            for (size_t t = d; t < move.state.size(); t += move.tubeDepth) {
              std::cout << move.state[t] << ",";
            }
            std::cout << std::endl;
          }
          solved = true;
          break;
        }
        move.encode();
        if (parents[move.encoded].empty()) {
          move.moves = currentBest.moves + 1;
          move.cost = getHscore(move) + move.moves;
          parents[move.encoded] = currentBest.encoded;
          nodes.push(move);
        }
      }
    }
  }

  return 0;
}

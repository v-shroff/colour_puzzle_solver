#include <algorithm>
#include <cstddef>
#include <iostream>
#include <queue>
#include <sstream>
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
int getHscore(const puzzle_state &state) {
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
//
// std::vector<puzzle_state> generateMoves(const puzzle_state &state) {
//         std::vector<puzzle_state> possibleMoves;
//
//     for (size_t tube = 0; tube < state.numTubes; ++tube) {
//     }
//
//
//
// return possibleMoves;
// }
//
int main(int argc, char *argv[]) {
  std::priority_queue<puzzle_state> nodes;

  puzzle_state startingState(4, 14);
  startingState.state = {
      1,  2,  3,  4,  // tube 0
      5,  6,  7,  8,  // tube 1
      4,  9,  7,  7,  // tube 2
      10, 1,  11, 9,  // tube 3
      1,  5,  8,  3,  // tube 4
      2,  6,  12, 8,  // tube 5
      5,  6,  5,  2,  // tube 6
      12, 2,  12, 11, // tube 7
      10, 6,  4,  4,  // tube 8
      3,  9,  9,  1,  // tube 9
      3,  7,  10, 11, // tube 10
      11, 8,  12, 10, // tube 11
      -1, -1, -1, -1, // tube 12 (empty)
      -1, -1, -1, -1  // tube 13 (empty)
  };
  startingState.moves = 0;
  startingState.cost = startingState.moves + getHscore(startingState);
  startingState.encode();
  nodes.push(startingState);

  std::unordered_map<std::string, std::string> parents;
  bool solved = false;
  std::string solvedEncoded;

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
          move.encode();
          parents[move.encoded] = currentBest.encoded;
          solvedEncoded = move.encoded;
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

  auto decodeState = [](const std::string &encoded, size_t tubeDepth,
                        size_t numTubes) {
    std::vector<int> state;
    std::stringstream ss(encoded);
    std::string token;
    while (std::getline(ss, token, ',')) {
      if (!token.empty())
        state.push_back(std::stoi(token));
    }
    return state;
  };

  std::vector<std::string> path;
  std::string current = solvedEncoded;
  while (!current.empty()) {
    path.push_back(current);
    current = parents[current];
  }
  std::reverse(path.begin(), path.end());

  std::cout << "Solved in " << path.size() - 1 << " moves\n\n";
  for (size_t step = 0; step < path.size(); ++step) {
    auto state = decodeState(path[step], startingState.tubeDepth,
                             startingState.numTubes);
    std::cout << "Step " << step << ":\n";
    for (size_t t = 0; t < startingState.numTubes; ++t) {
      std::cout << "  Tube " << t << ": ";
      for (size_t d = 0; d < startingState.tubeDepth; ++d) {
        int val = state[t * startingState.tubeDepth + d];
        if (val == -1)
          std::cout << "_ ";
        else
          std::cout << val << " ";
      }
      std::cout << "\n";
    }
    std::cout << "\n";
  }

  return 0;
}

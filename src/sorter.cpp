#include <cstddef>
#include <iostream>
#include <queue>
#include <string>
#include <unordered_map>
#include <vector>
class puzzle_state {
  // a state is defined as a M x N array with -1 used to represent an empty
  // space, and an integer used to represent a colour.
public:
  void encode() {
    for (int x : state) {
      encoded += std::to_string(x) + ",";
    }
  }

  size_t rows;
  size_t col;
  size_t cost = 0;
  size_t moves;
  std::vector<int> state;
  std::string encoded;
  explicit puzzle_state(size_t rows, size_t col)
      : rows(rows), col(col), state(rows * col, -1) {}
  bool operator<(const puzzle_state &rhs) const {
    return this->cost > rhs.cost;
  }

private:
};

bool isSolved(
    puzzle_state
        &stateToCheck) { // a puzzle is considered solved if each tube is filled
                         // with only one type of ball. So just check each one
  for (size_t c = 0; c < stateToCheck.col; ++c) {
    int firstInTube = stateToCheck.state[c];
    for (size_t i = c; i < stateToCheck.state.size(); i += stateToCheck.rows) {
      // this traverses the things column by column
      if (stateToCheck.state[i] != firstInTube)
        return false;
    }
  }
  return true;
}

int getHscore(puzzle_state &state) {
  int hScore = 0;
  for (size_t c = 0; c < state.col; ++c) {
    for (size_t i = c + state.rows; i < state.state.size(); i += state.rows) {
      if ((state.state[i] != state.state[i - state.rows]) &&
          state.state[i - state.rows] != -1)
      // if the two on top of each other match (ie
      // no transition) do nothing else hScore ++
      {
        ++hScore;
      }
    }
  }
  return hScore;
}

int main(int argc, char *argv[]) {
  std::priority_queue<puzzle_state> nodes;
  puzzle_state startingState(3, 3);
  startingState.state = {1, -1, -1, 2, -1, 2, 2, 1, 1};
  startingState.moves = 0;
  startingState.cost = startingState.moves + getHscore(startingState);
  startingState.encode();
  nodes.push(startingState);
  std::unordered_map<std::string, std::string> parents;
  bool solved = false;
  while (!solved) {
    puzzle_state currentBest = nodes.top();
    nodes.pop();
    for (size_t col = 0; col < currentBest.col; ++col) {
      int toMove = -9;    // fix this cuz its gonna cause problems
      size_t moveIdx = 0; // also fix this
      for (size_t i = col; i < currentBest.state.size();
           i += currentBest.rows) {
        if (currentBest.state[i] != -1) {
          toMove = currentBest.state[i];
          moveIdx = i;
          break;
        }
      }
      for (size_t checkCol = 0; checkCol < currentBest.col; ++checkCol) {
        if (checkCol != col) {
          for (size_t depth = checkCol + currentBest.rows;
               depth < currentBest.state.size(); depth += currentBest.rows) {
            if (((currentBest.state[depth] == toMove) &&
                 currentBest.state[depth - currentBest.rows] == -1) ||) {
              // we have a new state to add to the queue
              puzzle_state move(currentBest.rows, currentBest.col);
              move.state = currentBest.state;
              move.state[depth - move.rows] = toMove;
              move.state[moveIdx] = -1;
              if (isSolved(move)) {
                std::cout << move.state[0] << "," << move.state[1] << ","
                          << move.state[2] << "," << std::endl
                          << move.state[3] << "," << move.state[4] << ","
                          << move.state[5] << "," << std::endl
                          << move.state[6] << "," << move.state[7] << ","
                          << move.state[8] << "," << std::endl;
                solved = true;
                break;
              }
              move.encode();
              if (parents[move.encoded]
                      .empty()) { // if we have never found it before, then add
                                  // it in there and give it a parent
                move.moves = currentBest.moves + 1;
                move.cost = getHscore(move) + move.moves;
                parents[move.encoded] = currentBest.encoded;
                nodes.push(move);
              }
            }
          }
        }
      }
    }
    // dequeue current best node, put it in narnia since we have a hashed
    // version of it already. At some point I should make it a heap alloc thing
    // and then free it
  }

  return 0;
}

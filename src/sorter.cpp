#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <iostream>
#include <iterator>
#include <ostream>
#include <queue>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

struct state {
  state(std::vector<uint32_t> board) : board(board) {}
  std::vector<uint32_t> board; // this is the board state
  bool operator==(const state &rhs) const { return this->board == rhs.board; }
};

namespace std {
template <> struct hash<state> {
  std::size_t operator()(const state &p) const noexcept {
    if (p.board.empty())
      return 0;

    std::string_view bytes(reinterpret_cast<const char *>(p.board.data()),
                           p.board.size() * sizeof(uint32_t));

    return std::hash<std::string_view>{}(bytes);
  }
};
} // namespace std

struct boardState {
  size_t g; // this is the current move count (depth)
  int parentIdx;
  bool isClosed;

  boardState(size_t g, size_t parentIdx) : g(g), parentIdx(parentIdx) {}
};

bool isSolved(state &board, size_t numTubes, size_t tubeDepth) {
  for (size_t tube = 0; tube < numTubes; ++tube) {
    int firstInTube = static_cast<int>(board.board[tube * tubeDepth]);
    for (size_t depth = 0; depth < tubeDepth; ++depth) {
      if (static_cast<int>(board.board[tube * tubeDepth + depth]) !=
          firstInTube)
        return false;
    }
  }
  return true;
}
int getH(const state &board, size_t numTubes, size_t tubeDepth) {
  int hScore = 0;
  for (size_t t = 0; t < numTubes; ++t) {
    // find the bottom-most non-empty slot
    int bottomColor = 0;
    for (int d = tubeDepth - 1; d >= 0; --d) {
      int c = board.board[t * tubeDepth + d];
      if (c != 0) {
        bottomColor = c;
        break;
      }
    }
    // count balls in this tube that don't match the bottom color
    for (size_t d = 0; d < tubeDepth; ++d) {
      int c = board.board[t * tubeDepth + d];
      if (c != 0 && c != bottomColor)
        ++hScore;
    }
  }
  return hScore;
}
// int getH(const state &board, size_t numTubes, size_t tubeDepth) { // this is
// basically just dikstras or bfs - leads to a total of like 3million explored
// nodes
//   int hScore = 0;
//   for (size_t t = 0; t < numTubes; ++t) {
//     for (size_t d = 1; d < tubeDepth; ++d) {
//       int above = static_cast<int>(board.board[t * tubeDepth + d - 1]);
//       int curr = static_cast<int>(board.board[t * tubeDepth + d]);
//       if (curr != above && above != 0) {
//         ++hScore;
//       }
//     }
//   }
//   return hScore;
// }
bool generateValidMoves(
    const size_t best, const state board, size_t numTubes, size_t tubeDepth,
    std::vector<state> &stateSpace, std::vector<boardState> &metadata,
    std::unordered_map<state, int> &closedSet, // state array, key
    std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                        std::greater<std::pair<int, int>>>
        &openSet // fscore, key

) { // this function takes a valid map, creates valid move combinations, and
    // then appends them to the nessecary thingies maybe its too much idk
  state stateCpy = board;
  // std::cout << "inside valid moves top \n";
  for (size_t currTube = 0; currTube < numTubes;
       ++currTube) { // we need to check the first movable ball in each tube,
                     // lets go thru them 1 by 1
                     //
    // std::cout << "checking tubes " << currTube << std::endl;
    size_t depth = 0;
    int colour = 0;
    // now that we're in tube currTube, find the first avilabale ball
    for (; depth < tubeDepth; ++depth) {
      if (static_cast<int>(board.board[currTube * tubeDepth + depth]) != 0) {
        colour = static_cast<int>(board.board[currTube * tubeDepth + depth]);
        break;
      }
    }
    // std::cout << "found a colour " << colour << " at position " << depth
    //   << std::endl;
    if (colour == 0)
      continue; // if this tube has no valid moves, skip it
    //"depth" is the first available ball, lets iterate through the other tubes
    // to see where we can put it
    for (size_t placementTube = 0; placementTube < numTubes; ++placementTube) {
      if (placementTube == currTube)
        continue;
      // we are now checking a destination tube to see if we can put a ball in
      // it. Start from the bottom and then stop when we see an empty tube or a
      // tube in which there is a 0 and then the starting index colour
      int moveDepth = -1;
      for (size_t mvDepth = 0; mvDepth < tubeDepth; ++mvDepth) {
        if (static_cast<int>(
                board.board[placementTube * tubeDepth + mvDepth]) == 0) {
          moveDepth = mvDepth;
        }
      }
      if (moveDepth == -1 ||
          (moveDepth != static_cast<int>((tubeDepth - 1)) &&
           static_cast<int>(board.board.at(placementTube * tubeDepth +
                                           moveDepth + 1)) != colour))
        continue;
      // now we have a placment tube and a placment depth
      // Lets generate the state (we cant use emplace because we need to check
      // if it exists or not)
      // std::cout << "valid move at " << placementTube << ", " << moveDepth
      //  << std::endl;
      std::swap(stateCpy.board[currTube * tubeDepth + depth],
                stateCpy.board[placementTube * tubeDepth + moveDepth]);
      if (closedSet.emplace(stateCpy, stateSpace.size()).second) {
        // this means the swap worked and was placed in the thingie so were all
        // good
        stateSpace.push_back(stateCpy);
        metadata.push_back(boardState(metadata[best].g + 1, best));
        openSet.push(
            std::pair<int, int>{metadata[std::size(metadata) - 1].g +
                                    3 * getH(stateCpy, numTubes, tubeDepth),
                                std::size(stateSpace) - 1});

        // std::cout << "found one state\n" << std::endl;
        if (isSolved(stateCpy, numTubes, tubeDepth)) {
          return true;
        }
      }
      stateCpy = board;
    }
  }
  return false;
}

int main(int argc, char *argv[]) {
  std::vector<state> stateSpace;
  std::vector<boardState> metadata;
  std::unordered_map<state, int> closedSet; // state array, key
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      openSet; // fscore, key
  stateSpace.reserve(20000);
  metadata.reserve(20000);
  closedSet.reserve(20000);
  size_t tubeDepth = 5;
  size_t numTubes = 16;

  stateSpace.emplace_back(std::vector<uint32_t>{
      1,  2,  3,  4,  5,  // tube 0
      6,  7,  8,  9,  10, // tube 1
      11, 12, 13, 14, 1,  // tube 2
      2,  3,  4,  5,  6,  // tube 3
      7,  8,  9,  10, 11, // tube 4
      12, 13, 14, 1,  2,  // tube 5
      3,  4,  5,  6,  7,  // tube 6
      8,  9,  10, 11, 12, // tube 7
      13, 14, 1,  2,  3,  // tube 8
      4,  5,  6,  7,  8,  // tube 9
      9,  10, 11, 12, 13, // tube 10
      14, 1,  2,  3,  4,  // tube 11
      5,  6,  7,  8,  9,  // tube 12
      10, 11, 12, 13, 14, // tube 13
      0,  0,  0,  0,  0,  // tube 14 (empty)
      0,  0,  0,  0,  0   // tube 15 (empty)
  });
  // stateSpace.emplace_back(std::vector<int>{
  //     1, 2, 2,   // tube 1
  //     0, 0, 1, // tube 2
  //     0, 2, 1   // tube 3
  // });
  //
  // stateSpace.emplace_back(std::vector<uint32_t>{
  //     1,  2, 3,  4,  // tube 0
  //     5,  6, 7,  8,  // tube 1
  //     4,  9, 7,  7,  // tube 2
  //     10, 1, 11, 9,  // tube 3
  //     1,  5, 8,  3,  // tube 4
  //     2,  6, 12, 8,  // tube 5
  //     5,  6, 5,  2,  // tube 6
  //     12, 2, 12, 11, // tube 7
  //     10, 6, 4,  4,  // tube 8
  //     3,  9, 9,  1,  // tube 9
  //     3,  7, 10, 11, // tube 10
  //     11, 8, 12, 10, // tube 11
  //     0,  0, 0,  0,  // tube 12 (empty)
  //     0,  0, 0,  0   // tube 13 (empty)
  //
  // });

  metadata.emplace_back(0, 0);
  closedSet[stateSpace[0]] = 0;
  openSet.emplace(std::pair<int, int>{
      metadata[0].g + getH(stateSpace[0].board, numTubes, tubeDepth), 0});
  bool solved = isSolved(stateSpace[0], numTubes, tubeDepth);
  int solvedKey = 0;
  while (!solved) {
    std::pair<int, int> currBest = openSet.top();
    openSet.pop();
    if (generateValidMoves(currBest.second, stateSpace[currBest.second],
                           numTubes, tubeDepth, stateSpace, metadata, closedSet,
                           openSet)) {
      solved = true;
      solvedKey = stateSpace.size() - 1;
    }
  }

  int numMoves = 0;
  while (solvedKey != 0) {
    for (size_t tube = 0; tube < numTubes; ++tube) {
      for (size_t depth = 0; depth < tubeDepth; ++depth) {
        std::cout << stateSpace[solvedKey].board[tube * tubeDepth + depth]
                  << " ,";
      }
      std::cout << "Tube " << tube << std::endl;
    }
    ++numMoves;
    std::cout << std::endl;
    solvedKey = metadata[solvedKey].parentIdx;
  }
  std::cout << "Explored " << closedSet.size() << " states" << std::endl
            << std::endl;
  std::cout << "Solved in " << numMoves << " moves" << std::endl;
  return 0;
}

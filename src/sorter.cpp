#include <algorithm>
#include <cstddef>
#include <deque>
#include <functional>
#include <iterator>
#include <queue>
#include <string_view>
#include <unordered_map>
#include <utility>
#include <vector>

struct state {
  state(std::vector<int> board) : board(board) {}
  std::vector<int> board; // this is the board state
  bool operator==(const state &rhs) const { return this->board == rhs.board; }
};

namespace std {
template <> struct hash<state> {
  std::size_t operator()(const state &p) const noexcept {
    if (p.board.empty())
      return 0;

    std::string_view bytes(reinterpret_cast<const char *>(p.board.data()),
                           p.board.size() * sizeof(int));

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
    int firstInTube = board.board[tube * tubeDepth];
    for (size_t depth = 0; depth < tubeDepth; ++depth) {
      if (board.board[tube * tubeDepth + depth] != firstInTube)
        return false;
    }
  }
  return true;
}

int getH(const state &board, size_t numTubes, size_t tubeDepth) {
  int hScore = 0;
  for (size_t t = 0; t < numTubes; ++t) {
    for (size_t d = 1; d < tubeDepth; ++d) {
      int above = board.board[t * tubeDepth + d - 1];
      int curr = board.board[t * tubeDepth + d];
      if (curr != above && above != -1) {
        ++hScore;
      }
    }
  }
  return hScore;
}
bool generateValidMoves(
    const size_t best, const state &board, size_t numTubes, size_t tubeDepth,
    std::deque<state> &stateSpace, std::deque<boardState> &metadata,
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
    int colour = -1;
    // now that we're in tube currTube, find the first avilabale ball
    for (; depth < tubeDepth; ++depth) {
      if (board.board[currTube * tubeDepth + depth] != -1) {
        colour = board.board[currTube * tubeDepth + depth];
        break;
      }
    }
    // std::cout << "found a colour " << colour << " at position " << depth
    //   << std::endl;
    if (colour == -1)
      continue; // if this tube has no valid moves, skip it
    //"depth" is the first available ball, lets iterate through the other tubes
    // to see where we can put it
    for (size_t placementTube = 0; placementTube < numTubes; ++placementTube) {
      if (placementTube == currTube)
        continue;
      // we are now checking a destination tube to see if we can put a ball in
      // it. Start from the bottom and then stop when we see an empty tube or a
      // tube in which there is a -1 and then the starting index colour
      int moveDepth = -1;
      for (size_t mvDepth = 0; mvDepth < tubeDepth; ++mvDepth) {
        if (board.board[placementTube * tubeDepth + mvDepth] == -1) {
          moveDepth = mvDepth;
        }
      }
      if (moveDepth == -1 ||
          (moveDepth != static_cast<int>((tubeDepth - 1)) &&
           board.board.at(placementTube * tubeDepth + moveDepth + 1) != colour))
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
                                    getH(stateCpy, numTubes, tubeDepth),
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
  std::deque<state> stateSpace;
  std::deque<boardState> metadata;
  std::unordered_map<state, int> closedSet; // state array, key
  std::priority_queue<std::pair<int, int>, std::vector<std::pair<int, int>>,
                      std::greater<std::pair<int, int>>>
      openSet; // fscore, key
  size_t tubeDepth = 4;
  size_t numTubes = 14;
  // stateSpace.emplace_back(std::vector<int>{
  //     1, 2, 2,   // tube 1
  //     -1, -1, 1, // tube 2
  //     -1, 2, 1   // tube 3
  // });
  //
  stateSpace.emplace_back(std::vector<int>{
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

  });
  metadata.emplace_back(0, 0);
  closedSet[stateSpace[0]] = 0;
  openSet.emplace(std::pair<int, int>{
      metadata[0].g + getH(stateSpace[0].board, numTubes, tubeDepth), 0});
  bool solved = isSolved(stateSpace[0], numTubes, tubeDepth);

  while (!solved) {
    std::pair<int, int> currBest = openSet.top();
    openSet.pop();
    if (generateValidMoves(currBest.second, stateSpace[currBest.second],
                           numTubes, tubeDepth, stateSpace, metadata, closedSet,
                           openSet))
      return 1;
  }

  return 0;
}

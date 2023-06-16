#pragma once
#include "../state/state.hpp"
//self-added
int minimax(State *state, int depth, int p, int d);
/**
 * @brief Policy class for random policy, 
 * your policy class should have get_move method
 */
class Minimax{
public:
  static Move get_move(State *state, int depth);
//   void minimax(State *state, int depth, int p, int d, std::vector<Move> *action);
};
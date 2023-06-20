#pragma once
#include "../state/state.hpp"


/**
 * @brief Policy class for random policy, 
 * your policy class should have get_move method
 */
class MCTS{
  // friend class Random;
public:
  static Move get_move(State *state, int depth);

  static double get_UCB(Node* node);

  static Node MCts(Node* root);
  static Node TreePolicy(Node* node); //selection
  static Node BestChild(Node* node, bool is_explore);
  static Node Expand(Node* node);  //for expansion
  static int DefaultPolicy(Node* node); //for simulation
  static void BackUp(Node* node, int reward); //for propagation
};
#include <cstdlib>

#include "../state/state.hpp"
#include "./mcts.hpp"

/*
references:
https://zhuanlan.zhihu.com/p/30458774
https://medium.com/@ishaan.gupta0401/monte-carlo-tree-search-application-on-chess-5573fc0efb75
https://link.springer.com/article/10.1007/s10462-022-10228-y

selection: if(not_all_expand) expand, else BestChild()
expansion: randomly choose a *unwalked* node and add it to tree
simulation: play the game by *random* policy
backpropagate: return the reward(value?), and update Q and N
*/

/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move MCTS::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();

	Node* root=new Node(*state);
	Node* best_subnode;
	best_subnode=MCts(root);

  return best_subnode->act;
}

Node* MCTS::MCts(Node* root){
	int computation_budge=1000; //limit the times of searching
	Node* select_node=nullptr;
	int reward=0;
  for(int i=0;i<computation_budge;++i){
		select_node=TreePolicy(root);
	}

}
Node* MCTS::TreePolicy(Node* node){
    
}
Node* MCTS::BestChild(Node* node, bool is_explore){

}
Node* MCTS::Expand(Node* node){

}
int MCTS::DefaultPolicy(Node* node){

}
void MCTS::BackUp(Node* node, int reward){

}
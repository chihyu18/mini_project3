#include <cstdlib>
//self-added
#include<cmath>
#include<climits>
#include<set>

#include "../state/state.hpp"
#include "./mcts.hpp"
#include "./random.hpp"

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

	Node root=Node(*state);
	Node best_subnode;
	auto actions=state->legal_actions;
	for(auto act:actions){
		Node child=Node(*state);
		child.act=act;
		child.parent=&root;
		child.game_state=*(child.game_state.next_state(act)); //be careful?
		root.children.push_back(child);
	}
	best_subnode=MCts(&root);

  return best_subnode.act;
}

Node MCTS::MCts(Node* root){
	int computation_budge=1000; //limit the times of searching
	Node select_node;
	int reward=0;
  for(int i=0;i<computation_budge;++i){
		select_node=TreePolicy(root); //select
		reward=DefaultPolicy(&select_node); //simulate
		BackUp(&select_node, reward); //backup all the passed nodes
	}

}

Node MCTS::TreePolicy(Node* node){
	Node select_node;
	while(node->game_state.game_state==UNKNOWN || node->game_state.game_state==NONE){
		if(node->all_expanded()){
			select_node=BestChild(node, true); //exploitation...
		}	
		else	
			return Expand(node); //exploration...
	}
	return select_node;
}

Node MCTS::BestChild(Node* node, bool is_explore){
	//UCB: Q/N + C*sqrt(2*ln(N(parent))/N(child))
	//select priority: explored or not -> UCB

	Node best_node;
	auto& children=node->children;
	int max=INT_MIN, C;
	if(is_explore) C=1/sqrt(2);
	else C=0; //reduce the UCB of explored node(smaller priority)
	
	for(auto& child:children){
		int left=child.Q/child.N;
		int right=C*sqrt(2*log(node->N)/child.N);
		if(left+right>max){
			max=left+right;
			best_node=child;
		}
	}
	return best_node;
}
Node MCTS::Expand(Node* node){
	//randomly choose one unwalked node(with unwalked state)
	auto childrens=node->children;
	/*std::set<State> walked_states;
	for(auto &c:childrens){
		walked_states.insert(c.game_state);
	}*/
	int c=10;
	Node select_node=childrens[(rand()*c)%childrens.size()];
	while(select_node.N) select_node=childrens[(rand()*(++c))%childrens.size()];
	select_node.parent=node;
	return select_node;
}

int MCTS::DefaultPolicy(Node* node){
	//lose: if game_state=win but player is not me?
	State cur_state=node->game_state;
	int player=cur_state.player;
	int reward=0;
	while(cur_state.game_state==NONE || cur_state.game_state==UNKNOWN){
		auto move=get_move(&cur_state, 0);
		cur_state=*(cur_state.next_state(move));
	}
	if(cur_state.game_state==DRAW) reward=0.5;
	else if(cur_state.game_state==WIN && cur_state.player!=player) reward=-1; //lose...?
	else reward=1;
	return reward;

}
void MCTS::BackUp(Node* node, int reward){
	while(node!=nullptr){
		node->N++;
		node->Q+=reward;
		node=node->parent;
	}
}
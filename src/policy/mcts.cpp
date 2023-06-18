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

build tree is for propagating to the root?
*/

const int material_table[7] = {0, 2, 6, 7, 8, 20, 100};

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

	Node best_subnode;
	Node root=Node(*state);
	root.N=1;
	/*auto actions=state->legal_actions;
	for(auto act:actions){
		Node child=Node(*state);
		child.act=act;
		child.parent=&root;
		child.game_state=*(child.game_state.next_state(act)); //be careful?
		root.children.push_back(child);
	}*/
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
	return BestChild(root, false);
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
	//randomly choose one action that haven't chosed
	int c=10;
	srand(20);
	std::set<Move> walked_actions;
	for(auto& c:node->children) walked_actions.insert(c.act);
	
	auto actions=node->game_state.legal_actions;
	Move select_act=actions[(rand()*c)%actions.size()];
	while(walked_actions.find(select_act)!=walked_actions.end())
		select_act=actions[(rand()*c)%actions.size()];

	State* next_state=node->game_state.next_state(select_act);
	Node select_node=Node(*next_state);
	select_node.parent=node;
	return select_node;
}

int MCTS::DefaultPolicy(Node* node){
	//lose: if game_state=win but player is not me?
	State cur_state=node->game_state;
	int player=cur_state.player;
	int reward=0;

	//to check if draw:
	int step=0;
	int max_step=50;

	while(cur_state.game_state==NONE || cur_state.game_state==UNKNOWN){
		auto actions=cur_state.legal_actions;
		srand(10);
		auto move=actions[rand()%actions.size()];
		cur_state=*(cur_state.next_state(move));
		step++;
	
		if(step>max_step){
			int white_material = 0;
      int black_material = 0;
      int piece;
			State game=cur_state;
			for(size_t i=0; i<BOARD_H; i+=1){
        for(size_t j=0; j<BOARD_W; j+=1){
          if((piece=game.board.board[0][i][j])){
            white_material += material_table[piece];
          }
          if((piece=game.board.board[1][i][j])){
            black_material += material_table[piece];
          }
        }
      }
      if(white_material<black_material){
        game.player = 1;
        game.game_state = WIN;
      }else if(white_material>black_material){
        game.player = 0;
        game.game_state = WIN;
      }else{
        game.game_state = DRAW;
      }
		}
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
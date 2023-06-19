#include <cstdlib>
//self-added
#include<cmath>
#include<climits>
#include<set>
#include<ctime>

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

	srand(time(0));

	Node best_subnode;
	Node *root=new Node(*state);
	root->N=1;
	/*auto actions=state->legal_actions;
	for(auto act:actions){
		Node child=Node(*state);
		// child.N=1;
		child.act=act;
		child.parent=&root;
		child.game_state=*(child.game_state.next_state(act)); //be careful?
		root.children.push_back(child);
	}*/
	best_subnode=MCts(root);

	auto tmp=root->game_state.legal_actions;
  return tmp[rand()%tmp.size()]; //best_subnode.act; //tmp[rand()%tmp.size()];
}

Node MCTS::MCts(Node* root){
	int computation_budget=1000; //limit the times of searching
	Node select_node;
	int reward=0;
	srand(time(0));
  for(int i=0;i<computation_budget;++i){
		select_node=TreePolicy(root); //select
		reward=DefaultPolicy(&select_node); //simulate
		BackUp(&select_node, reward); //backup all the passed nodes
	}
	Node best_subnode=BestChild(root, false);
	return best_subnode;
}

Node MCTS::TreePolicy(Node* node){
	Node* cur_node=node;
	Node expand_node;
	while(cur_node->game_state.game_state==UNKNOWN || cur_node->game_state.game_state==NONE){
		if(cur_node->all_expanded()){
			*cur_node=BestChild(cur_node, true);
		}
		else{
			expand_node=Expand(cur_node);
			return expand_node;
		}		
	}
	return *cur_node;
}

Node MCTS::BestChild(Node* node, bool is_explore){
	//UCB: Q/N + C*sqrt(2*ln(N(parent))/N(child))
	//select priority: explored or not -> UCB

	auto& children=node->children;
	Node best_node=children[0];
	int max=INT_MIN;
	double C;
	if(is_explore) C=1/sqrt(2);
	else C=0.0; //reduce the UCB of explored node(smaller priority)
	
	for(auto& child:children){
		double left=child.Q/child.N;
		double right=C*sqrt(2*log(node->N)/child.N);
		if(left+right>max){
			max=left+right;
			best_node=child;
		}
	}
	best_node.act=children[0].game_state.legal_actions[0];
	return best_node;
}
Node MCTS::Expand(Node* node){
	//randomly choose one action that haven't chosed

	Move select_act;
	auto actions=node->game_state.legal_actions;

	/*if(!node->children.empty()){
		std::set<Move> walked_actions;
		std::vector<Move> unwalked_actions;
		for(auto& c:node->children) walked_actions.insert(c.act);
		// if(node->game_state.legal_actions.empty())
		// node->game_state.get_legal_actions();
		
		for(auto& act:actions){
			if(walked_actions.find(act)==walked_actions.end()) 
				unwalked_actions.push_back(act);
		}

		select_act=unwalked_actions[rand()%unwalked_actions.size()];

	}
	else select_act=actions[rand()%actions.size()];*/
	select_act=actions[0];

	State* next_state=node->game_state.next_state(select_act);
	Node select_node=Node(*next_state);
	select_node.act=select_act;
	select_node.parent=node;
	node->children.push_back(select_node);
	delete(next_state);
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
		
		auto move=actions[rand()%actions.size()];
		cur_state=*(cur_state.next_state(move));
		step++;
	
		if(step>max_step){
			int white_material = 0;
      int black_material = 0;
      int piece;
		State &game=cur_state;
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
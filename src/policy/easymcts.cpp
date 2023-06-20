#include <cstdlib>
//self-added
#include<cmath>
#include<climits>
#include<ctime>
#include<vector>

#include "../state/state.hpp"
#include "./easymcts.hpp"

const int material_table[7] = {0, 2, 6, 7, 8, 20, 100};

easyNode BestChild(easyNode* root);
int Rollout(easyNode* node);

Move EASYMCTS::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();

	srand(time(0));

  easyNode root=easyNode(state);
  int computation_budget=2000;
  auto& actions=state->legal_actions;
  int max=INT_MIN;
  Move best_act;
  for(auto& act:actions){

      State* next=state->next_state(act);
      easyNode child=easyNode(next);
      child.act=act;
      root.children.push_back(child);
  }
  for(auto& child:root.children){
    for(int i=0;i<computation_budget;++i){ 
      int reward=Rollout(&child);
      child.Q+=reward;
    }
  }
  
  for(auto& child:root.children){
    if(child.Q>max){
      max=child.Q;
      best_act=child.act;
    }
  }
  return best_act;
}

easyNode BestChild(easyNode* root){
    auto& children=root->children;
    int max=INT_MIN;
    double left, right;
    double C=1/sqrt(2);
    easyNode best_node;
    for(auto& child: children){
        left=child.Q/child.N+1e-6;
        right=C*sqrt(2*log(root->N+1e-6)/child.N+1e-9);
        if(left+right>max){
            max=left+right;
            best_node=child;
        }
    }
    return best_node;
}

int Rollout(easyNode* node){
  State cur_state=node->state;
  int player=!node->state.player;
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
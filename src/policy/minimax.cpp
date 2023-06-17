/*
the state value is determined by its branches
if oppn: choose "my" min
else: choose "my" max
me->opp->me->...int turns
by dfs?
*/
#include <cstdlib>
//self-added
#include<climits>
#include<iostream>

#include "../state/state.hpp"
#include "./minimax.hpp"


/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move Minimax::get_move(State *state, int depth){
  if(!state->legal_actions.size()) //no legal actions
    state->get_legal_actions();
  
  auto actions = state->legal_actions;

  //self-added
	int max=0;
	Move ans=actions[0];
	for(auto& act:actions){		
		// State next=*state;
		State next=State(state->board, state->player);//found that there is no copy constructor
		Board& b=next.board;
		std::swap(b.board[state->player][act.first.first][act.first.second], b.board[state->player][act.second.first][act.second.second]);
		next.legal_actions.clear();
		next.get_legal_actions();
		minimax(&next, depth, !state->player, 0);
		// next.evaluate();
		if(next.value>max){
			max=next.value;
			ans=act;
		}
		// max=std::max(max, minimax(&next, depth, !state->player, 0)); //next level would be oppn...?
	}

  return ans;
}

int minimax(State *state, int depth, int p, int d){
	//the right now player is state->player, while p is the "imagine" player!!
	//therefore, if p==state->player, return max, otherwise return min
	//but since we pass in out "oppn", the return value is opposite
	if(d==depth) return state->evaluate();

	if(state->legal_actions.empty()) state->get_legal_actions();
	auto actions=state->legal_actions;
	int min=INT_MAX, max=0;
	for(auto& act:actions){
		State next=State(state->board, state->player);
		Board& b=next.board;
		std::swap(b.board[p][act.first.first][act.first.second], b.board[p][act.second.first][act.second.second]);
		next.get_legal_actions();
		if(p==state->player){//oppn decide
			min=std::min(min, minimax(&next, depth, !p, d+1));
		}
		else{//me decide
			max=std::max(max, minimax(&next, depth, !p, d+1));
		}
		// minimax(&next, depth, !p, d+1);
	}
	if(p==state->player) return state->value=min; //oppn decide
	return state->value=max; //me decide
}
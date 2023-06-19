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
	int max=INT_MIN;
	Move ans=actions[0];
	for(auto& act:actions){		

		State *next;
		next=state->next_state(act);

		state->value=minimax(next, depth, state->player, 0);
		// next.evaluate();
		if(next->value>max){
			max=next->value;
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
	if(d==depth || state->game_state==WIN || state->legal_actions.empty()) return state->evaluate(p);

	auto actions=state->legal_actions;

	if(state->player!=p){ //oppn
		int min=INT_MAX;
		for(auto& act:actions){
			State *next;
			next=state->next_state(act);
			min=std::min(min, minimax(next, depth, p, d+1));
		}
		return state->value=min;
	}
	else{
		int max=INT_MIN;
		for(auto& act:actions){
			State *next;
			next=state->next_state(act);
			max=std::max(max, minimax(next, depth, p, d+1));
		}
		return state->value=max;
	}
}
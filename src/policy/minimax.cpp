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
	Move *ans;
	for(auto act:actions){
		State next=*state;
		Board& b=next.board;
		std::swap(b.board[act.first.first][act.first.second], b.board[act.second.first][act.second.second]);
		if(minimax(&next, depth, !state->player, 0)>max){
			max=minimax(&next, depth, !state->player, 0);
			ans=&act;
		}
		// max=std::max(max, minimax(&next, depth, !state->player, 0)); //next level would be oppn...?
	}

  return *ans;
}

int minimax(State *state, int depth, int p, int d){ //因為不知道上一層傳的會是min or max...
	//the right now player is state->player, while p is the "imagine" player!!
	//therefore, if p==state->player, return max, otherwise return min
	if(d==depth) return state->evaluate();

	auto actions=state->legal_actions;
	int min=INT_MAX, max=0;
	for(auto act:actions){
		State next=*state;
		Board& b=next.board;
		std::swap(b.board[act.first.first][act.first.second], b.board[act.second.first][act.second.second]);
		if(p==state->player){
			max=std::max(max, minimax(&next, depth, !p, d+1));
		}
		else{
			min=std::min(min, minimax(&next, depth, !p, d+1));
		}
		// minimax(&next, depth, !p, d+1);
	}
	if(p==state->player) return max;
	return min;
}
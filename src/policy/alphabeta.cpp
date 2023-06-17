/*
think bottom up?
for the same level, 
if player take moves: return the minimum one to the parent(oppn)
if oppen take moves: return the maximum to the parent(player)
*/
#include <cstdlib>
//self-added
#include<climits>

#include "../state/state.hpp"
#include "./alphabeta.hpp"

int alphabeta(State *state, int depth, int p, int d);

/**
 * @brief Randomly get a legal action
 * 
 * @param state Now state
 * @param depth You may need this for other policy
 * @return Move 
 */
Move Alphabeta::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();

  auto actions=state->legal_actions;
  //reset...?
  state->alpha=INT_MIN;
  state->beta=INT_MAX;
  int max=INT_MIN;
  Move ans=actions[0];
  for(auto& act:actions){
    State next=State(state->board, state->player, state->alpha, state->beta);
    auto& b=next.board.board;
    if(b[state->player][act.second.first][act.second.second]){
      b[state->player][act.second.first][act.second.second]=b[state->player][act.first.first][act.first.second];
      b[state->player][act.first.first][act.first.second]=0;
    }
    else
      std::swap(b[state->player][act.first.first][act.first.second], b[state->player][act.second.first][act.second.second]);
    next.get_legal_actions();
    alphabeta(&next, depth, !state->player, 0);
		if(next.value>max){
			max=next.value;
			ans=act;
		}
  }

  return ans;
}

int alphabeta(State *state, int depth, int p, int d){
    //since oppn must choose the min, but the last level(player) will choose the max, hence any smaller than that min is unnecessary
    //so for player, those alpha>=beta are redundant; while for oppn, those beta<=alpha are redundant
    //OMG the conditions are the same!!!
    if(d==depth) return state->evaluate();

    auto actions=state->legal_actions;
    if(p==state->player){ //oppn
      int min=INT_MAX;
      for(auto& act:actions){
        State next=State(state->board, state->player, state->alpha, state->beta);
        auto& b=next.board.board;
        if(b[p][act.second.first][act.second.second]){
          b[p][act.second.first][act.second.second]=b[p][act.first.first][act.first.second];
          b[p][act.first.first][act.first.second]=0;
        }
        else
          std::swap(b[p][act.first.first][act.first.second], b[p][act.second.first][act.second.second]);

        next.get_legal_actions();
        min=state->beta=std::min(state->beta, alphabeta(&next, depth, !p, d+1));
        if(state->alpha>=state->beta) break;
      }
      return state->value=min;
    }
    else{ //me
      int max=INT_MIN;
      for(auto& act:actions){
        State next=State(state->board, state->player, state->alpha, state->beta);
        auto& b=next.board.board;
        if(b[p][act.second.first][act.second.second]){
          b[p][act.second.first][act.second.second]=b[p][act.first.first][act.first.second];
          b[p][act.first.first][act.first.second]=0;
        }
        else
          std::swap(b[p][act.first.first][act.first.second], b[p][act.second.first][act.second.second]);

        next.get_legal_actions();
        max=state->alpha=std::max(state->alpha, alphabeta(&next, depth, !p, d+1));
        if(state->alpha>=state->beta) break;
      }
      return state->value=max;
    }

}

/*  if(d==depth) return state->evaluate();

    auto actions=state->legal_actions;
    int min=INT_MAX, max=0;
    for(auto& act:actions){
        State next=State(state->board, state->player, state->alpha, state->beta);
        auto& b=next.board.board;
        if(b[p][act.second.first][act.second.second]){
          b[p][act.second.first][act.second.second]=b[p][act.first.first][act.first.second];
          b[p][act.first.first][act.first.second]=0;
        }
        else
          std::swap(b[p][act.first.first][act.first.second], b[p][act.second.first][act.second.second]);
        next.get_legal_actions();
        if(p==state->player){
            // min=std::min(min, alphabeta(&next, depth, !p, d+1));
            min=state->beta=std::min(state->beta, alphabeta(&next, depth, !p, d+1));
        }
        else{
            // max=std::max(max, alphabeta(&next, depth, !p, d+1));
            max=state->alpha=std::max(state->alpha, alphabeta(&next, depth, !p, d+1));
        }
        if(state->alpha>=state->beta) break;
    }
    if(p==state->player) return state->value=min;
    return state->value=max;*/
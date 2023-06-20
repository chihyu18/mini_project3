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

Move Alphabeta::get_move(State *state, int depth){
  if(!state->legal_actions.size())
    state->get_legal_actions();

  auto actions=state->legal_actions;
  //reset...?
  state->alpha=INT_MIN;
  state->beta=INT_MAX;
  Move ans=actions[0];

  if(state->player==0){
    int max=INT_MIN;   
    for(auto& act:actions){  
      State* next=state->next_state(act);
      int value=alphabeta(next, depth, !state->player, 0);
      if(value>max){
        max=value;
        ans=act;
      }
      delete(next);
    }
  }
  else{
    int min=INT_MAX;
    for(auto& act:actions){  
      State* next=state->next_state(act);
      int value=alphabeta(next, depth, !state->player, 0);
      if(value<min){
        min=value;
        ans=act;
      }
    }
  }
  

  return ans;
}

int alphabeta(State *state, int depth, int p, int d){
    //since oppn must choose the min, but the last level(player) will choose the max, 
    // hence any smaller than that min is unnecessary
    //so for player, those alpha>=beta are redundant; while for oppn, those beta<=alpha are redundant
    //OMG the conditions are the same!!!
    if(d==depth) 
      return state->evaluate();

    state->get_legal_actions();
    auto actions=state->legal_actions;

    if(p==1){ //for black
      int min=INT_MAX;
      for(auto& act:actions){

        State* next=state->next_state(act);
        next->alpha=state->alpha;
        next->beta=state->beta;

        int v=alphabeta(next, depth, !p, d+1);
        min=std::min(min, v);
        state->beta=min;
        
        delete(next);
        if(state->alpha>=state->beta) break;
      }
      return state->value=min;
    }
    else{ //for white
      int max=INT_MIN;
      for(auto& act:actions){

        State* next=state->next_state(act);
        next->alpha=state->alpha;
        next->beta=state->beta;

        int v=alphabeta(next, depth, !p, d+1);
        max=std::max(max, v);
        state->alpha=max;
        delete(next);
        if(state->alpha>=state->beta) break;
      }
      return state->value=max;
    }

}
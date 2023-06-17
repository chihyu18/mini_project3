/*
think bottom up?
for the same level, 
if player take moves: return the minimum one to the parent(oppn)
if oppen take moves: return the maximum to the parent(player)
*/
#include <cstdlib>

#include "../state/state.hpp"
#include "./alphabeta.hpp"


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
  
  auto actions = state->legal_actions;
  return actions[(rand()+depth)%actions.size()];
}

int alphabeta(State *state, int depth, int p, int d){
    
    if(d==depth) return state->evaluate();

    auto actions=state->legal_actions;
    int min=INT_MAX, max=0;
    for(auto& act:actions){
        State next=State(state->board, state->player, state->alpha, state->beta);
        Board& b=next.board;
        std::swap(b.board[p][act.first.first][act.first.second], b.board[p][act.second.first][act.second.second]);
        next.get_legal_actions();
        if(p==state->player){ //oppn make move
            min=next.beta=std::min(next.beta, alphabeta(&next, depth, !p, d+1));
            if(next.beta<next.alpha) break;
        }
        else{ //me make move
            max=next.alpha=std::max(next.alpha, alphabeta(&next, depth, !p, d+1));
            if(next.alpha>next.beta) break;
        }
    }
    if(p==state->player) state->alpha=min;
    else state->beta=max;
}
#ifndef __STATE_H__
#define __STATE_H__

#include <string>
#include <cstdlib>
#include <vector>
#include <utility>
//self-added
#include<climits>
#include<set>
#include<map>

#include "../config.hpp"


typedef std::pair<size_t, size_t> Point;
typedef std::pair<Point, Point> Move; //from:to
class Board{
  public:
    char board[2][BOARD_H][BOARD_W] = {{
      //white
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {1, 1, 1, 1, 1}, //pawns
      {2, 3, 4, 5, 6}, //rook knight bishop queen king
    }, {
      //black
      {6, 5, 4, 3, 2},
      {1, 1, 1, 1, 1},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
      {0, 0, 0, 0, 0},
    }};
};

enum GameState {
  UNKNOWN = 0,
  WIN,
  DRAW,
  NONE
};

//self-added
//reference:https://www.chessprogramming.org/Simplified_Evaluation_Function
const int pawn_Square_Table[2][5][5]={
  {
    {0,   0,   0,   0,   0},      
    {5,  10,  10,  10,   5},
    {5,   5,  10,  10,   5},
    {5,   5,   5,   5,   5},
    {0,   0,   0,   0,   0}
  }, 
  {
    {0,   0,   0,   0,   0},
    {5,   5,   5,   5,   5}, 
    {5,   5,  10,  10,   5},
    {5,  10,  10,  10,   5},
    {0,   0,   0,   0,   0}    
  }
};

const int knight_Square_Table[5][5]={
  {0,   0,   0,   0,   0},
  {0,   5,   5,   5,   0},
  {0,   5,  10,   5,   0},
  {0,   5,   5,   5,   0},
  {0,   0,   0,   0,   0}
};

const int bishop_Square_Table[5][5]={
  {0,   0,   0,   0,   0},
  {0,  10,   5,  10,   0},
  {0,   5,  10,   5,   0},
  {0,  10,   5,  10,   0},
  {0,   0,   0,   0,    0}
};

const int rook_Square_Table[2][5][5]={
  {
    {0,   5,   5,   0,   0},
    {0,   5,   5,   0,   0},
    {0,   5,   5,   0,   0},
    {0,   5,   5,   0,   0},
    {0,   5,   5,   0,   0}
  },
  {
    {0,   0,   5,   5,   0},
    {0,   0,   5,   5,   0},
    {0,   0,   5,   5,   0},
    {0,   0,   5,   5,   0},
    {0,   0,   5,   5,   0}
  }
};

const int queen_Square_Table[5][5]={
  {0,   0,   5,   0,   0},
  {0,   5,   5,   5,   0},
  {5,   5,  10,   5,   5},
  {0,   5,   5,   5,   0},
  {0,   0,   5,   0,   0}
};

const int king_Square_Table1[2][5][5]={
  {
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0},
    {0,   5,   0,   5,   0}
  },
  {
    {0,   5,   0,   5,   0},
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0},
    {0,   0,   0,   0,   0}
  }
  
};

const int king_Square_Table2[5][5]={
  {0,   0,   0,   0,   0},
  {0,   5,   5,   5,   0},
  {0,   5,  10,   5,   0},
  {0,   5,   5,   5,   0},
  {0,   0,   0,   0,   0}
};



class State{
  public:
    //You may want to add more property for a state
    GameState game_state = UNKNOWN;
    Board board;
    int player = 0;
    std::vector<Move> legal_actions;
    //self-added
    int value;
    int alpha=INT_MIN; //the max the player can get
    int beta=INT_MAX; //the min the oppn can get
    int step=0; //record how many steps have been walked

    // std::multiset<std::pair<Piece, Point>> self_pieces, oppn_pieces;
    // std::map<Point, Piece> self_pieces, oppn_pieces;
    
    State(){};
    State(int player): player(player){};
    State(Board board): board(board){};
    State(Board board, int player): board(board), player(player){};
    
    //self-added
    State(Board board, int player, int a, int b): board(board), player(player), alpha(a), beta(b){};
    // State(State& s): board(s.board), player(s.player), alpha(s.alpha), beta(s.beta), game_state(s.game_state){};
    int eval(int p);

    int evaluate();
    State* next_state(Move move);
    void get_legal_actions();
    std::string encode_output();
    std::string encode_state();
};

//self-added for mcts
class Node{
public:
  Node *parent=nullptr;
  std::vector<Node> children; //存走過的node?
  int Q=0, N=0; //for UCB
  State game_state; //if win: value=1, lose: value=2, draw: value=0.5
  Move act; //for preventing repeatedly?
  Node(){};
  // Node(Node& n):parent(n.parent), children(n.children), Q(n.Q), N(n.N), game_state(n.game_state){};
  Node(State& s):game_state(s){};
  bool all_expanded(){
    if(children.size()==game_state.legal_actions.size()) return true;
    return false;
    // for(auto& c:children){
    //   if(c.N==0) return false;
    // }
    // return true;
  }
};

#endif

/*just for backup:D
// Pawn
    {
        {0,   0,   0,   0,   0},
        {5,   5,   5,   5,   5},
        {5,  10,  10,  10,   5},
        {5,   5,  10,  10,   5},
        {0,   0,   0,   0,   0}
    },
    // Rook
    {
        {0,   0,   5,   5,   0},
        {0,   0,   5,   5,   0},
        {0,   0,   5,   5,   0},
        {0,   0,   5,   5,   0},
        {0,   0,   5,   5,   0}
    },
    // Bishop
    {
        {0,   0,   0,   0,   0},
        {0,  10,   5,  10,   0},
        {0,   5,  10,   5,   0},
        {0,  10,   5,  10,   0},
        {0,   0,   0,   0,    0}
    },
    // Queen
    {
        {0,   0,   5,   0,   0},
        {0,   5,   5,   5,   0},
        {5,   5,  10,   5,   5},
        {0,   5,   5,   5,   0},
        {0,   0,   5,   0,   0}
    },
    // Knight
    {
        {0,   0,   0,   0,   0},
        {0,   5,   5,   5,   0},
        {0,   5,  10,   5,   0},
        {0,   5,   5,   5,   0},
        {0,   0,   0,   0,   0}
    },
    // King
    {
        {0,   0,   0,   0,   0},
        {0,   5,   5,   5,   0},
        {0,   5,  10,   5,   0},
        {0,   5,   5,   5,   0},
        {0,   0,   0,   0,   0}
    }
*/
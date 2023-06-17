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
      {2, 3, 4, 5, 6}, //castle knight bishop queen king
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
enum Piece{
  PAWN=10, ROOK=20, KNIGHT=35, BISHOP=40, QUEEN=100, KING=100000000
  // PAWN=1, CASTLE, KNIGHT, BISHOP, QUEEN, KING
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
    // std::multiset<std::pair<Piece, Point>> self_pieces, oppn_pieces;
    std::map<Point, Piece> self_pieces, oppn_pieces;
    
    State(){};
    State(int player): player(player){};
    State(Board board): board(board){};
    State(Board board, int player): board(board), player(player){};
    
    //slef-added
    State(Board board, int player, int a, int b): board(board), player(player), alpha(a), beta(b){};
    int get_val(int player, int x, int y);
    int eval(int p);

    int evaluate();
    State* next_state(Move move);
    void get_legal_actions();
    std::string encode_output();
    std::string encode_state();
};

#endif
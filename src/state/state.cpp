#include <iostream>
#include <sstream>
#include <cstdint>

#include "./state.hpp"
#include "../config.hpp"

//self-added
#define INF 1000;

/**
 * @brief evaluate the state
 * 
 * @return int 
 */

//self-added
//reference: https://rustic-chess.org/evaluation/material.html
//pawn rook knight bishop queen king /rank
int piece_val[7]={0, 10, 50, 30, 30, 90, 1000}; //1
// int piece_val[7]={0, 1, 5, 3, 3, 9, 100}; //2
// int piece_val[7]={0, 100, 500, 300, 300, 900, 20000}; //not good

/**
 * @brief return next state after the move
 * 
 * @param move 
 * @return State* 
 */
State* State::next_state(Move move){
  Board next = this->board;
  Point from = move.first, to = move.second;
  
  int8_t moved = next.board[this->player][from.first][from.second];
  //promotion for pawn
  if(moved == 1 && (to.first==BOARD_H-1 || to.first==0)){
    moved = 5;
  }
  if(next.board[1-this->player][to.first][to.second]){
    next.board[1-this->player][to.first][to.second] = 0;
  }
  
  //self-added for test
  // std::swap(next.board[this->player][from.first][from.second], next.board[this->player][to.first][to.second]);
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}


static const int move_table_rook_bishop[8][7][2] = {
  //rook: 0:3
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  //bishop: 4:7
  {{1, 1}, {2, 2}, {3, 3}, {4, 4}, {5, 5}, {6, 6}, {7, 7}},
  {{1, -1}, {2, -2}, {3, -3}, {4, -4}, {5, -5}, {6, -6}, {7, -7}},
  {{-1, 1}, {-2, 2}, {-3, 3}, {-4, 4}, {-5, 5}, {-6, 6}, {-7, 7}},
  {{-1, -1}, {-2, -2}, {-3, -3}, {-4, -4}, {-5, -5}, {-6, -6}, {-7, -7}},
};
static const int move_table_knight[8][2] = {
  {1, 2}, {1, -2},
  {-1, 2}, {-1, -2},
  {2, 1}, {2, -1},
  {-2, 1}, {-2, -1},
};
static const int move_table_king[8][2] = {
  {1, 0}, {0, 1}, {-1, 0}, {0, -1}, 
  {1, 1}, {1, -1}, {-1, 1}, {-1, -1},
};

bool check_valid(int x, int y){
  if(x<0 || x>=BOARD_H || y<0 || y>=BOARD_W) return false;
  return true;
}

int State::eval(int p){
  int value=0;
  int king_threat=0;
  int nx, ny;
  auto& self_board=board.board[p];
  auto& oppn_board=board.board[!p];
  for(int i=0;i<BOARD_H;++i){
    for(int j=0;j<BOARD_W;++j){
      value+=piece_val[self_board[i][j]];
      switch(self_board[i][j]){
        case 1:
          value+=piece_val[1]+pawn_Square_Table[p][i][j];
          break;
        case 2:
          value+=piece_val[2]+rook_Square_Table[p][i][j];
          break;
        case 3:
          value+=piece_val[3]+knight_Square_Table[i][j];
          break;
        case 4:
          value+=piece_val[4]+bishop_Square_Table[i][j];
          break;
        case 5:
          value+=piece_val[5]+queen_Square_Table[i][j];
          break;
        case 6:
          value+=piece_val[6]+king_Square_Table2[i][j];
          for(int k=0;k<8;++k){
            nx=i+move_table_knight[k][0];
            ny=j+move_table_knight[k][1];
            if(!check_valid(nx, ny) || self_board[nx][ny]) continue;
            if(oppn_board[nx][ny]==3) king_threat++;
          }
          for(int path=0;path<4;++path){
            for(int k=0;k<7;++k){
              nx=i+move_table_rook_bishop[path][k][0];
              ny=j+move_table_rook_bishop[path][k][1];
              if(!check_valid(nx, ny) || self_board[nx][ny]) break;
              if(oppn_board[nx][ny]==2 || oppn_board[nx][ny]==5) king_threat++;
              else if(oppn_board[nx][ny])  break; //the attack can be blocked
            }
          }
          for(int path=4;path<8;++path){
            for(int k=0;k<7;++k){
              nx=i+move_table_rook_bishop[path][k][0];
              ny=j+move_table_rook_bishop[path][k][1];
              if(!check_valid(nx, ny) || self_board[nx][ny]) break;
              if(oppn_board[nx][ny]==4 || oppn_board[nx][ny]==5) king_threat++;
              else if(oppn_board[nx][ny]) break;
            }
          }
          break;
        default:
          break;
      }
    }
  }
  return value-king_threat*90; //causing threats is just like losing queens(?
}

int State::evaluate(){ //eval from p's perspective
  // [TODO] design your own evaluation function
  /*
  evaluate the present state?
  border is not a good place, center should get higher score(?)
  if check, score high
  if threatened, score low

  val of each pieces: val*num_of_walkable_path + val_of_oppn_pieces_can_be_eaten?
  */
 //optimize: maybe use a map to record the pos of pieces?
  if(game_state==WIN && player==1) return -100000;
  if(game_state==WIN) return 100000;
  int white_val=eval(0);
  int black_val=eval(1);
  // int my_val=eval(p);
  // int oppn_val=eval(!p);
  return value=white_val-black_val;
}


/**
 * @brief get all legal actions of now state
 * 
 */
void State::get_legal_actions(){
  // [Optional]
  // This method is not very efficient
  // You can redesign it
  this->game_state = NONE;
  std::vector<Move> all_actions;
  auto self_board = this->board.board[this->player];
  auto oppn_board = this->board.board[1 - this->player];
  
  int now_piece, oppn_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece=self_board[i][j])){
        // std::cout << this->player << "," << now_piece << ' ';
        switch (now_piece){
          case 1: //pawn
            if(this->player && i<BOARD_H-1){
              //black
              if(!oppn_board[i+1][j] && !self_board[i+1][j])
                all_actions.push_back(Move(Point(i, j), Point(i+1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i+1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i+1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i+1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }else if(!this->player && i>0){
              //white
              if(!oppn_board[i-1][j] && !self_board[i-1][j])
                all_actions.push_back(Move(Point(i, j), Point(i-1, j)));
              if(j<BOARD_W-1 && (oppn_piece=oppn_board[i-1][j+1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j+1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
              if(j>0 && (oppn_piece=oppn_board[i-1][j-1])>0){
                all_actions.push_back(Move(Point(i, j), Point(i-1, j-1)));
                if(oppn_piece==6){
                  this->game_state = WIN;
                  this->legal_actions = all_actions;
                  return;
                }
              }
            }
            break;
          
          case 2: //rook
          case 4: //bishop
          case 5: //queen
            int st, end;
            switch (now_piece){
              case 2: st=0; end=4; break; //rook
              case 4: st=4; end=8; break; //bishop
              case 5: st=0; end=8; break; //queen
              default: st=0; end=-1;
            }
            for(int part=st; part<end; part+=1){
              auto move_list = move_table_rook_bishop[part];
              for(int k=0; k<std::max(BOARD_H, BOARD_W); k+=1){
                int p[2] = {move_list[k][0] + i, move_list[k][1] + j};
                
                if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) break;
                now_piece = self_board[p[0]][p[1]];
                if(now_piece) break; 
                //if want to combine the state-value func, maybe here(but it didn't care about whose piece is it)
                
                all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
                
                oppn_piece = oppn_board[p[0]][p[1]];
                if(oppn_piece){
                  if(oppn_piece==6){
                    this->game_state = WIN;
                    this->legal_actions = all_actions;
                    return;
                  }else
                    break;
                };
              }
            }
            break;
          
          case 3: //knight
            for(auto move: move_table_knight){
              int x = move[0] + i;
              int y = move[1] + j;
              
              if(x>=BOARD_H || x<0 || y>=BOARD_W || y<0) continue;
              now_piece = self_board[x][y];
              if(now_piece) continue;
              all_actions.push_back(Move(Point(i, j), Point(x, y)));
              
              oppn_piece = oppn_board[x][y];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
          
          case 6: //king
            for(auto move: move_table_king){
              int p[2] = {move[0] + i, move[1] + j};
              
              if(p[0]>=BOARD_H || p[0]<0 || p[1]>=BOARD_W || p[1]<0) continue;
              now_piece = self_board[p[0]][p[1]];
              if(now_piece) continue;
              
              all_actions.push_back(Move(Point(i, j), Point(p[0], p[1])));
              
              oppn_piece = oppn_board[p[0]][p[1]];
              if(oppn_piece==6){
                this->game_state = WIN;
                this->legal_actions = all_actions;
                return;
              }
            }
            break;
        }
      }
    }
  }
  std::cout << "\n";
  this->legal_actions = all_actions;
}


const char piece_table[2][7][5] = {
  {" ", "♙", "♖", "♘", "♗", "♕", "♔"},
  {" ", "♟", "♜", "♞", "♝", "♛", "♚"}
};
/**
 * @brief encode the output for command line output
 * 
 * @return std::string 
 */
std::string State::encode_output(){
  std::stringstream ss;
  int now_piece;
  for(int i=0; i<BOARD_H; i+=1){
    for(int j=0; j<BOARD_W; j+=1){
      if((now_piece = this->board.board[0][i][j])){
        ss << std::string(piece_table[0][now_piece]);
      }else if((now_piece = this->board.board[1][i][j])){
        ss << std::string(piece_table[1][now_piece]);
      }else{
        ss << " ";
      }
      ss << " ";
    }
    ss << "\n";
  }
  return ss.str();
}


/**
 * @brief encode the state to the format for player
 * 
 * @return std::string 
 */
std::string State::encode_state(){
  std::stringstream ss;
  ss << this->player;
  ss << "\n";
  for(int pl=0; pl<2; pl+=1){
    for(int i=0; i<BOARD_H; i+=1){
      for(int j=0; j<BOARD_W; j+=1){
        ss << int(this->board.board[pl][i][j]);
        ss << " ";
      }
      ss << "\n";
    }
    ss << "\n";
  }
  return ss.str();
}
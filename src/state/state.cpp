#include <iostream>
#include <sstream>
#include <cstdint>

#include "./state.hpp"
#include "../config.hpp"

//self-added
#define INF 1e6;

/**
 * @brief evaluate the state
 * 
 * @return int 
 */
int State::get_val(int player, int x, int y){
  //get val of the piece on the board
  switch(board.board[!player][x][y]){
    case 1: return 10;
    case 2: return 20;
    case 3: return 35;
    case 4: return 40;
    case 5: return 100;
    case 6: return INF;
    default: return 0;
  }
}


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
  
  next.board[this->player][from.first][from.second] = 0;
  next.board[this->player][to.first][to.second] = moved;
  
  State* next_state = new State(next, 1-this->player);
  
  if(this->game_state != WIN)
    next_state->get_legal_actions();
  return next_state;
}


static const int move_table_rook_bishop[8][7][2] = {
  //rook: 0~3
  {{0, 1}, {0, 2}, {0, 3}, {0, 4}, {0, 5}, {0, 6}, {0, 7}},
  {{0, -1}, {0, -2}, {0, -3}, {0, -4}, {0, -5}, {0, -6}, {0, -7}},
  {{1, 0}, {2, 0}, {3, 0}, {4, 0}, {5, 0}, {6, 0}, {7, 0}},
  {{-1, 0}, {-2, 0}, {-3, 0}, {-4, 0}, {-5, 0}, {-6, 0}, {-7, 0}},
  //bishop: 4~7
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

int State::eval_myside(){
  auto self_board=board.board[player], oppn_board=board.board[!player];
  int value=0;
  int now_piece;
  int nx, ny, walkable=0, k;
  for(int i=0;i<BOARD_H;++i){
    for(int j=0;j<BOARD_W;++j){
      switch(now_piece=self_board[i][j]){
        case 1: //pawn-10
          if(!player && i>0){ //white
            if(!self_board[i-1][j] && !oppn_board[i-1][j]) value+=10;
            if(j>0 && oppn_board[i-1][j-1]) value+=get_val(~player, i-1, j-1);
            if(j<BOARD_W-1 && oppn_board[i-1][j+1]) value+=get_val(~player, i-1, j+1);
          }
          else if(player && i<BOARD_H-1){ //black
            if(!self_board[i+1][j] && !oppn_board[i+1][j]) value+=10;
            if(j>0 && oppn_board[i+1][j-1]) value+=get_val(~player, i+1, j-1);
            if(j<BOARD_W-1 && oppn_board[i+1][j+1]) value+=get_val(~player, i+1, j+1);
          }
          break;
        case 2: //rook-20
        case 4: //bishop-40
        case 5: //queen-100
          int st, end, val;
          switch(now_piece){
            case 2: st=0; end=4; val=20; break; //rook
            case 4: st=4; end=8; val=40; break; //bishop
            case 5: st=0; end=8; val=100; break; //queen
            default: st=0; end=-1;
          }
          // int nx, ny, k;
          for(int path=st;path<end;++path){
            
            for(k=0;k<7;++k){
              nx=i+move_table_rook_bishop[path][k][0];
              ny=j+move_table_rook_bishop[path][k][1];
              if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) break;
              if(self_board[nx][ny]) break;
              if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
            }
            value+=val*k;
          }
          break;
        case 3: //knight-35
          // int nx, ny, k, walkable=0;
          for(k=0;k<8;++k){
            nx=i+move_table_knight[k][0];
            ny=j+move_table_knight[k][1];
            if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) continue;
            if(self_board[nx][ny]) continue;
            if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
            walkable++;
          }
          value+=35*walkable;
          break;
        case 6: //king-INF
        //maybe i need to consider check...
          // int nx, ny;
          for(k=0;k<8;++k){
            nx=i+move_table_king[k][0];
            ny=j+move_table_king[k][1];
            if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) continue;
            if(self_board[nx][ny]) continue;
            if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
          }
          value+=INF;
          break;
        default:
          break;
      }
    }
  }
  return 0;
}
int State::eval_oppnside(){
  auto self_board=board.board[!player], oppn_board=board.board[player];
  int value=0;
  int now_piece;
  int nx, ny, walkable=0, k;
  for(int i=0;i<BOARD_H;++i){
    for(int j=0;j<BOARD_W;++j){
      
      switch(now_piece=self_board[i][j]){
        case 1: //pawn-10
          if(!player && i>0){ //white
            if(!self_board[i-1][j] && !oppn_board[i-1][j]) value+=10;
            if(j>0 && oppn_board[i-1][j-1]) value+=get_val(~player, i-1, j-1);
            if(j<BOARD_W-1 && oppn_board[i-1][j+1]) value+=get_val(~player, i-1, j+1);
          }
          else if(player && i<BOARD_H-1){ //black
            if(!self_board[i+1][j] && !oppn_board[i+1][j]) value+=10;
            if(j>0 && oppn_board[i+1][j-1]) value+=get_val(~player, i+1, j-1);
            if(j<BOARD_W-1 && oppn_board[i+1][j+1]) value+=get_val(~player, i+1, j+1);
          }
          break;
        case 2: //rook-20
        case 4: //bishop-40
        case 5: //queen-100
          int st, end, val;
          switch(now_piece){
            case 2: st=0; end=4; val=20; break; //rook
            case 4: st=4; end=8; val=40; break; //bishop
            case 5: st=0; end=8; val=100; break; //queen
            default: st=0; end=-1;
          }
          for(int path=st;path<end;++path){
            // int nx, ny, k;
            for(k=0;k<7;++k){
              nx=i+move_table_rook_bishop[path][k][0];
              ny=j+move_table_rook_bishop[path][k][1];
              if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) break;
              if(self_board[nx][ny]) break;
              if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
            }
            value+=val*k;
          }
          break;
        case 3: //knight-35
          // int nx, ny, k, walkable=0;
          for(k=0;k<8;++k){
            nx=i+move_table_knight[k][0];
            ny=j+move_table_knight[k][1];
            if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) continue;
            if(self_board[nx][ny]) continue;
            if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
            walkable++;
          }
          value+=35*walkable;
          break;
        case 6: //king-INF
        //maybe i need to consider check...
          // int nx, ny;
          for(k=0;k<8;++k){
            nx=i+move_table_king[k][0];
            ny=j+move_table_king[k][1];
            if(nx<0 || nx>=BOARD_H || ny<0 || ny>=BOARD_W) continue;
            if(self_board[nx][ny]) continue;
            if(oppn_board[nx][ny]) value+=get_val(~player, nx, ny);
          }
          value+=INF;
          break;
        default:
          break;
      }
    }
  }
  return 0;
}

int State::evaluate(){
  // [TODO] design your own evaluation function
  /*
  evaluate the present state?
  border is not a good place, center should get higher score(?)
  if check, score high
  if threatened, score low

  val of each pieces: val*num_of_walkable_path + val_of_oppn_pieces_can_be_eaten?
  */
 //optimize: maybe use a map to record the pos of pieces?

  int my_val=eval_myside();
  int oppn_val=eval_oppnside();
  return my_val-oppn_val;
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
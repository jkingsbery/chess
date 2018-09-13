#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
#include <vector>
#include <utility>

using namespace std;

sf::Texture texture;
sf::Font font;
sf::Text text_to_play;
sf::Text game_messages;

const int FPS = 16;
const sf::Time SLEEP_TIME = sf::milliseconds(1000/FPS);
enum Player{WHITE, BLACK};

class ChessGameState;

class GamePiece {
public:
  virtual sf::Sprite * getSprite() {
    return new sf::Sprite(texture,sf::IntRect(80*getXLoc(),80*getYLoc(),80,80));
  }
  Player getColor() {
    return color;
  }
  //TODO implement logic for pieces other than pawns
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y) = 0;
  virtual GamePiece * copy() = 0;
  void markAsMoved() {
    hasMovedAtLeastOnce = true;
  }
  bool hasMovedBefore() {
    return hasMovedAtLeastOnce;
  }
  virtual ~GamePiece() {
    //doesn't allocate any memory, nothing to do
  }
  virtual bool isKing() {
    return false;
  }
protected:
  Player color;
  int xloc;
  int yloc;
  bool hasMovedAtLeastOnce;
  GamePiece(Player color) {
    this->color=color;
    hasMovedAtLeastOnce = false;
  }
  GamePiece(Player color, bool hasMoved) {
    this->color = color;
    hasMovedAtLeastOnce = hasMoved;
  }
  int getXLoc() {
    return color == WHITE ? xloc : xloc + 3;
  }
  int getYLoc() {
    return yloc;
  }
};

class King : public GamePiece {
public:
  King(Player color) : GamePiece(color) {
    xloc = 1;
    yloc = 1;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new King(color);
  }
  virtual bool isKing() {
    return true;
  }
};

class Queen : public GamePiece {
public:
  Queen(Player color) : GamePiece(color) {
    xloc = 1;
    yloc = 0;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new Queen(color);
  }
};

class Pawn : public GamePiece {
public:
  Pawn(Player color) : GamePiece(color) {
    xloc = 2;
    yloc = 0;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new Pawn(color);
  }
};

class Rook : public GamePiece {
public:
  Rook(Player color) : GamePiece(color) {
    xloc = 0;
    yloc = 0;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new Rook(color);
  }
};

class Bishop : public GamePiece {
public:
  Bishop(Player color) : GamePiece(color) {
    xloc = 2;
    yloc = 1;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new Bishop(color);
  }
};

class Knight  : public GamePiece {
public:
  Knight(Player color) : GamePiece(color) {
    xloc = 0;
    yloc = 1;
  };
  virtual vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  virtual GamePiece * copy(){
    return new Knight(color);
  }
};

class ChessGameState {
private:
  GamePiece* board[8][8];
  bool piece_selected;
  int pieceAboutToMoveX;
  int pieceAboutToMoveY;
  Player to_move;
public:
  ChessGameState() {
    setup_init_board(board);
    piece_selected = false;
    to_move = WHITE;
  }

  ChessGameState(ChessGameState * state) {
    for(int x = 0; x<8; x++) {
      for(int y = 0; y<8; y++) {
        if(state->getPieceAt(x,y)) {
          board[x][y] = state->getPieceAt(x,y)->copy();
        } else {
          board[x][y] = 0;
        }
      }
    }
    piece_selected = false;
    to_move = state->getPlayer();
  }
  ~ChessGameState() {
    for(int x = 0; x<8; x++) {
      for(int y = 0; y<8; y++) {
        if(board[x][y]) {
          delete board[x][y];
        }
      }
    }
  }

  void selected_square(int x, int y);
  void setup_init_board(GamePiece* board[8][8]);
  void draw_to_window(sf::RenderWindow * window);
  GamePiece* getPieceAt(int x, int y);
  void executeMove(int init_x, int init_y, int dest_x, int dest_y);
  Player getPlayer() {
    return to_move;
  }
  bool verifyCheck(int x, int y);
};

GamePiece* ChessGameState::getPieceAt(int x, int y) {
  return board[x][y];
}

void ChessGameState::executeMove(int init_x, int init_y, int dest_x, int dest_y) {
  GamePiece* piece = board[init_x][init_y];
  if(!piece) {
    cout << "No piece found at " << init_x << ", " << init_y << "\n";
  }
  board[init_x][init_y] = 0;
  piece_selected = false;
  board[dest_x][dest_y] = piece;
  piece->markAsMoved();
  to_move = (to_move == WHITE ? BLACK : WHITE);
}

bool ChessGameState::verifyCheck(int x, int y) {
  ChessGameState * copy_for_check = new ChessGameState(this);
  copy_for_check->executeMove(pieceAboutToMoveX, pieceAboutToMoveY, x, y);
  // verify that player's own king is not in check after the move
  // 1. Find the king's position
  // Loop over the set of pieces
  // for each piece on the board, if a set of legal moves includes the king, the move is illegal
  pair<int, int> king_loc_temp;
  bool inCheck = false;
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      GamePiece * piece = copy_for_check->getPieceAt(x,y);
      if(piece && piece->isKing() && piece->getColor() == to_move) {
        king_loc_temp.first = x;
        king_loc_temp.second = y;
      }
    }
  }
  auto king_loc = make_pair(king_loc_temp.first, king_loc_temp.second);
  for(int x = 0; x < 8; x++) {
    for(int y = 0; y < 8; y++) {
      GamePiece * piece = copy_for_check->getPieceAt(x,y);
      if(piece && piece->getColor() != to_move) {
        auto moves = piece->getLegalMoves(copy_for_check, x, y);
        if(moves->size() > 0) {
          inCheck |= find(moves->begin(), moves->end(), king_loc) != moves->end();
        }
      }
    }
  }
  delete copy_for_check;
  return inCheck;
}

void ChessGameState::selected_square(int x, int y) {
  if(x<0 || x >=8 || y < 0 || y >= 8) {
      return;
  }
  if(!piece_selected) {

    if(board[x][y] == 0) {
      //ignore stray clicks
      return;
    }
    //check it's a piece for the right player
    if(board[x][y]->getColor() == to_move) {   
      pieceAboutToMoveX = x;
      pieceAboutToMoveY = y;
      piece_selected = true;
      game_messages.setString("Where do you want\nto move to?");
    } else {
      game_messages.setString("Select the right color!");
    }
  } else if(piece_selected) {
    if(pieceAboutToMoveX == x && pieceAboutToMoveY == y) {
      piece_selected = false;
      game_messages.setString("");
    } else {      
      GamePiece* piece = board[pieceAboutToMoveX][pieceAboutToMoveY];
      //check if legal move
      auto legal_moves = piece->getLegalMoves(this,pieceAboutToMoveX, pieceAboutToMoveY);
      auto p = std::make_pair(x, y);
      bool isLegalMove = find(legal_moves->begin(), legal_moves->end(), p) == legal_moves->end();
      bool inCheck = verifyCheck(x, y);
      if(isLegalMove ) {
        game_messages.setString("That's not a legal move!");
      } else if(inCheck) {
        game_messages.setString("That move would put the\nking in check!");
      } else {
        executeMove(pieceAboutToMoveX, pieceAboutToMoveY, x, y);
        game_messages.setString("");
      }
      delete legal_moves;
    }
  }

}

void ChessGameState::setup_init_board(GamePiece* board[8][8]) {
  for(int x = 0; x<8; x++){
    for(int y=0; y<8; y++){
      board[x][y]=0;
    }
  }
  board[0][7] = new Rook(WHITE);
  board[1][7] = new Knight(WHITE);
  board[2][7] = new Bishop(WHITE);
  board[3][7] = new Queen(WHITE);
  board[4][7] = new King(WHITE);
  board[5][7] = new Bishop(WHITE);
  board[6][7] = new Knight(WHITE);
  board[7][7] = new Rook(WHITE);
  board[0][0] = new Rook(BLACK);
  board[1][0] = new Knight(BLACK);
  board[2][0] = new Bishop(BLACK);
  board[3][0] = new Queen(BLACK);
  board[4][0] = new King(BLACK);
  board[5][0] = new Bishop(BLACK);
  board[6][0] = new Knight(BLACK);
  board[7][0] = new Rook(BLACK);
  for(int i=0; i<8; i++) {
    board[i][6] = new Pawn(WHITE);
    board[i][1] = new Pawn(BLACK);
  }
}

vector<pair<int, int> > * getLegalMovesInRange(ChessGameState * game, int x, int y, vector<pair<int, int> > * directions) {
  auto result = new vector<pair<int, int> >();
  for (auto dir = directions->begin(); dir != directions->end(); ++dir) {
    int steps = 1;
    auto loc = make_pair(x + dir->first * steps, y + dir->second * steps);
    //get loc from directions
    while(0 <= loc.first && loc.first < 8 &&
       0 <= loc.second && loc.second < 8 &&
       (game->getPieceAt(loc.first, loc.second) == 0 ||
        game->getPieceAt(loc.first, loc.second)->getColor() != game->getPieceAt(x, y)->getColor())) {

      result->push_back(make_pair(loc.first, loc.second));

      auto new_spot = game->getPieceAt(loc.first, loc.second);
      auto old_spot = game->getPieceAt(x, y);
      if(new_spot && old_spot && new_spot->getColor() != old_spot->getColor()) {
        //This move would capture a piece, have to stop here.
        break;
      }

      //Increment so the loop checks the next location
      steps++;
      loc = make_pair(x + dir->first * steps, y + dir->second * steps);

    }
  }
  return result;

}

vector<pair<int, int> > * Queen::getLegalMoves(ChessGameState * game, int x, int y) {
  auto possible = new vector<pair<int, int> >();
  possible->push_back(make_pair(1, 1));
  possible->push_back(make_pair( - 1, -1));
  possible->push_back(make_pair( - 1, 1));
  possible->push_back(make_pair( 1, -1));
  possible->push_back(make_pair( 0,  1));
  possible->push_back(make_pair( 0, - 1));
  possible->push_back(make_pair( 1,  0));
  possible->push_back(make_pair(- 1,  0));
  auto result = getLegalMovesInRange(game, x, y, possible);
  delete possible;
  return result;
}


vector<pair<int, int> > * Rook::getLegalMoves(ChessGameState * game, int x, int y) {
  auto possible = new vector<pair<int, int> >();
  possible->push_back(make_pair( 0,  1));
  possible->push_back(make_pair( 0, - 1));
  possible->push_back(make_pair( 1,  0));
  possible->push_back(make_pair(- 1,  0));
  auto result = getLegalMovesInRange(game, x, y, possible);
  delete possible;
  return result;
}


vector<pair<int, int> > * Bishop::getLegalMoves(ChessGameState * game, int x, int y) {
  auto possible = new vector<pair<int, int> >();
  possible->push_back(make_pair(1, 1));
  possible->push_back(make_pair( - 1, -1));
  possible->push_back(make_pair( - 1, 1));
  possible->push_back(make_pair( 1, -1));
  auto result = getLegalMovesInRange(game, x, y, possible);
  delete possible;
  return result;
}

vector<pair<int, int> > * getLegalMovesFromAbsolute(ChessGameState * game, int x, int y,  vector<pair<int, int> > * directions) {
 auto result = new vector<pair<int, int> >();
  for (auto loc = directions->begin(); loc != directions->end(); ++loc) {
    if(0 <= loc->first && loc->first < 8 &&
       0 <= loc->second && loc->second < 8 &&
       (game->getPieceAt(loc->first, loc->second) == 0 ||
        game->getPieceAt(loc->first, loc->second)->getColor() != game->getPieceAt(x, y)->getColor())) {
      result->push_back(make_pair(loc->first, loc->second));
    }
  }
  return result;
}

vector<pair<int, int> > * King::getLegalMoves(ChessGameState * game, int x, int y) {
  auto possible = new vector<pair<int, int> >();
  possible->push_back(make_pair(x + 1, y + 1));
  possible->push_back(make_pair(x - 1, y - 1));
  possible->push_back(make_pair(x - 1, y + 1));
  possible->push_back(make_pair(x + 1, y - 1));
  possible->push_back(make_pair(x + 0, y + 1));
  possible->push_back(make_pair(x + 0, y - 1));
  possible->push_back(make_pair(x + 1, y + 0));
  possible->push_back(make_pair(x - 1, y + 0));
  auto result = getLegalMovesFromAbsolute(game, x, y, possible);
  delete possible;
  return result;
}


vector<pair<int, int> > * Knight::getLegalMoves(ChessGameState * game, int x, int y) {
  auto possible = new vector<pair<int, int> >();
  possible->push_back(make_pair(x + 2, y + 1));
  possible->push_back(make_pair(x - 2, y - 1));
  possible->push_back(make_pair(x + 2, y - 1));
  possible->push_back(make_pair(x - 2, y + 1));
  possible->push_back(make_pair(x + 1, y + 2));
  possible->push_back(make_pair(x - 1, y - 2));
  possible->push_back(make_pair(x + 1, y - 2));
  possible->push_back(make_pair(x - 1, y + 2));
  auto result = getLegalMovesFromAbsolute(game, x, y, possible);
  delete possible;
  return result;
}

vector<pair<int, int> > * Pawn::getLegalMoves(ChessGameState * game, int x, int y) {
  auto result = new vector<pair<int, int> >();
  int ydelta = getColor() == WHITE ? -1 : 1;
  if(!hasMovedBefore() && game->getPieceAt(x, y + ydelta) == 0 &&
     game->getPieceAt(x, y + 2 * ydelta) == 0) {
    result->push_back(make_pair(x, y + 2* ydelta));
  }
  if(game->getPieceAt(x, y + ydelta) == 0) {
    result->push_back(make_pair(x, y + ydelta));
  }
  if(x+1 < 8) {
    auto onTheRight = game->getPieceAt(x + 1, y + ydelta);
    if(onTheRight != 0 && onTheRight->getColor() != getColor()) {
      result->push_back(make_pair(x + 1, y + ydelta));
    }
  }
  if(x-1 > 0) {
    auto onTheLeft = game->getPieceAt(x - 1, y + ydelta);
    if(onTheLeft != 0 && onTheLeft->getColor() != getColor()) {
      result->push_back(make_pair(x - 1, y + ydelta));
    }
  }
  return result;
}


void ChessGameState::draw_to_window(sf::RenderWindow * window) {

  // clear the window with black color
  window->clear(sf::Color::Black);

  //draw game board grid
  for(int x=0; x<8; x++) {
    for(int y=0; y<8; y++) {
      if((x%2==0 && (y)%2==0) || (x%2==1 && (y)%2==1)) {
        sf::RectangleShape rect(sf::Vector2f(80,80));
        rect.move(sf::Vector2f(80*(x), 80*(y)));
        window->draw(rect);
      }
    }
  }

  //draw pieces
  for(int x=0; x<8; x++) {
    for(int y=0; y<8; y++) {
      if(board[x][y]) {
        sf::Sprite * sprite = board[x][y]->getSprite();
        sprite->move(sf::Vector2f(80*x, 80*y));
        window->draw(*sprite);
      }
    }
  }
  if(to_move == WHITE) {
    text_to_play.setString("White to Play");
  } else {
    text_to_play.setString("Black to Play");
  }

  window->draw(text_to_play);
  window->draw(game_messages);
}

class ProgramState {

};

int main()
{
  sf::RenderWindow window(sf::VideoMode(1200, 800), "My window");

  texture.loadFromFile("assets/pieces.png");

  if (!font.loadFromFile("assets/arial.ttf"))
    return -1;

  text_to_play.setFont(font);
  text_to_play.setCharacterSize(36);
  text_to_play.move(sf::Vector2f(80*9, 80*1));
  game_messages.setFont(font);
  game_messages.setCharacterSize(36);
  game_messages.move(sf::Vector2f(80*9, 80*2));
  game_messages.setString("");
  ChessGameState gameState;

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed) {
        window.close();
      }
      else if (event.type == sf::Event::MouseButtonPressed) {
        // Figure out cell selected
        cout << "selected piece at " << event.mouseButton.x / 80 << ", " << event.mouseButton.y / 80 << std::endl;
        gameState.selected_square(event.mouseButton.x / 80, event.mouseButton.y / 80);
      }
    }

    gameState.draw_to_window(&window);
    // end the current frame
    window.display();
    sf::sleep(SLEEP_TIME);
  }

  return 0;

}

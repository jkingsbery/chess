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
  vector<pair<int, int> > * getLegalMoves(ChessGameState * game, int x, int y);
  void markAsMoved() {
    hasMovedAtLeastOnce = true;
  }
  bool hasMovedBefore() {
    return hasMovedAtLeastOnce;
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
};

class Queen : public GamePiece {
public:
  Queen(Player color) : GamePiece(color) {
    xloc = 1;
    yloc = 0;
  };
};

class Pawn : public GamePiece {
public:
  Pawn(Player color) : GamePiece(color) {
    xloc = 2;
    yloc = 0;
  };

};

class Rook : public GamePiece {
public:
  Rook(Player color) : GamePiece(color) {
    xloc = 0;
    yloc = 0;
  };
};

class Bishop : public GamePiece {
public:
  Bishop(Player color) : GamePiece(color) {
    xloc = 2;
    yloc = 1;
  };
};

class Knight  : public GamePiece {
public:
  Knight(Player color) : GamePiece(color) {
    xloc = 0;
    yloc = 1;
  };
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
  void selected_square(int x, int y);
  void setup_init_board(GamePiece* board[8][8]);
  void draw_to_window(sf::RenderWindow * window);
  GamePiece* getPieceAt(int x, int y);
};

GamePiece* ChessGameState::getPieceAt(int x, int y) {
  return board[x][y];
}

void ChessGameState::selected_square(int x, int y) {
  if(x<0 || x >=8 || y < 0 || y >= 8) {
      return;
  }
  if(!piece_selected) {
    //check it's a piece for the right player
    if(board[x][y] == 0) {
      //ignore stray clicks
      return;
    }
    if(board[x][y]->getColor() == to_move) {   
      pieceAboutToMoveX = x;
      pieceAboutToMoveY = y;
      piece_selected = true;
      game_messages.setString("");
    } else {
      game_messages.setString("Select the right color!");
    }
  } else if(piece_selected) {
    if(pieceAboutToMoveX == x && pieceAboutToMoveY == y) {
      piece_selected = false;
    } else {      
      GamePiece* piece = board[pieceAboutToMoveX][pieceAboutToMoveY];
      //check if legal move
      auto legal_moves = piece->getLegalMoves(this,pieceAboutToMoveX, pieceAboutToMoveY);
      auto p = std::make_pair(x, y);
      if(find(legal_moves->begin(), legal_moves->end(), p) == legal_moves->end() ) {
        game_messages.setString("That's not a legal move!");
      } else {
        //execute move
        board[pieceAboutToMoveX][pieceAboutToMoveY] = 0;
        pieceAboutToMoveX = 0;
        pieceAboutToMoveY = 0;
        piece_selected = false;
        board[x][y] = piece;
        piece->markAsMoved();
        to_move = (to_move == WHITE ? BLACK : WHITE);
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


vector<pair<int, int> > * GamePiece::getLegalMoves(ChessGameState * game, int x, int y) {
  cout << "Calculating legal moves for pos " << x << ", " << y << "...\n";
  vector<pair<int, int> > * result = new vector<pair<int, int> >();
  int ydelta = getColor() == WHITE ? -1 : 1;
  auto onTheRight = game->getPieceAt(x + 1, y + ydelta);
  auto onTheLeft = game->getPieceAt(x - 1, y + ydelta);
  if(!hasMovedBefore() && game->getPieceAt(x, y + ydelta) == 0 &&
     game->getPieceAt(x, y + 2 * ydelta) == 0) {
    result->push_back(make_pair(x, y + 2* ydelta));
  }
  if(game->getPieceAt(x, y + ydelta) == 0) {
    result->push_back(make_pair(x, y + ydelta));
  }
  if(onTheRight != 0 && onTheRight->getColor() != getColor()) {
    result->push_back(make_pair(x + 1, y + ydelta));      
  }
  if(onTheLeft != 0 && onTheLeft->getColor() != getColor()) {
    result->push_back(make_pair(x - 1, y + ydelta));
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
        std::cout << "mouse x: " << event.mouseButton.x / 80 << std::endl;
        std::cout << "mouse y: " << event.mouseButton.y / 80 << std::endl;
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

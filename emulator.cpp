#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/System/Time.hpp>
#include <iostream>
using namespace std;
sf::Texture texture;

const int FPS = 16;
const sf::Time SLEEP_TIME = sf::milliseconds(1000/FPS);
enum Player{WHITE, BLACK};

class GamePiece {
public:
  virtual sf::Sprite * getSprite() {
    return new sf::Sprite(texture,sf::IntRect(80*getXLoc(),80*getYLoc(),80,80));
  }

protected:
  Player color;
  int xloc;
  int yloc;
  GamePiece(Player color) {
    this->color=color;
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
  bool about_to_move;
  int pieceAboutToMoveX;
  int pieceAboutToMoveY;

public:
  ChessGameState() {
    setup_init_board(board);
    about_to_move = false;
  }
  void selected_square(int x, int y);
  void setup_init_board(GamePiece* board[8][8]);
  void draw_to_window(sf::RenderWindow * window);

};

void ChessGameState::selected_square(int x, int y) {
  if(x<0 || x >=8 || y < 0 || y >= 8) {
      return;
  }
  if(!about_to_move) {
    pieceAboutToMoveX = x;
    pieceAboutToMoveY = y;
    about_to_move = true;
  } else if(about_to_move) {
    GamePiece* piece = board[pieceAboutToMoveX][pieceAboutToMoveY];
    board[pieceAboutToMoveX][pieceAboutToMoveY] = 0;
    pieceAboutToMoveX = 0;
    pieceAboutToMoveY = 0;
    about_to_move = false;
    board[x][y] = piece;
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

}

class ProgramState {

};


int main()
{
  sf::RenderWindow window(sf::VideoMode(1000, 800), "My window");

  texture.loadFromFile("assets/pieces.png");

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
        std::cout << "mouse x: " << event.mouseButton.x / 80 << std::endl;
        std::cout << "mouse y: " << event.mouseButton.y / 80 << std::endl;
        gameState.selected_square(event.mouseButton.x / 80, event.mouseButton.y / 80);
        // Figure out cell

      }
    }

    gameState.draw_to_window(&window);

    // end the current frame
    window.display();
    sf::sleep(SLEEP_TIME);
  }

  return 0;

}

#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

int main()
{
  sf::RenderWindow window(sf::VideoMode(1000, 800), "My window");

  sf::Texture texture;
  texture.loadFromFile("assets/pieces.png");

  sf::Sprite white_rook_a(texture,sf::IntRect(80*0,80*0,80,80));
  sf::Sprite white_rook_b(texture,sf::IntRect(80*0,80*0,80,80));
  sf::Sprite white_bishop_a(texture,sf::IntRect(80*2,80*1,80,80));
  sf::Sprite white_bishop_b(texture,sf::IntRect(80*2,80*1,80,80));
  sf::Sprite white_knight_a(texture,sf::IntRect(80*0,80*1,80,80));
  sf::Sprite white_knight_b(texture,sf::IntRect(80*0,80*1,80,80));
  sf::Sprite white_king(texture,sf::IntRect(80*1,80*0,80,80));
  sf::Sprite white_queen(texture,sf::IntRect(80*1,80*1,80,80));
  
  white_rook_a.move(sf::Vector2f(80*0, 80*7));
  white_rook_b.move(sf::Vector2f(80*7, 80*7));
  white_knight_a.move(sf::Vector2f(80*1, 80*7));
  white_knight_b.move(sf::Vector2f(80*6, 80*7));
  white_bishop_a.move(sf::Vector2f(80*2, 80*7));
  white_bishop_b.move(sf::Vector2f(80*5, 80*7));
  white_king.move(sf::Vector2f(80*3, 80*7));
  white_queen.move(sf::Vector2f(80*4, 80*7));

  // run the program as long as the window is open
  while (window.isOpen()) {
    // check all the window's events that were triggered since the last iteration of the loop
    sf::Event event;
    while (window.pollEvent(event)) {
      // "close requested" event: we close the window
      if (event.type == sf::Event::Closed)
        window.close();
    }

    // clear the window with black color
    window.clear(sf::Color::Black);
    
    //draw game board grid
    for(int i=0; i<64; i++) {
      if((i%2==0 && (i/8)%2==0) || (i%2==1 && (i/8)%2==1)) {
        sf::RectangleShape rect(sf::Vector2f(80,80));
        rect.move(sf::Vector2f(80*(i%8), 80*(i/8)));
        window.draw(rect);
      }
    }

    //Draw pieces based on location
    window.draw(white_rook_a);
    window.draw(white_rook_b);
    window.draw(white_knight_a);
    window.draw(white_knight_b);
    window.draw(white_bishop_a);
    window.draw(white_bishop_b);
    window.draw(white_king);
    window.draw(white_queen);
    
    // draw everything here...
    // window.draw(...);

    // end the current frame
    window.display();
  }

  return 0;

}

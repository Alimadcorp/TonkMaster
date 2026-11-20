#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <sstream>

// DEFINITIONS OF GAME CONSTANTS
#define TANK_SPEED 200.f        // pixels per second
#define TANK_ROTATION_SPEED 2.f // radians per second
#define TANK_HIT_RADIUS 20.f
#define MAX_TANKS 2048

using namespace sf;
using namespace std;

int main() {
  // define main window, rendering, settings & clock
  unsigned int width = 500, height = 500;
  ContextSettings settings;
  settings.depthBits = 24;
  settings.stencilBits = 8;
  settings.antiAliasingLevel = 4;
  settings.majorVersion = 3;
  settings.minorVersion = 0;

  RenderWindow window(VideoMode({width, height}), "TankMaster", Style::Default,
                      State::Fullscreen, settings);
  width = window.getSize().x;
  height = window.getSize().y;
  window.setVerticalSyncEnabled(true);
  glEnable(GL_TEXTURE_2D);
  Clock clock;

  // --- VARIABLE DECLARATIONS ---
  float totalTime = 0.0f;
  int frameCount = 0;
  float tankX[MAX_TANKS], tankY[MAX_TANKS], tankA[MAX_TANKS];
  bool isTankGreen[MAX_TANKS] = {true};
  unsigned short int lowerTank = 0, upperTank = 1;

  // Initialize two tanks (Green on bottom-left, Red on top-right)
  tankX[0] = 100.f;
  tankY[0] = height - 100.f;
  tankX[1] = width - 100.f;
  tankY[1] = 100.f;
  tankA[0] = tankA[1] = 0.f;
  isTankGreen[0] = true;
  isTankGreen[1] = false;

  Vector2f rocks[10] = {{width / 2.f, height * 2.f / 3.f},
                        {width * 1.f / 4.f, height * 2.f / 5.f},
                        {width * 2.f / 3.f, height * 4.f / 7.f},
                        {width * 4.f / 5.f, height * 1.f / 5.f},
                        {width * 1.f / 5.f, height * 4.f / 5.f},
                        {width * 1.f / 8.f, height * 1.f / 8.f},
                        {width * 7.f / 8.f, height * 7.f / 8.f},
                        {width * 1.f / 2.f, height * 1.f / 4.f},
                        {width * 3.f / 4.f, height * 3.f / 4.f},
                        {width * 3.f / 8.f, height * 7.f / 8.f}};

  // --- TEXTURES ---
  Texture greenTankTexture("tank1.png");
  Texture redTankTexture("tank2.png");
  Texture rockT("rocks.png");
  Texture sand("sand.png");

  // --- OBJECTS ---
  Font mojangles("mojangles.ttf");
  Text fpsText(mojangles);
  Sprite greenTank(greenTankTexture);
  Sprite redTank(redTankTexture);
  Sprite bg(sand);
  Sprite rock1(rockT);
  Sprite rock2(rockT);
  Sprite rock3(rockT);

  // --- OBJECT PROPERTIES ---
  mojangles.setSmooth(false);
  sand.setSmooth(false);
  sand.setRepeated(true);
  rockT.setSmooth(false);
  bg.setPosition({0.f, 0.f});
  bg.setScale({3.f, 3.f});
  bg.setTextureRect(IntRect({0, 0}, {int(width), int(height)}));

  greenTankTexture.setSmooth(false);
  redTankTexture.setSmooth(false);
  greenTank.setScale({4.0f, 4.0f});
  redTank.setScale({4.0f, 4.0f});

  fpsText.setCharacterSize(24);
  fpsText.setFillColor(sf::Color::Green);
  fpsText.setPosition({10.f, 10.f});

  rock1.setTextureRect(IntRect({0, 0}, {11, 9}));
  rock2.setTextureRect(IntRect({1, 11}, {11, 10}));
  rock3.setTextureRect(IntRect({0, 22}, {12, 10}));
  rock1.setScale({3.f, 3.f});
  rock2.setScale({3.f, 3.f});
  rock3.setScale({3.f, 3.f});

  FloatRect rock1Bounds = rock1.getLocalBounds();
  FloatRect rock2Bounds = rock2.getLocalBounds();
  FloatRect rock3Bounds = rock3.getLocalBounds();
  FloatRect greenBounds = greenTank.getLocalBounds();
  FloatRect redBounds = redTank.getLocalBounds();

  rock1.setOrigin({rock1Bounds.size.x / 2.f, rock1Bounds.size.y / 2.f});
  rock2.setOrigin({rock2Bounds.size.x / 2.f, rock2Bounds.size.y / 2.f});
  rock3.setOrigin({rock3Bounds.size.x / 2.f, rock3Bounds.size.y / 2.f});
  greenTank.setOrigin({greenBounds.size.x / 2.f, greenBounds.size.y / 2.f});
  redTank.setOrigin({redBounds.size.x / 2.f, redBounds.size.y / 2.f});

  // MAIN LOOP
  while (window.isOpen()) {
    // --- EVENTS ---
    while (const optional event = window.pollEvent()) {
      if (event->is<Event::Closed>())
        window.close();
    }

    float deltaTime = clock.restart().asSeconds();
    totalTime += deltaTime;
    frameCount++;

    // --- INPUT AND PROCESSING ---
    Vector2f velGreen{0.f, 0.f};
    Vector2f velRed{0.f, 0.f};

    if (Keyboard::isKeyPressed(Keyboard::Key::W))
      velGreen.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::S))
      velGreen.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::A))
      velGreen.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::D))
      velGreen.x += 1.f;

    if (Keyboard::isKeyPressed(Keyboard::Key::Up))
      velRed.y += 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::Down))
      velRed.y -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::Left))
      velRed.x -= 1.f;
    if (Keyboard::isKeyPressed(Keyboard::Key::Right))
      velRed.x += 1.f;

    // Update Green Tank Position & Rotation
    tankA[0] += velGreen.x * deltaTime * TANK_ROTATION_SPEED;
    tankX[0] += velGreen.y * sin(tankA[0]) * deltaTime * TANK_SPEED;
    tankY[0] -= velGreen.y * cos(tankA[0]) * deltaTime * TANK_SPEED;
    if (tankX[0] < TANK_HIT_RADIUS)
      tankX[0] = TANK_HIT_RADIUS;
    if (tankY[0] < TANK_HIT_RADIUS)
      tankY[0] = TANK_HIT_RADIUS;
    if (tankX[0] > width - TANK_HIT_RADIUS)
      tankX[0] = width - TANK_HIT_RADIUS;
    if (tankY[0] > height - TANK_HIT_RADIUS)
      tankY[0] = height - TANK_HIT_RADIUS;

    // Update Red Tank Position & Rotation
    tankA[1] += velRed.x * deltaTime * TANK_ROTATION_SPEED;
    tankX[1] += velRed.y * sin(tankA[1]) * deltaTime * TANK_SPEED;
    tankY[1] -= velRed.y * cos(tankA[1]) * deltaTime * TANK_SPEED;
    if (tankX[1] < TANK_HIT_RADIUS)
      tankX[1] = TANK_HIT_RADIUS;
    if (tankY[1] < TANK_HIT_RADIUS)
      tankY[1] = TANK_HIT_RADIUS;
    if (tankX[1] > width - TANK_HIT_RADIUS)
      tankX[1] = width - TANK_HIT_RADIUS;
    if (tankY[1] > height - TANK_HIT_RADIUS)
      tankY[1] = height - TANK_HIT_RADIUS;

    if (totalTime >= 1.0f) {
      int fps = static_cast<int>(frameCount / totalTime);
      stringstream ss;
      ss << "FPS: " << fps;
      fpsText.setString(ss.str());
      frameCount = 0;
      totalTime = 0.0f;
    }

    // --- RENDER ---
    window.clear();
    window.draw(bg);

    for (int i = lowerTank; i <= upperTank; i++) {
      if (isTankGreen[i]) {
        greenTank.setPosition({tankX[i], tankY[i]});
        greenTank.setRotation(radians(tankA[i]));
        window.draw(greenTank);
      } else {
        redTank.setPosition({tankX[i], tankY[i]});
        redTank.setRotation(radians(tankA[i]));
        window.draw(redTank);
      }
    }

    for (int i = 0; i < 10; i++) {
      Sprite rockRef = i % 3 == 0 ? rock1 : (i % 3 == 1 ? rock2 : rock3);
      rockRef.setPosition(rocks[i]);
      window.draw(rockRef);
    }

    window.draw(fpsText);
    window.display();
  }

  return 0;
}
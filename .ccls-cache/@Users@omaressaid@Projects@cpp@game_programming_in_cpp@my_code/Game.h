#include "SDL2/SDL.h"

struct Vector2 {
  float x;
  float y;
};

class Game {
public:
  Game();
  bool Initialize(); // Initialize the game.
  void RunLoop();    // Run the game loop until the game is over.
  void Shutdown();   // Shutdown the game.
private:
  // Helper functions for the game loop
  void ProcessInput();
  void UpdateGame();
  void GenerateOutput();

  // Window created by SDL
  SDL_Window *mWindow;

  // Game should continue to run
  bool mIsRunning;

  Uint32 mTicksCount;

  SDL_Renderer *mRenderer;

  Vector2 mPaddlePos;
  Vector2 mBallPos;
  Vector2 mBallVel;
  int mPaddleDir;
};

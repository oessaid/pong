#include "Game.h"
#include "SDL2/SDL.h"

const int thickness = 15, paddleHeight = thickness * 10;

Game::Game() : mWindow(nullptr), mIsRunning(true), mTicksCount(0) {
  mPaddlePos = {0, 768 / 2};
  mBallPos = {1024 / 2, 768 / 2};
  mBallVel = {-200.0f, 235.0f};
}

bool Game::Initialize() {
  int sdl_result = SDL_Init(SDL_INIT_VIDEO);
  if (sdl_result != 0) {
    SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
    return false;
  }
  // If SDL initializes successfully, we create a window.
  mWindow = SDL_CreateWindow("Pong", 100, 100, 1024, 768, 0);
  if (!mWindow) {
    SDL_Log("Failed to create the window: %s", SDL_GetError());
    return false;
  }
  mRenderer = SDL_CreateRenderer(
      mWindow, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!mRenderer) {
    SDL_Log("Failed to initialize the renderer: %s", SDL_GetError());
    return false;
  }
  return true;
}

void Game::Shutdown() {
  SDL_DestroyRenderer(mRenderer);
  SDL_DestroyWindow(mWindow);
  SDL_Quit();
}

void Game::RunLoop() {
  while (mIsRunning) {
    ProcessInput();
    UpdateGame();
    GenerateOutput();
  }
}

void Game::ProcessInput() {
  SDL_Event event;
  // While there are still events in the queue
  while (SDL_PollEvent(&event)) {
    switch (event.type) {
    // Handle different event types here
    case SDL_QUIT:
      mIsRunning = false;
      break;
    }
  }

  // Get state of the keyboard
  const Uint8 *state = SDL_GetKeyboardState(NULL);
  // If escape is pressed, end loop
  if (state[SDL_SCANCODE_ESCAPE])
    mIsRunning = false;

  mPaddleDir = 0;
  if (state[SDL_SCANCODE_J])
    mPaddleDir += 1;
  if (state[SDL_SCANCODE_K])
    mPaddleDir -= 1;
}

void Game::UpdateGame() {
  // Frame limiting: for a target frame rate of 60 FPS, the target time
  // is 1s/60 ~ 16.6ms. If a frame completes after only 15ms, frame limiting
  // says to wait an additonal 1.6ms to meet the target time.
  // The line below ensures that at least 16ms elapses between frames.
  while (!SDL_TICKS_PASSED(SDL_GetTicks(), mTicksCount + 16))
    ;
  // Delta time as the the difference in ticks from the last frame
  // (converted to seconds)
  float deltaTime = (SDL_GetTicks() - mTicksCount) / 1000.0f;
  // Update tick counts (for next frame)
  mTicksCount = SDL_GetTicks();

  // Clamp maximum delta time: to prevent a jump too far forward while
  // stepping through the code in a debugger for example.
  if (deltaTime > 0.05f)
    deltaTime = 0.05;

  // Update objects in game world as function of delta time
  if (mPaddleDir != 0) {
    // Update the paddle's vertical position based on the direction
    // (if any) given by the player.
    mPaddlePos.y += mPaddleDir * 300.0f * deltaTime;
    // Prevent the paddle from going off-screen:
    // if the paddle's vertical position is less than the wall's
    // thickness plus half the paddle height, we constrain it to
    // stay on screen.
    // if the paddle's vertical position is more than the screen height
    // minus the wall's thickness and half the paddle's height, we constrain
    // it as well.
    if (mPaddlePos.y < (paddleHeight / 2.0f + thickness))
      mPaddlePos.y = paddleHeight / 2.0f + thickness;
    else if (mPaddlePos.y > (768 - paddleHeight / 2 - thickness))
      mPaddlePos.y = 768 - paddleHeight / 2 - thickness;
  }

  mBallPos.x += mBallVel.x * deltaTime;
  mBallPos.y += mBallVel.y * deltaTime;

  // Handle the ball colliding with walls
  // Top wall
  if (mBallPos.y <= thickness && mBallVel.y < 0) {
    mBallVel.y *= -1;
  }

  // Bottom wall
  if (mBallPos.y >= 768 - thickness && mBallVel.y > 0) {
    mBallVel.y *= -1;
  }

  // Right wall
  if (mBallPos.x >= 1024 - thickness && mBallVel.x > 0) {
    mBallVel.x *= -1;
  }

  // Colliding with the paddle
  float diff = abs(mBallPos.y - mPaddlePos.y);
  if (diff <= paddleHeight / 2 && mBallPos.x <= 25 && mBallPos.x >= 20 &&
      mBallVel.x < 0) {
    mBallVel.x *= -1;
  }

  // If the ball goes off screen, the game ends
  if (mBallPos.x < 0)
    mIsRunning = false;
}

void Game::GenerateOutput() {

  // We clear the back buffer to a color
  SDL_SetRenderDrawColor(mRenderer, 77, 116, 81, 255);
  SDL_RenderClear(mRenderer);
  // We draw the game scene
  SDL_SetRenderDrawColor(mRenderer, 255, 255, 255, 255);
  // Render the walls
  SDL_Rect topWall = {0, 0, 1024, thickness};
  SDL_Rect bottomWall = {0, 768 - thickness, 1024, thickness};
  SDL_Rect rightWall = {1024 - thickness, 0, thickness, 768};
  SDL_RenderFillRect(mRenderer, &topWall);
  SDL_RenderFillRect(mRenderer, &bottomWall);
  SDL_RenderFillRect(mRenderer, &rightWall);
  // Render the ball
  SDL_Rect ball = {static_cast<int>(mBallPos.x - thickness / 2),
                   static_cast<int>(mBallPos.y - thickness / 2), thickness,
                   thickness};
  SDL_RenderFillRect(mRenderer, &ball);
  // Render the paddle
  SDL_Rect paddle = {static_cast<int>(mPaddlePos.x + thickness / 2),
                     static_cast<int>(mPaddlePos.y - paddleHeight / 2),
                     thickness, paddleHeight};
  SDL_RenderFillRect(mRenderer, &paddle);
  // We swap the front and back buffers
  SDL_RenderPresent(mRenderer);
}

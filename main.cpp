#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <iostream>
#include <utility>
#include <vector>
using namespace std;
#ifdef LOGGING
#define LOG(x, y) cout << x << y << "\n"
#endif // LOGGING
#ifndef LOGGING
#define LOG(x, y)
#endif // !LOGGING
#define WINDOW_WIDTH 550
#define WINDOW_HEIGHT 550
#define RECT_SIZE 50
constexpr int block_num =
    (WINDOW_HEIGHT * WINDOW_WIDTH) / ((RECT_SIZE + 2) * (RECT_SIZE + 2));
vector<SDL_Rect> sdl_rects;
int dx = 2;
int dy = 1;

typedef struct {
  Uint8 r, g, b, a;
} RectColor;

void make_board(SDL_Renderer *renderer) {
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (int i = 0; i < (block_num * RECT_SIZE) / WINDOW_WIDTH; i++) {
    for (int j = 0; j < (block_num * RECT_SIZE) / WINDOW_HEIGHT; j++) {
      SDL_Rect new_rect = {.x = 5 + j * (RECT_SIZE + 5),
                           .y = 5 + i * (RECT_SIZE + 5),
                           .w = RECT_SIZE,
                           .h = RECT_SIZE};
      LOG(new_rect.x, new_rect.y);
      sdl_rects.push_back(new_rect);
      SDL_RenderDrawRect(renderer, &new_rect);
    }
  }
  SDL_RenderPresent(renderer);
}
void update_board(pair<int, int> &&coords, SDL_Renderer *renderer) {
  for (SDL_Rect rects : sdl_rects) {
    if ((coords.first > rects.x && coords.second > rects.y) &&
        (coords.first < rects.x + RECT_SIZE &&
         coords.second < rects.y + RECT_SIZE)) {
      SDL_RenderFillRect(renderer, &rects);
    }
  }
  SDL_RenderPresent(renderer);
}

void update_color(RectColor &color) {
  color.r -= 10;
  color.g += 10;
  color.b += 5;
}
void update(SDL_Rect &rect, RectColor &color) {
  if (rect.x + RECT_SIZE > WINDOW_WIDTH || rect.x < 0) {
    dx *= -1;
    update_color(color);
  }
  if (rect.y + RECT_SIZE > WINDOW_HEIGHT || rect.y < 0) {
    dy *= -1;
    update_color(color);
  }
  rect.x += dx;
  rect.y += dy;
}

void move(SDL_Renderer *renderer, SDL_Rect &rect, RectColor &color) {

  update(rect, color);
  SDL_SetRenderDrawColor(renderer, 10, 10, 0, 255);
  SDL_RenderFillRect(renderer, nullptr);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
  SDL_RenderFillRect(renderer, &rect);
  SDL_SetRenderDrawColor(renderer, 0, 255, 255, 255);
  SDL_RenderDrawRect(renderer, &rect);
  SDL_RenderPresent(renderer);
}

int main(int argc, char **argv) {
  if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
    cout << "Error in creating SDL instance\n";
    puts(SDL_GetError());
  }
  SDL_Window *win =
      SDL_CreateWindow("Test", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                       WINDOW_WIDTH, WINDOW_HEIGHT, SDL_WINDOW_SHOWN);
  if (!win)
    cout << "Error";

  SDL_Renderer *renderer =
      SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
  SDL_SetRenderDrawColor(renderer, 10, 10, 0, 255);
  SDL_RenderFillRect(renderer, nullptr);
  SDL_RenderClear(renderer);

  make_board(renderer);

  bool close = false;
  pair<int, int> start_pos = {1, 1};
  while (!close) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        close = true;
        break;
      case SDL_MOUSEBUTTONDOWN: {
        int x = event.button.x;
        int y = event.button.y;
        cout << "Clicked at : " << x << "," << y << endl;
        update_board(make_pair(x, y), renderer);
        break;
      }
      default:
        //   cout << event.key.keysym.scancode << endl;
        break;
      }
    }
  }
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}

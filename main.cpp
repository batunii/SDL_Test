#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <iostream>
using namespace std;
#define WINDOW_WIDTH 468
#define WINDOW_HEIGHT 324
#define RECT_SIZE 50
int dx = 2;
int dy = 1;

typedef struct {
  int r, g, b, a;
} RectColor;

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

  SDL_Rect red_rec = {.x = 20, .y = 40, .w = RECT_SIZE, .h = RECT_SIZE};
  RectColor rect_color = {.r = 255, .g = 0, .b = 0, .a = 255};
  SDL_SetRenderDrawColor(renderer, rect_color.r, rect_color.g, rect_color.b,
                         rect_color.a);
  SDL_RenderFillRect(renderer, &red_rec);
  SDL_RenderPresent(renderer);

  bool close = false;

  while (!close) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
        close = true;
        break;
      default:
        cout << event.key.keysym.scancode << endl;
      }
    }
    move(renderer, red_rec, rect_color);
    SDL_Delay(10);
  }
  SDL_DestroyWindow(win);
  SDL_Quit();

  return 0;
}

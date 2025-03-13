#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <chrono>
#include <cstdlib>
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
vector<vector<SDL_Rect *>> sdl_rects;
int dx = 2;
int dy = 1;

typedef struct {
  Uint8 r, g, b, a;
} RectColor;

void make_board(SDL_Renderer *renderer) {
  printf("printing %d blocks\n", block_num);
  SDL_RenderClear(renderer);
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  for (int i = 0; i < (block_num * (RECT_SIZE + 2)) / WINDOW_WIDTH; i++) {
    vector<SDL_Rect *> rows;
    for (int j = 0; j < (block_num * (RECT_SIZE + 2)) / WINDOW_HEIGHT; j++) {
      SDL_Rect *new_rect = (SDL_Rect *)malloc(sizeof(SDL_Rect));
      new_rect->x = 5 + j * (RECT_SIZE + 5);
      new_rect->y = 5 + i * (RECT_SIZE + 5);
      new_rect->w = RECT_SIZE;
      new_rect->h = RECT_SIZE;
      LOG(new_rect.x, new_rect.y);
      rows.push_back(new_rect);
      SDL_RenderDrawRect(renderer, new_rect);
    }
    sdl_rects.push_back(rows);
  }
  SDL_RenderPresent(renderer);
}
void update_board(pair<int, int> &&coords, SDL_Renderer *renderer) {
  for (vector<SDL_Rect *> rows : sdl_rects)
    for (SDL_Rect *rects : rows) {
      if ((coords.first > rects->x && coords.second > rects->y) &&
          (coords.first < rects->x + RECT_SIZE &&
           coords.second < rects->y + RECT_SIZE)) {
        printf("Found block!\n");
        break;
        // SDL_RenderFillRect(renderer, rects);
      }
    }
  // SDL_RenderPresent(renderer);
}

void update_board_binary(const pair<int, int> &&coords,
                         SDL_Renderer *renderer) {
  int start_x = 0;
  int end_x = sdl_rects[0].size();
  int x_mid = 0;
  int x_row = 0;
  const vector<SDL_Rect *> &rows = sdl_rects[0];
  while (start_x < end_x) {
    x_mid = (start_x + end_x) / 2;
    int x_0 = rows.at(x_mid)->x;
    int x_1 = x_0 + RECT_SIZE;
    if (coords.first > x_0 && coords.first < x_1) {
      x_row = x_mid;
      break;
    }
    if (coords.first < x_0 && coords.first < x_1)
      end_x = x_mid;
    if (coords.first > x_0 && coords.first > x_1)
      start_x = x_mid + 1;
  }

  int start_y = 0;
  int end_y = sdl_rects.size();
  int mid_y = 0;
  int col_y = 0;

  while (start_y < end_y) {
    mid_y = (start_y + end_y) / 2;
    int y0 = sdl_rects[mid_y][0]->y;
    int y1 = y0 + RECT_SIZE;
    if (coords.second > y0 && coords.second < y1) {
      col_y = mid_y;
      break;
    }
    if (coords.second > y0 && coords.second > y1)
      start_y = mid_y + 1;
    if (coords.second < y0 && coords.second < y1)
      end_y = mid_y;
  }

  printf("Found block at %d, %d \n", x_row, col_y);
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
        auto start1 = chrono::high_resolution_clock::now();
        update_board(make_pair(x, y), renderer);
        auto end1 = chrono::high_resolution_clock::now();
        auto duration1 =
            chrono::duration_cast<chrono::microseconds>(end1 - start1);
        cout << "It took " << duration1.count() << " seconds for linear"
             << endl;

        auto start2 = chrono::high_resolution_clock::now();
        update_board_binary(make_pair(x, y), renderer);
        auto end2 = chrono::high_resolution_clock::now();
        auto duration2 =
            chrono::duration_cast<chrono::microseconds>(end2 - start2);
        cout << "It took " << duration2.count() << " seconds for binary"
             << endl;
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

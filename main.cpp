#include <SDL2/SDL.h>
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_stdinc.h>
#include <SDL2/SDL_timer.h>
#include <cstdlib>
#include <iostream>
#include <unordered_set>
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

struct PairHash {
  size_t operator()(const pair<int, int> &p) const {
    return p.first ^ p.second;
  }
};
constexpr int block_num =
    (WINDOW_HEIGHT * WINDOW_WIDTH) / ((RECT_SIZE + 2) * (RECT_SIZE + 2));
vector<vector<SDL_Rect *>> sdl_rects;
unordered_set<pair<int, int>, PairHash> selected_rects;
void free_rects() {
  for (vector<SDL_Rect *> rect_row : sdl_rects) {
    for (SDL_Rect *rects : rect_row) {
      free(rects);
    }
  }
}

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

void update_board_binary(const pair<int, int> &&coords,
                         SDL_Renderer *renderer) {
  int start_x = 0;
  int end_x = sdl_rects[0].size();
  int mid_x = 0;
  int row_x = 0;
  const vector<SDL_Rect *> &rows = sdl_rects[0];
  while (start_x < end_x) {
    mid_x = (start_x + end_x) / 2;
    int x_0 = rows.at(mid_x)->x;
    int x_1 = x_0 + RECT_SIZE;
    if (coords.first > x_0 && coords.first < x_1) {
      row_x = mid_x;
      break;
    }
    if (coords.first < x_0 && coords.first < x_1)
      end_x = mid_x;
    if (coords.first > x_0 && coords.first > x_1)
      start_x = mid_x + 1;
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

  printf("Found block at %d, %d \n", row_x, col_y);

  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderFillRect(renderer, sdl_rects[col_y][row_x]);
  SDL_RenderPresent(renderer);
  selected_rects.emplace(col_y, row_x);
}

int emod(int a, int b) { return (((a % b) + b) % b); }

int calculate_neighbors(int y, int x) {
  int total_neighs = 0;
  int dir[] = {-1, 0, 1};

  for (int dirx : dir) {
    for (int diry : dir) {
      int new_x = emod(x + dirx, WINDOW_WIDTH);
      int new_y = emod(y + diry, WINDOW_HEIGHT);
      if (selected_rects.find(make_pair(new_x, new_y)) !=
          selected_rects.end()) {
        printf("Found a selected neighbor close to %d, %d : %d, %d \n", x, y,
               new_x, new_y);
        ++total_neighs;
      }
    }
  }
  return total_neighs;
}

void calculate_next_gol() {
  for (int y = 0; y < sdl_rects.size(); ++y) {
    for (int x = 0; x < sdl_rects[y].size(); ++x) {
      int neighbors = calculate_neighbors(y, x);
    }
  }
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
        free_rects();

        break;
      case SDL_MOUSEBUTTONDOWN: {
        int x = event.button.x;
        int y = event.button.y;
        cout << "Clicked at : " << x << "," << y << endl;
        update_board_binary(make_pair(x, y), renderer);
        break;
      }
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == 32)
          calculate_next_gol();
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

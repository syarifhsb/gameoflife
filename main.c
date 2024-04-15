#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <time.h>

#define CELL_SIZE 6

/* int check_neighbor(bool *cells, int i, int j) */ 
bool *cells, *next_cells;
void update_cell(int cell_n, int cell_m)
{

  for (int i = 0; i < cell_n; i++) {
    for (int j = 0; j < cell_m; j++) {

      int neighbors = 0;
      for (int di = -1; di <= 1; di++) {
        for (int dj = -1; dj <= 1; dj++) {
          if (dj == 0 && di == 0) continue;
          int x = j + dj;
          int y = i + di;
          neighbors += 0 <= y && y < cell_n && 0 <= x && x < cell_m ?
            cells[cell_n*y + x] : 0;
        }
      }

      if (cells[cell_n*i + j] && (neighbors < 2 || neighbors > 3)) {
        next_cells[cell_n*i + j] = false;
      } else if (!cells[cell_n*i + j] && neighbors == 3) {
        next_cells[cell_n*i + j] = true;
      } else {
        next_cells[cell_n*i +j] = cells[cell_n*i +j];
      }
    }
  }

  for (int i = 0; i < cell_n*cell_m; ++i) {
    cells[i] = next_cells[i];
  }
}

int main(int argc, char *argv[])
{
  SDL_Init( SDL_INIT_EVENTS | SDL_INIT_VIDEO );
  SDL_Window *window;
  SDL_Renderer *renderer;

  FILE *init_file = fopen("./init.txt", "r");
  assert(init_file != NULL);
  int init_n = 0;
  int init_m = 0;
  char c;

  while ((c = fgetc(init_file)) != EOF) {
    if (c == '\n') {
      init_n++;
    }
    if (init_n == 0) {
      init_m++;
    }
  }
  int ret = fseek(init_file, 0, SEEK_SET);
  assert(ret == 0);

  cells = calloc( init_m * init_n, sizeof( *cells ) );
  next_cells = calloc( init_m * init_n, sizeof( *next_cells ) );
  int counter = 0;

/* #if 0 */
/*   while ((c = fgetc(init_file)) != EOF) { */
/*     if (c == '\n') continue; */
/*     else if (c == '*') cells[counter] = true; */

/*     counter++; */
/*   } */
/* #else */
  srand(time(NULL));
  for (int i = 0; i < init_n*init_m; ++i) {
    if (rand() < 0.3 * RAND_MAX)
      cells[i] = true;
  }
/* #endif */

  window = SDL_CreateWindow( "Conway's game of life", 
      SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
      CELL_SIZE * init_m, CELL_SIZE * init_n,
      SDL_WINDOW_SHOWN );

  renderer = SDL_CreateRenderer( window, -1,
      SDL_RENDERER_ACCELERATED );

  SDL_Event e;
  int loop = 1;
  struct timespec t1, t2;
  clock_gettime(CLOCK_REALTIME, &t1);
  while ( loop ) {
    if ( SDL_PollEvent ( &e ) ) {
      switch ( e.type ) {
        case SDL_QUIT:
          loop = 0;
          goto quit_while;
          break;
      }
    }

    SDL_SetRenderDrawColor( renderer, 255, 255, 255, 255 );
    SDL_RenderClear( renderer );
    SDL_SetRenderDrawColor( renderer, 0, 0, 0, 255 );

    SDL_Rect cell;
    for ( int i = 0; i < init_n; i++) {
      for ( int j = 0; j < init_m; j++) {
        if ( cells[i * init_n + j] == true ) {
          cell.x = CELL_SIZE * j;
          cell.y = CELL_SIZE * i;
          cell.w = cell.h = CELL_SIZE;
          SDL_RenderFillRect( renderer, &cell );
        }
      }
    }
    SDL_RenderPresent( renderer );
    clock_gettime(CLOCK_REALTIME, &t2);
    if (((t2.tv_sec - t1.tv_sec) * 1000 + 
          (t2.tv_nsec - t1.tv_nsec) / 1000000) >= 100) {
      update_cell(init_n, init_m);
      t1 = t2;
    }
  }

quit_while:

  SDL_Quit();
  return 0;
}

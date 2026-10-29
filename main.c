#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define FPS 60
#define D_SC_W 600
#define D_SC_H 900
#define PAD 12
#define CELL_SIZE 48
#define FONT_SIZE 24

// centerx/y,matrix width and height,matrix offset x/y
int cx, cy, mw, mh, ox, oy;

Color B_RED = {244, 179, 127, 255};
Color B_GREEN = {166, 227, 157, 255};
Color B_GRAY = {191, 177, 161, 255};
Color B_WHITE = {250, 247, 243, 255};
Color B_YELLOW = {237, 204, 95, 255};
Color B_BLACK = {120, 110, 101, 255};

// Gameplay
#define N 4
#define I_2_N 5
int board[N][N];
static int score;

typedef enum { NONE, UP, DOWN, LEFT, RIGHT } Dir;
Dir dir;

typedef enum {
  PLAYING,
  WIN,
  LOSE,
} GameState;

GameState gameState;

void GenNewNums(int gn) {
  // gen some new "2" randomly
  int n = N * N;
  int r[n];
  // filter available slots
  int nb_av_slots = 0;
  for (int i = 0; i < n; i++) {
    int x = i % N;
    int y = i / N;
    if (!board[x][y]) {
      nb_av_slots++;
    }
  }
  for (int i = 0; i < nb_av_slots; i++) {
    if (i < gn) {
      r[i] = 2;
    } else {
      r[i] = 0;
    }
  }
  for (int i = nb_av_slots - 1; i > 0; i--) {
    // Pick a random index from 0 to i
    int j = rand() % (i + 1);
    // Swap arr[i] with the element at random index
    int t = r[j];
    r[j] = r[i];
    r[i] = t;
    // AND I DONT EVEN KNOW WHY YOU GOT DIFF RESULT WHEN USING XOR
    // r[i] ^= r[j];
    // r[j] ^= r[i];
    // r[i] ^= r[j];
  }
  for (int i = 0; i < nb_av_slots; i++) {
    int x = i % N;
    int y = i / N;
    if (board[x][y]) {
      continue;
    }
    board[x][y] = r[i];
  }
}

void InitScreen() {
  SetTargetFPS(FPS);
  InitWindow(D_SC_W, D_SC_H, "2048");
  cx = GetScreenWidth() / 2;
  cy = GetScreenHeight() / 2;
  mw = (2 * N - 3) * PAD + N * CELL_SIZE;
  mh = (2 * N - 3) * PAD + N * CELL_SIZE;
  ox = cx - mw / 2;
  oy = cy - mh / 2;
}

void InitBoard() {
  // clear the board
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      board[x][y] = 0;
    }
  }
  GenNewNums(I_2_N);
}

void InitGame() {
  gameState = PLAYING;
  score = 0;
  dir = NONE;
  srand(time(NULL));
  InitBoard();
}

void init() {
  InitScreen();
  InitGame();
}

void MoveLine(int line[N]) {
  int n = 0;
  for (int i = N - 2; i >= 0; i--) {
    if (line[i] == 0) {
      continue;
    }
    while (line[i + 1] == 0) {
      line[i + 1] = line[i];
      line[i] = 0;
      i++;
    }
    if (line[i] == line[i + 1]) {
      line[i] = 0;
      // n += line[i + 1];
      line[i + 1] *= 2;
    }
  }
}

// 0 2 2 2
// 2 2 0 0
// 0 2 2 2
// 2 2 0 0
void UpdateBoard(Dir dir) {
  int line[N];
  switch (dir) {
  case UP:
    for (int j = 0; j < N; j++) {
      for (int i = 0; i < N; i++) {
        line[i] = board[j][N - 1 - i];
      }
      MoveLine(line);
      for (int i = 0; i < N; i++) {
        board[j][N - 1 - i] = line[i];
      }
    }
    break;
  case DOWN:
    for (int j = 0; j < N; j++) {
      for (int i = 0; i < N; i++) {
        line[i] = board[j][i];
      }
      MoveLine(line);
      for (int i = 0; i < N; i++) {
        board[j][i] = line[i];
      }
    }
    break;
  case LEFT:
    for (int j = 0; j < N; j++) {
      for (int i = 0; i < N; i++) {
        line[i] = board[N - 1 - i][j];
      }
      MoveLine(line);
      for (int i = 0; i < N; i++) {
        board[i][j] = line[N - 1 - i];
      }
    }
    break;
  case RIGHT:
    for (int j = 0; j < N; j++) {
      for (int i = 0; i < N; i++) {
        line[i] = board[i][j];
      }
      MoveLine(line);
      for (int i = 0; i < N; i++) {
        board[i][j] = line[i];
      }
    }
    break;
  }
}

void UpdateScore() {
  score = 0;
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      score += board[x][y];
    }
  }
}

void update() {
  // handle player input
  if (IsKeyPressed(KEY_R)) {
    InitGame();
  }
  if (IsKeyPressed(KEY_W)) {
    dir = UP;
  }
  if (IsKeyPressed(KEY_S)) {
    dir = DOWN;
  }
  if (IsKeyPressed(KEY_A)) {
    dir = LEFT;
  }
  if (IsKeyPressed(KEY_D)) {
    dir = RIGHT;
  }
  switch (dir) {
  case UP:
    UpdateBoard(UP);
    GenNewNums(3);
    dir = NONE;
    break;
  case DOWN:
    UpdateBoard(DOWN);
    GenNewNums(3);
    dir = NONE;
    break;
  case LEFT:
    UpdateBoard(LEFT);
    GenNewNums(3);
    dir = NONE;
    break;
  case RIGHT:
    UpdateBoard(RIGHT);
    GenNewNums(3);
    dir = NONE;
    break;
  case NONE:
  }
  UpdateScore();
}

void DrawCell(int gx, int gy) {
  DrawRectangle(ox + (PAD + CELL_SIZE) * gx + PAD,
                oy + (PAD + CELL_SIZE) * gy + PAD, CELL_SIZE, CELL_SIZE,
                B_WHITE);
  if (board[gx][gy])
    DrawText(TextFormat("%d", board[gx][gy]), ox + (PAD + CELL_SIZE) * gx + PAD,
             oy + (PAD + CELL_SIZE) * gy + PAD, FONT_SIZE, B_BLACK);
}

void DrawMaxtrix() {
  // Draw The Board
  DrawRectangle(ox, oy, mw, mh, B_GRAY);
  for (int y = 0; y < N; y++) {
    for (int x = 0; x < N; x++) {
      DrawCell(x, y);
    }
  }
}

void DrawInfo() {
  // Draw Score
  DrawText(TextFormat("Score: %d", score), ox, oy - 44, FONT_SIZE, B_WHITE);

  // Draw Debug

  // DrawText(TextFormat("DIR: %d", dir), 0, 400, 32, B_WHITE);
}

void draw() {
  BeginDrawing();
  ClearBackground(B_YELLOW);
  switch (gameState) {
  case PLAYING:
    DrawInfo();
    DrawMaxtrix();
  case LOSE:
  case WIN:
  }
  EndDrawing();
}

int main() {
  init();
  while (!WindowShouldClose()) {
    update();
    draw();
  }
  return 0;
}

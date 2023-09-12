#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <Windows.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>

#define SCALE 15
#define WIDTH (SCALE*2)
#define HEIGHT (SCALE)

// Struct to hold point information
typedef struct SnakePoints{
  int x;
  int y;
}Point;

int size = 1;
Point *snake;
Point apple = {WIDTH/2, HEIGHT/2};
char board[WIDTH*HEIGHT];
bool increaseSnake = false;

enum direction {
  Up,
  Right,
  Down,
  Left
};

void clearScreen()
{
  printf("\e[?25l");
  const char *CLEAR_SCREEN_ANSI = "\e[1;1H\e[2J";
  write(STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);
}

int switchDirection(char ch)
{
  switch (ch) {
  case 'w':
    return 0;
  case 'd':
    return 1;
  case 's':
    return 2;
  case 'a':
    return 3;
  default:
    return -1;
  }
}

void moveSnake(int direction)
{
  if (increaseSnake) {
    size += 1;
    snake = realloc(snake, size*sizeof(Point));
    if (snake == NULL) {
      printf("Memory could not be allocated for snake\n");
      exit(0);
    }
  }
  Point prevPosition = *snake;
  Point newHead;
  switch (direction) {
  case Up:
    newHead.x = prevPosition.x;
    newHead.y = prevPosition.y - 1;
    *snake = newHead;
    break;
    
  case Right:
    newHead.x = prevPosition.x + 1;
    newHead.y = prevPosition.y;
    *snake = newHead;
    break;

  case Down:
    newHead.x = prevPosition.x;
    newHead.y = prevPosition.y + 1;
    *snake = newHead;
    break;

  case Left:
    newHead.x = prevPosition.x - 1;
    newHead.y = prevPosition.y;
    *snake = newHead;
    break;
  }

  Point temp;
  // Iterate through all parts of the snake
  for (int i = 1; i < size; ++i) {
    temp = snake[i]; 
    snake[i] = prevPosition;

    // Check if we are not in last index of snake
    if (i != size - 1) {
      prevPosition = temp;
    }
    else {
      if (increaseSnake) {
	increaseSnake = false;
      }
    }
  }
}

void increaseSize()
{
  increaseSnake = true;
}

bool checkIfApple() {
  Point head = *snake;
  if (board[head.y*WIDTH + head.x] == '@') return true;
  return false;
}

bool checkOutOfBounds()
{
  Point head = *snake;
  //test collision with boundary
  if ((head.x < 0 || head.y < 0) || (head.x > WIDTH-1 || head.y > HEIGHT-1)) {
    return true;
  }
  return false;
}

bool checkCollision(int direction) { 
  Point desiredPosition;
  switch(direction) {
  case Up:
    desiredPosition = *snake;
    desiredPosition.y = desiredPosition.y - 1;
    break;
  case Right:
    desiredPosition = *snake;
    desiredPosition.x = desiredPosition.x + 1;
    break;
  case Down:
    desiredPosition = *snake;
    desiredPosition.y = desiredPosition.y + 1;
    break;
  case Left:
    desiredPosition = *snake;
    desiredPosition.x = desiredPosition.x - 1;
    break;
  }
  Point body;
  for (int i = 0; i < size; ++i) {
    body = snake[i];
    if (body.x == desiredPosition.x && body.y == desiredPosition.y) return true;
  }
  return false;
}

void generateApple()
{
  Point newApple;
  int x = rand() % WIDTH;
  int y = rand() % HEIGHT;
  while (board[y*WIDTH + x] == 'O') {
    x = rand() % WIDTH;
    y = rand() % HEIGHT;
  }
  newApple.x = x;
  newApple.y = y;
  apple = newApple;
}

void printBoard()
{
  for (int i = 0; i < WIDTH + 2; ++i) {
    putc('-', stdout);
  }
  putc('\n', stdout);
  // Iterate through snake
  for (int i = 0; i < size; ++i) {
    board[snake[i].y*WIDTH + snake[i].x] = 'X';
  }

  board[apple.y*WIDTH + apple.x] = '@';

  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
      if (x == 0) {
	putc('|', stdout);
      }
      putc(board[y*WIDTH + x], stdout);
    }
    putc('|', stdout);
    putc('\n', stdout);
  }
  for (int i = 0; i < WIDTH + 2; ++i) {
    putc('-', stdout);
  }
  putc('\n', stdout);
}

void clearBoard()
{
  for (int y = 0; y < HEIGHT; ++y) {
    for (int x = 0; x < WIDTH; ++x) {
	board[y*WIDTH + x] = ' ';
    }
  }
}

bool goodMemoryPtr()
{
  if (snake == NULL) return false;
  else return true;
}

int main(void)
{
  snake = (Point*)calloc(size, sizeof(Point));
  Point start = {2, 2};
  
  if (!goodMemoryPtr) {
    printf("Ptr wasn't created for snake\n");
    exit(0);
  }
  
  snake[0] = start;
  srand(time(NULL));
  clearBoard();
  generateApple();
  unsigned int score = 0;
  int direction = Right;
  bool alive = true;
  
  while(alive)
  {
    
    // Clear the screen
    system("cls");

    // Check if we encountered an apple then generate a new apple
    if (checkIfApple()) {
      increaseSize();
      score += 100;
      generateApple();
    }
    
    // Clear and Print the state of game board
    clearBoard();
    printBoard();
    
    // Check if alive
    if (checkOutOfBounds()) {
      alive = false;
      break;
    }

    // Sleep cpu to adjust game speed
    Sleep(70);
    
    // Check if key was pressed, then change direction
    if (kbhit()) {
      char ch = getch();
      if (isspace(ch)) {
	alive = false;
	break;
      }
      int selectedDirection = switchDirection(ch);
      if (selectedDirection >= 0) {
	direction = selectedDirection;
      }
    }
    
    // Check for collision
    if (checkCollision(direction)) {
      alive = false;
    }
    
    //move snake in previous/new direction
    moveSnake(direction);
  }
  printf("Game Over!\n");
  printf("Your Score: %u\n", score);
  free(snake);
  return 0;
}

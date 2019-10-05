/* Template of the 7 wonders of the world of the 7 colors assigment. */

#include <stdio.h>     /* printf */
#include <stdlib.h>     //random
#include <time.h>     //to initialize random

//TODO : include BOARD_SIZE and NB_COLORS into the struct

/* We want a 30x30 board game by default */
#define BOARD_SIZE 30
#define NB_COLORS 7 // 7 colors (+ 2 players)
//#define MAX(a, b) ((a)<(b)? (b) : (a)) //max between two numbers




enum colors {
  A, B, C, D, E, F, G, PLAYER1, PLAYER2
};

typedef struct board_game board_game;

struct board_game {
  char* game;       //the board game
  enum colors turn; //the player who has to play next
  int unused;       //the number of positions which doesn't belong to any player
  int nb_player1;   //number of positions that player 1 has conquered
  int nb_player2;   //same for player 2
};





/** Represent the actual current board game
 *
 * NOTE: global variables are usually discouraged (plus encapsulation in
 *     an appropriate data structure would also be preferred), but don't worry.
 *     For this first assignment, no dinosaure will get you if you do that.
 */

char tab[BOARD_SIZE * BOARD_SIZE] = { 0 }; // Filled with zeros
board_game board = {tab, PLAYER1, BOARD_SIZE * BOARD_SIZE - 2, 1, 1};




/** Retrieves the color of a given board cell */
enum colors get_cell(int x, int y)
{
    return tab[y * BOARD_SIZE + x];
}

/** Changes the color of a given board cell */
void set_cell(int x, int y, enum colors color)
{
    tab[y * BOARD_SIZE + x] = color;
}

//Allows us to transform a color into its char representation
char colors_to_char(enum colors color) {
  switch(color) {       //TODO (maybe) : use a smarter way using pointer arithmetic and ascii representation
      case A:
        return 'A';
      case B:
        return 'B';
      case C:
        return 'C';
      case D:
        return 'D';
      case E:
        return 'E';
      case F:
        return 'F';
      case G:
        return 'G';
      case PLAYER1:
        return '^';
      case PLAYER2:
        return 'v';
      default:
        return 0;
  }
}

int colors_to_int(enum colors color) {
  switch (color) {
    case A:
      return 0;
    case B:
      return 1;
    case C:
      return 2;
    case D:
      return 3;
    case E:
      return 4;
    case F:
      return 5;
    case G:
      return 6;
    case PLAYER1:
      return 7;
    case PLAYER2:
      return 8;
  }
}

/** Prints the current state of the board on screen
 *
 * Implementation note: It would be nicer to do this with ncurse or even
 * SDL/allegro, but this is not really the purpose of this assignment.
 */
void print_board(void)
{
    int i, j;
    for (i = 0; i < BOARD_SIZE; i++) {
        for (j = 0; j < BOARD_SIZE; j++) {
          enum colors color =  get_cell(i, j);
          printf("%c", colors_to_char(color));
        }
        printf("\n");
    }
}

void init_board() { //initialize the board by picking a random color for each position
  for (int x = 0 ; x < BOARD_SIZE ; x++) {
    for (int y = 0 ; y < BOARD_SIZE ; y++) {
      int color_index = rand() % NB_COLORS;
      enum colors color = A;
      color += color_index;
      set_cell(x, y, color);
    }
  }
  set_cell(0, BOARD_SIZE - 1, PLAYER1); //first player
  set_cell(BOARD_SIZE - 1, 0, PLAYER2); //second player
}

//update one position next to (x, y) in the direction (dx, dy) where dx + dy = 1, dxdy = 0
void update_one_dir(int x, int y, int dx, int dy, enum colors color_played, int* has_done_something) {
  if (x + dx < BOARD_SIZE && x + dx >= 0 && y + dy < BOARD_SIZE && y + dy >= 0 && get_cell(x + dx, y + dy) == color_played) { //checking if the position isn't out of the board
    set_cell(x + dx, y + dy, board.turn);
    *has_done_something = 1;
    board.unused--;
    if (board.turn == PLAYER1) {
      board.nb_player1++;
    }
    else {
      board.nb_player2++;
    }
  }
}

int update_around_cell (int x, int y, enum colors color_played) { //returns 1 if something has changed, 0 otherwhise
  int has_done_something = 0;
  if (get_cell(x, y) == board.turn) {
    update_one_dir(x, y, -1, 0, color_played, &has_done_something);
    update_one_dir(x, y, 1, 0, color_played, &has_done_something);
    update_one_dir(x, y, 0, -1, color_played, &has_done_something);
    update_one_dir(x, y, 0, 1, color_played, &has_done_something);
  }
  return has_done_something;
}

void update_board(enum colors color_played) { //player = "^" ou "v" et color_played = A...G
  int has_done_something = 1;
  while (has_done_something) {
    has_done_something = 0;
    for (int x = 0 ; x < BOARD_SIZE ; x++) {
      for (int y = 0 ; y < BOARD_SIZE ; y++) {
        has_done_something = has_done_something || update_around_cell(x, y, color_played);
      }
    }
  }
  if (board.turn == PLAYER1) {
    board.turn = PLAYER2;
  }
  else {
    board.turn = PLAYER1;
  }
}

void full_game(enum colors (*p1)(), enum colors (*p2)()) { //game between two payers p1 and p2
  int k = 0;
  enum colors color;
  while(board.nb_player1 <= BOARD_SIZE * BOARD_SIZE / 2 && board.nb_player2 <= BOARD_SIZE * BOARD_SIZE / 2) {
    printf("\n\n");
    if (k == 0) {
      color = p1();
    }
    else {
      color = p2();
    }
    printf("player %d played color %c\n", k + 1, colors_to_char(color));
    update_board(color);
    print_board();
    printf("Player 1 has conquered %d percent(s)\n", board.nb_player1 * 100 / (BOARD_SIZE * BOARD_SIZE));
    printf("Player 2 has conquered %d percent(s)\n", board.nb_player2 * 100 / (BOARD_SIZE * BOARD_SIZE));
    k = (k + 1) % 2;
  }
  if (board.nb_player1 > board.nb_player2) {
    printf("Player 1 won the game! with %d percents of the board\n", 100 * board.nb_player1 / (BOARD_SIZE * BOARD_SIZE));
  }
  else {
    printf("Player 2 won the game! with %d percents of the board\n", 100 * board.nb_player2 / (BOARD_SIZE * BOARD_SIZE));
  }
}

enum colors player_random() {  //player who randomly chooses a color each turn
  int color_index = rand() % NB_COLORS;
  enum colors color = A;
  color += color_index;
  return color;

}

enum colors player_human() { //pretty self explanatory
  int color_index = 0;
  do {
    printf("Enter a number between 1 and 7 (for A to G) : ");
    scanf("%d", &color_index);
  } while (!(color_index > 0 && color_index <= 7));
  enum colors color = A;
  return color + (color_index - 1);
}

void add_adjacent_colors(int x, int y, int dx, int dy, int *colors_available) {
  if (x + dx < BOARD_SIZE && x + dx >= 0 && y + dy < BOARD_SIZE && y + dy >= 0) { //checking if the position isn't out of the board
    int i = colors_to_int(get_cell(x + dx, y + dy));
    colors_available[i] = 1;
  }
}


enum colors player_smart_random() { //player who randomly chooses a color among the ones that he can reach
  int colors_available[9] = {0};
  int nb_colors = 0;
  for (int x = 0 ; x < BOARD_SIZE ; x++) {    //we keep track of which colors whe can access and which we can't
    for (int y = 0 ; y < BOARD_SIZE ; y++) {
      if (get_cell(x, y) == board.turn) {
        add_adjacent_colors(x, y, -1, 0, colors_available);
        add_adjacent_colors(x, y, 1, 0, colors_available);
        add_adjacent_colors(x, y, 0, -1, colors_available);
        add_adjacent_colors(x, y, 0, 1, colors_available);
      }
    }
  }
  for (int i = 0 ; i < 7 ; i++) { //searching for the number of colors we can access
    if (colors_available[i]) {
      nb_colors++;
    }
  }
  int color_number = rand() % nb_colors + 1;
  int index = 0;
  int colors_seen = 0;
  enum colors color = A;
  while (colors_seen < color_number) {  //searching for the index of the color_indexth we can access
    if (colors_available[index]) {
      colors_seen++;
    }
    index++;
  }
  if (colors_seen > 0){
    return color + (index - 1);
  }
  else {  //we can't acess any color (the other player is blocking the way), so any color will have no effect
    return A;
  }
}

/** Program entry point */
int main(void)
{
    printf("\n\nWelcome to the 7 wonders of the world of the 7 colors\n"
	   "*****************************************************\n\n"
	   "Current board state:\n");

    srand(time(NULL)); //initialization of the random sequence
    init_board();
    print_board();
    full_game(player_random, player_smart_random);
    return 0; // Everything went well
}

// cs_snake.c
// Written by Mustafa Abdalruhman z5688640 on 21/10/2025
//
// Description: <INSERT DESCRIPTION OF PROGRAM>

// Provided Libraries
#include <stdio.h>

// Add your own #include statements below this line
#include <stdbool.h>

// Provided constants
#define ROWS 10 
#define COLS 10

#define NO_SNAKE -1

// Add your own #define constants below this line


// Provided enums
// Enum for features on the game board
enum entity {
    BODY_SEGMENT,
    EXIT_LOCKED,
    EXIT_UNLOCKED,
    WALL,
    APPLE_NORMAL,
    APPLE_REVERSE,
    APPLE_SPLIT,
    APPLE_EXPLODE,
    EXPLOSION,
    PASSAGE_UP,
    PASSAGE_DOWN,
    PASSAGE_LEFT,
    PASSAGE_RIGHT,
    PORTAL,
    EMPTY
};

// Add your own enums below this line

// Provided structs
// Represents a tile/cell on the game board
struct tile {
    enum entity entity;
};

// Add your own structs below this line


// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(
    struct tile board[ROWS][COLS], 
    int snake_row, 
    int snake_col
);
void print_game_statistics(
    int points,
    int moves_made,
    int num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
);
void print_game_statistics_with_rival(
    int original_points,
    int original_moves_made,
    int original_num_apples_eaten,
    int rival_points,
    int rival_moves_made,
    int rival_num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
);
void print_board_line(void);
void print_tile_spacer(void);
void print_board_header(void);

// Add your function prototypes below this line
bool is_position_in_bounds(int row, int col);
bool is_tile_empty(struct tile board[ROWS][COLS], int row, int col);
bool can_place_wall_segment(
    struct tile board[ROWS][COLS], int row, int col, int length, char direction
);
void place_wall_segment(
    struct tile board[ROWS][COLS], int row, int col, int length, char direction
);

// Provided sample main() function (you will need to modify this)
int main(void) {
    printf("Welcome to CS Snake!\n\n");

    struct tile board[ROWS][COLS];
    initialise_board(board);

    printf("--- Map Setup ---\n");

    char command;
    while (scanf(" %c", &command) == 1) {
        if (command == 's') {
            break;
        } else if (command == 'w') {
            int row;
            int col;
            if (scanf(" %d %d", &row, &col) != 2) {
                continue;
            }
            if (!is_position_in_bounds(row, col)) {
                printf(
                    "ERROR: Invalid position, %d %d is out of bounds!\n",
                    row,
                    col
                );
                continue;
            }
            if (!is_tile_empty(board, row, col)) {
                printf(
                    "ERROR: Invalid tile, %d %d is occupied!\n", row, col
                );
                continue;
            }
            board[row][col].entity = WALL;
        } else if (command == 'e') {
            int row;
            int col;
            if (scanf(" %d %d", &row, &col) != 2) {
                continue;
            }
            if (!is_position_in_bounds(row, col)) {
                printf(
                    "ERROR: Invalid position, %d %d is out of bounds!\n",
                    row,
                    col
                );
                continue;
            }
            if (!is_tile_empty(board, row, col)) {
                printf(
                    "ERROR: Invalid tile, %d %d is occupied!\n", row, col
                );
                continue;
            }
            board[row][col].entity = EXIT_LOCKED;
        } else if (command == 'a') {
            char apple_type;
            int row;
            int col;
            if (scanf(" %c %d %d", &apple_type, &row, &col) != 3) {
                continue;
            }
            if (apple_type != 'n') {
                continue;
            }
            if (!is_position_in_bounds(row, col)) {
                printf(
                    "ERROR: Invalid position, %d %d is out of bounds!\n",
                    row,
                    col
                );
                continue;
            }
            if (!is_tile_empty(board, row, col)) {
                printf(
                    "ERROR: Invalid tile, %d %d is occupied!\n", row, col
                );
                continue;
            }
            board[row][col].entity = APPLE_NORMAL;
        } else if (command == 'W') {
            char direction;
            int row;
            int col;
            int length;
            if (scanf(" %c %d %d %d", &direction, &row, &col, &length) != 4) {
                continue;
            }
            if (!is_position_in_bounds(row, col)) {
                printf(
                    "ERROR: Invalid position, %d %d is out of bounds!\n",
                    row,
                    col
                );
                continue;
            }
            if (!can_place_wall_segment(board, row, col, length, direction)) {
                continue;
            }
            place_wall_segment(board, row, col, length, direction);
        } else {
            // Ignore unknown commands
        }
    }

    print_board(board, NO_SNAKE, NO_SNAKE);

    printf("--- Spawning Snake ---\n");

    int snake_row;
    int snake_col;
    while (1) {
        printf("Enter the snake's starting position: ");
        if (scanf(" %d %d", &snake_row, &snake_col) != 2) {
            continue;
        }
        if (!is_position_in_bounds(snake_row, snake_col)) {
            printf(
                "ERROR: Invalid position, %d %d is out of bounds!\n",
                snake_row,
                snake_col
            );
            continue;
        }
        if (!is_tile_empty(board, snake_row, snake_col)) {
            printf(
                "ERROR: Invalid tile, %d %d is occupied!\n",
                snake_row,
                snake_col
            );
            continue;
        }
        break;
    }

    print_board(board, snake_row, snake_col);

    return 0;
}

// Add your function definitions below this line


bool is_position_in_bounds(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

bool is_tile_empty(struct tile board[ROWS][COLS], int row, int col) {
    return board[row][col].entity == EMPTY;
}

bool can_place_wall_segment(
    struct tile board[ROWS][COLS], int row, int col, int length, char direction
) {
    if (direction == 'h') {
        for (int offset = 0; offset < length; offset++) {
            int check_col = col + offset;
            if (!is_position_in_bounds(row, check_col)) {
                printf(
                    "ERROR: Invalid position, part of the wall is out of bounds!\n"
                );
                return false;
            }
            if (!is_tile_empty(board, row, check_col)) {
                printf(
                    "ERROR: Invalid tile, part of the wall is occupied!\n"
                );
                return false;
            }
        }
    } else if (direction == 'v') {
        for (int offset = 0; offset < length; offset++) {
            int check_row = row + offset;
            if (!is_position_in_bounds(check_row, col)) {
                printf(
                    "ERROR: Invalid position, part of the wall is out of bounds!\n"
                );
                return false;
            }
            if (!is_tile_empty(board, check_row, col)) {
                printf(
                    "ERROR: Invalid tile, part of the wall is occupied!\n"
                );
                return false;
            }
        }
    }
    return true;
}

void place_wall_segment(
    struct tile board[ROWS][COLS], int row, int col, int length, char direction
) {
    if (direction == 'h') {
        for (int offset = 0; offset < length; offset++) {
            board[row][col + offset].entity = WALL;
        }
    } else if (direction == 'v') {
        for (int offset = 0; offset < length; offset++) {
            board[row + offset][col].entity = WALL;
        }
    }
}


// =============================================================================
// EDITABLE Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tiles to EMPTY.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = EMPTY;
        }
    }
}

// Prints the game board, showing the snake's head position on the board.
void print_board(
    struct tile board[ROWS][COLS], 
    int snake_row, 
    int snake_col
) {
    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        print_tile_spacer();
        for (int col = 0; col < COLS; col++) {
            printf(" ");
            struct tile tile = board[row][col];
            if (row == snake_row && col == snake_col) {
                printf("^~^");
            } else if (tile.entity == WALL) {
                printf("|||");
            } else if (tile.entity == BODY_SEGMENT) {
                printf("###");
            } else if (tile.entity == EXIT_LOCKED) {
                printf("[X]");
            } else if (tile.entity == EXIT_UNLOCKED) {
                printf("[ ]");
            } else if (tile.entity == APPLE_NORMAL) {
                printf("(`)");
            } else if (tile.entity == APPLE_REVERSE) {
                printf("(R)");
            } else if (tile.entity == APPLE_SPLIT) {
                printf("(S)");
            } else if (tile.entity == PASSAGE_UP) {
                printf("^^^");
            } else if (tile.entity == PASSAGE_DOWN) {
                printf("vvv");
            } else if (tile.entity == PASSAGE_LEFT) {
                printf("<<<");
            } else if (tile.entity == PASSAGE_RIGHT) {
                printf(">>>");
            } else if (tile.entity == PORTAL) {
                printf("~O~");
            } else if (tile.entity == EXPLOSION) {
                printf("***");
            } else {
                printf("   ");
            }
        }
        printf("\n");
    }
    print_tile_spacer();
}

// =============================================================================
// DO NOT EDIT Provided Functions 
// =============================================================================

// Prints statistics about the game
void print_game_statistics(
    int points,
    int moves_made,
    int num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("============ Game Statistics ============\n");
    printf("Totals:\n");
    printf("  - Points: %d\n", points);
    printf("  - Moves Made: %d\n", moves_made);
    printf("  - Number of Apples Eaten: %d\n", num_apples_eaten);
    printf("Completion:\n");
    printf("  - Number of Apples Remaining: %d\n", num_apples_remaining);
    printf("  - Apple Completion Percentage: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("=========================================\n");
}

// Prints statistics about the game for both snakes when there are two players
void print_game_statistics_with_rival(
    int original_points,
    int original_moves_made,
    int original_num_apples_eaten,
    int rival_points,
    int rival_moves_made,
    int rival_num_apples_eaten,
    int num_apples_remaining,
    double completion_percentage,
    int maximum_points_remaining
) {
    printf("============ Game Statistics ============\n");
    printf("Original Snake Totals:\n");
    printf("  - Points: %d\n", original_points);
    printf("  - Moves Made: %d\n", original_moves_made);
    printf("  - Number of Apples Eaten: %d\n", original_num_apples_eaten);
    printf("Rival Snake Totals:\n");
    printf("  - Points: %d\n", rival_points);
    printf("  - Moves Made: %d\n", rival_moves_made);
    printf("  - Number of Apples Eaten: %d\n", rival_num_apples_eaten);
    printf("Completion:\n");
    printf("  - Number of Apples Remaining: %d\n", num_apples_remaining);
    printf("  - Apple Completion Percentage: %.1f%%\n", completion_percentage);
    printf("  - Maximum Points Remaining: %d\n", maximum_points_remaining);
    printf("=========================================\n");
}

// Helper function for print_board().
void print_board_header(void) {
    printf("|            C S _ S N A K E            |\n");
}

// Helper function for print_board(). 
void print_board_line(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("---+");
    }
    printf("\n");
}

// Helper function for print_board(). 
void print_tile_spacer(void) {
    printf("+");
    for (int col = 0; col < COLS; col++) {
        printf("   +");
    }
    printf("\n");
}


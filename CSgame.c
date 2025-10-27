// cs_snake.c
// Written by Mustafa Abdalruhman z5688640 on 21/10/2025
//
// Description: <INSERT DESCRIPTION OF PROGRAM>

// Provided Libraries
#include <stdio.h>

// Add your own #include statements below this line


// Provided constants
#define ROWS 10
#define COLS 10

#define NO_SNAKE -1

#define MAX_SNAKE_LENGTH (ROWS * COLS)

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
int is_position_in_bounds(int row, int col);
int is_tile_empty(struct tile board[ROWS][COLS], int row, int col);
int can_place_long_wall(
    struct tile board[ROWS][COLS], int start_row, int start_col,
    char direction, int length
);
void unlock_exits_if_needed(struct tile board[ROWS][COLS], int apples_remaining);
void print_current_statistics(
    int points,
    int moves_made,
    int apples_eaten,
    int apples_remaining,
    int apples_initial,
    int points_remaining_total
);
void reverse_snake(int snake_rows[MAX_SNAKE_LENGTH], int snake_cols[MAX_SNAKE_LENGTH],
    int snake_length);
void remove_tail_segments(
    struct tile board[ROWS][COLS],
    int snake_rows[MAX_SNAKE_LENGTH],
    int snake_cols[MAX_SNAKE_LENGTH],
    int *snake_length,
    int segments_to_remove
);


// Provided sample main() function (you will need to modify this)
int main(void) {
    printf("Welcome to CS Snake!\n\n");

    struct tile board[ROWS][COLS];
    initialise_board(board);

    printf("--- Map Setup ---\n");

    char command;
    int apples_initial = 0;
    int apples_remaining = 0;
    int points_remaining_total = 0;
    int snake_rows[MAX_SNAKE_LENGTH];
    int snake_cols[MAX_SNAKE_LENGTH];
    int snake_length = 0;
    while (scanf(" %c", &command) == 1) {
        if (command == 's' || command == 'S') {
            break;
        }

        if (command == 'w') {
            int row;
            int col;
            if (scanf("%d %d", &row, &col) != 2) {
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
                    "ERROR: Invalid tile, %d %d is occupied!\n",
                    row,
                    col
                );
                continue;
            }
            board[row][col].entity = WALL;
            continue;
        }

        if (command == 'e') {
            int row;
            int col;
            if (scanf("%d %d", &row, &col) != 2) {
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
                    "ERROR: Invalid tile, %d %d is occupied!\n",
                    row,
                    col
                );
                continue;
            }
            board[row][col].entity = EXIT_LOCKED;
            continue;
        }

        if (command == 'a') {
            char apple_type;
            int row;
            int col;
            if (scanf(" %c %d %d", &apple_type, &row, &col) != 3) {
                continue;
            }
            enum entity apple_entity;
            int apple_points = 0;
            if (apple_type == 'n') {
                apple_entity = APPLE_NORMAL;
                apple_points = 5;
            } else if (apple_type == 'r') {
                apple_entity = APPLE_REVERSE;
                apple_points = 10;
            } else if (apple_type == 's') {
                apple_entity = APPLE_SPLIT;
                apple_points = 20;
            } else {
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
                    "ERROR: Invalid tile, %d %d is occupied!\n",
                    row,
                    col
                );
                continue;
            }
            board[row][col].entity = apple_entity;
            apples_initial++;
            apples_remaining++;
            points_remaining_total += apple_points;
            continue;
        }

        if (command == 'p') {
            char passage_direction;
            int row;
            int col;
            if (scanf(" %c %d %d", &passage_direction, &row, &col) != 3) {
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
                    "ERROR: Invalid tile, %d %d is occupied!\n",
                    row,
                    col
                );
                continue;
            }

            enum entity passage_entity = EMPTY;
            if (passage_direction == '^') {
                passage_entity = PASSAGE_UP;
            } else if (passage_direction == 'v') {
                passage_entity = PASSAGE_DOWN;
            } else if (passage_direction == '<') {
                passage_entity = PASSAGE_LEFT;
            } else if (passage_direction == '>') {
                passage_entity = PASSAGE_RIGHT;
            } else {
                continue;
            }

            board[row][col].entity = passage_entity;
            continue;
        }

        if (command == 'W') {
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
            int placement_check = can_place_long_wall(board, row, col, direction, length);
            if (placement_check == 1) {
                printf(
                    "ERROR: Invalid position, part of the wall is out of bounds!\n"
                );
                continue;
            }
            if (placement_check == 2) {
                printf(
                    "ERROR: Invalid tile, part of the wall is occupied!\n"
                );
                continue;
            }

            int row_delta = 0;
            int col_delta = 0;
            if (direction == 'h') {
                col_delta = 1;
            } else if (direction == 'v') {
                row_delta = 1;
            }
            for (int index = 0; index < length; index++) {
                int target_row = row + row_delta * index;
                int target_col = col + col_delta * index;
                board[target_row][target_col].entity = WALL;
            }
            continue;
        }

        int ch = getchar();
        while (ch != '\n' && ch != EOF) {
            ch = getchar();
        }
    }

    print_board(board, NO_SNAKE, NO_SNAKE);

    printf("--- Spawning Snake ---\n");

    int snake_row;
    int snake_col;
    while (1) {
        printf("Enter the snake's starting position: ");
        if (scanf("%d %d", &snake_row, &snake_col) != 2) {
            return 0;
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

    board[snake_row][snake_col].entity = BODY_SEGMENT;
    int snake_head_row = snake_row;
    int snake_head_col = snake_col;
    snake_rows[0] = snake_row;
    snake_cols[0] = snake_col;
    snake_length = 1;
    int moves_made = 0;
    int apples_eaten = 0;
    int points = 0;

    print_board(board, snake_head_row, snake_head_col);

    printf("--- Gameplay Phase ---\n");

    while (1) {
        char gameplay_command;
        int read_result = scanf(" %c", &gameplay_command);
        if (read_result != 1) {
            printf("--- Quitting Game ---\n");
            return 0;
        }

        if (gameplay_command == 'p') {
            print_current_statistics(
                points,
                moves_made,
                apples_eaten,
                apples_remaining,
                apples_initial,
                points_remaining_total
            );
            continue;
        }

        int delta_row = 0;
        int delta_col = 0;
        if (gameplay_command == 'w') {
            delta_row = -1;
        } else if (gameplay_command == 'a') {
            delta_col = -1;
        } else if (gameplay_command == 's') {
            delta_row = 1;
        } else if (gameplay_command == 'd') {
            delta_col = 1;
        } else {
            continue;
        }

        moves_made++;

        int new_row = snake_head_row + delta_row;
        int new_col = snake_head_col + delta_col;

        int lose_game = 0;
        int win_game = 0;
        int should_add_segment = 0;
        int apple_value = 0;
        int consumed_reverse = 0;
        int consumed_split = 0;

        if (!is_position_in_bounds(new_row, new_col)) {
            lose_game = 1;
        } else {
            enum entity target_entity = board[new_row][new_col].entity;

            if (target_entity == WALL || target_entity == BODY_SEGMENT) {
                lose_game = 1;
            } else if (target_entity == EXIT_LOCKED) {
                if (apples_remaining == 0) {
                    board[new_row][new_col].entity = EXIT_UNLOCKED;
                    win_game = 1;
                } else {
                    lose_game = 1;
                }
            } else if (target_entity == EXIT_UNLOCKED) {
                win_game = 1;
            } else if (
                target_entity == PASSAGE_UP ||
                target_entity == PASSAGE_DOWN ||
                target_entity == PASSAGE_LEFT ||
                target_entity == PASSAGE_RIGHT
            ) {
                int correct_direction = 0;
                if (target_entity == PASSAGE_UP && delta_row == -1) {
                    correct_direction = 1;
                } else if (target_entity == PASSAGE_DOWN && delta_row == 1) {
                    correct_direction = 1;
                } else if (target_entity == PASSAGE_LEFT && delta_col == -1) {
                    correct_direction = 1;
                } else if (target_entity == PASSAGE_RIGHT && delta_col == 1) {
                    correct_direction = 1;
                }

                if (correct_direction) {
                    should_add_segment = 1;
                    board[new_row][new_col].entity = BODY_SEGMENT;
                } else {
                    lose_game = 1;
                }
            } else {
                should_add_segment = 1;

                if (target_entity == APPLE_NORMAL) {
                    apple_value = 5;
                    apples_eaten++;
                    apples_remaining--;
                    points += apple_value;
                    points_remaining_total -= apple_value;
                } else if (target_entity == APPLE_REVERSE) {
                    apple_value = 10;
                    apples_eaten++;
                    apples_remaining--;
                    points += apple_value;
                    points_remaining_total -= apple_value;
                    consumed_reverse = 1;
                } else if (target_entity == APPLE_SPLIT) {
                    apple_value = 20;
                    apples_eaten++;
                    apples_remaining--;
                    points += apple_value;
                    points_remaining_total -= apple_value;
                    consumed_split = 1;
                }

                if (points_remaining_total < 0) {
                    points_remaining_total = 0;
                }

                board[new_row][new_col].entity = BODY_SEGMENT;
            }
        }

        if (should_add_segment) {
            snake_rows[snake_length] = new_row;
            snake_cols[snake_length] = new_col;
            snake_length++;

            snake_head_row = new_row;
            snake_head_col = new_col;

            if (consumed_reverse) {
                reverse_snake(snake_rows, snake_cols, snake_length);
                snake_head_row = snake_rows[snake_length - 1];
                snake_head_col = snake_cols[snake_length - 1];
            }

            if (consumed_split) {
                int body_segments = snake_length - 1;
                int segments_to_remove = 0;
                if (body_segments > 0) {
                    if (body_segments % 2 == 0) {
                        segments_to_remove = body_segments / 2;
                    } else {
                        segments_to_remove = (body_segments + 1) / 2;
                    }
                }
                if (segments_to_remove > 0) {
                    remove_tail_segments(
                        board,
                        snake_rows,
                        snake_cols,
                        &snake_length,
                        segments_to_remove
                    );
                }
                snake_head_row = snake_rows[snake_length - 1];
                snake_head_col = snake_cols[snake_length - 1];
            }
        } else if (win_game || lose_game) {
            if (is_position_in_bounds(new_row, new_col)) {
                snake_head_row = new_row;
                snake_head_col = new_col;
            }
        }

        if (apples_remaining == 0) {
            unlock_exits_if_needed(board, apples_remaining);
        }

        print_board(board, snake_head_row, snake_head_col);

        if (lose_game) {
            printf("--- Game Over ---\n");
            printf("Guessss I was the prey today.\n");
            print_current_statistics(
                points,
                moves_made,
                apples_eaten,
                apples_remaining,
                apples_initial,
                points_remaining_total
            );
            return 0;
        }

        if (win_game) {
            printf("--- Game Over ---\n");
            printf("Ssslithered out with a full stomach!\n");
            print_current_statistics(
                points,
                moves_made,
                apples_eaten,
                apples_remaining,
                apples_initial,
                points_remaining_total
            );
            return 0;
        }
    }
}

// Add your function definitions below this line

int is_position_in_bounds(int row, int col) {
    return row >= 0 && row < ROWS && col >= 0 && col < COLS;
}

int is_tile_empty(struct tile board[ROWS][COLS], int row, int col) {
    return board[row][col].entity == EMPTY;
}

int can_place_long_wall(
    struct tile board[ROWS][COLS], int start_row, int start_col,
    char direction, int length
) {
    int row_delta = 0;
    int col_delta = 0;
    if (direction == 'h') {
        col_delta = 1;
    } else if (direction == 'v') {
        row_delta = 1;
    }

    for (int index = 0; index < length; index++) {
        int target_row = start_row + row_delta * index;
        int target_col = start_col + col_delta * index;
        if (!is_position_in_bounds(target_row, target_col)) {
            return 1;
        }
    }

    for (int index = 0; index < length; index++) {
        int target_row = start_row + row_delta * index;
        int target_col = start_col + col_delta * index;
        if (!is_tile_empty(board, target_row, target_col)) {
            return 2;
        }
    }

    return 0;
}

void unlock_exits_if_needed(struct tile board[ROWS][COLS], int apples_remaining) {
    if (apples_remaining != 0) {
        return;
    }

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (board[row][col].entity == EXIT_LOCKED) {
                board[row][col].entity = EXIT_UNLOCKED;
            }
        }
    }
}

void print_current_statistics(
    int points,
    int moves_made,
    int apples_eaten,
    int apples_remaining,
    int apples_initial,
    int points_remaining_total
) {
    double completion_percentage = 100.0;
    if (apples_initial != 0) {
        completion_percentage = 100.0 * apples_eaten / (double)apples_initial;
    }

    int maximum_points_remaining = points_remaining_total;

    print_game_statistics(
        points,
        moves_made,
        apples_eaten,
        apples_remaining,
        completion_percentage,
        maximum_points_remaining
    );
}

void reverse_snake(
    int snake_rows[MAX_SNAKE_LENGTH],
    int snake_cols[MAX_SNAKE_LENGTH],
    int snake_length
) {
    for (int index = 0; index < snake_length / 2; index++) {
        int opposite = snake_length - 1 - index;
        int temp_row = snake_rows[index];
        int temp_col = snake_cols[index];
        snake_rows[index] = snake_rows[opposite];
        snake_cols[index] = snake_cols[opposite];
        snake_rows[opposite] = temp_row;
        snake_cols[opposite] = temp_col;
    }
}

void remove_tail_segments(
    struct tile board[ROWS][COLS],
    int snake_rows[MAX_SNAKE_LENGTH],
    int snake_cols[MAX_SNAKE_LENGTH],
    int *snake_length,
    int segments_to_remove
) {
    if (segments_to_remove <= 0 || segments_to_remove >= *snake_length) {
        return;
    }

    for (int index = 0; index < segments_to_remove; index++) {
        int row = snake_rows[index];
        int col = snake_cols[index];
        board[row][col].entity = EMPTY;
    }

    int new_length = *snake_length - segments_to_remove;
    for (int index = 0; index < new_length; index++) {
        snake_rows[index] = snake_rows[index + segments_to_remove];
        snake_cols[index] = snake_cols[index + segments_to_remove];
    }

    *snake_length = new_length;
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


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
#define MAX_PORTAL_PAIRS 5

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

struct snake_state {
    int rows[MAX_SNAKE_LENGTH];
    int cols[MAX_SNAKE_LENGTH];
    int length;
};

struct portal_pair {
    int row1;
    int col1;
    int row2;
    int col2;
};
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
void unlock_exits_if_no_apples(struct tile board[ROWS][COLS], int apples_remaining);
void copy_board(
    struct tile source[ROWS][COLS],
    struct tile destination[ROWS][COLS]
);
void snake_initialise(struct snake_state *snake, int row, int col);
void snake_copy(struct snake_state *destination, const struct snake_state *source);
void snake_append_head(struct snake_state *snake, int row, int col);
void snake_reverse(struct snake_state *snake);
void snake_remove_tail_segments(
    struct snake_state *snake,
    int segments_to_remove,
    struct tile board[ROWS][COLS]
);
struct portal_pair find_portal_pair(
    struct portal_pair portal_pairs[MAX_PORTAL_PAIRS],
    int num_portal_pairs,
    int row,
    int col
);
int compute_points_remaining(
    int normal_apples_remaining,
    int reverse_apples_remaining,
    int split_apples_remaining
);


// Provided sample main() function (you will need to modify this)
int main(void) {
    printf("Welcome to CS Snake!\n\n");

    struct tile board[ROWS][COLS];
    initialise_board(board);

    struct snake_state snake;
    struct snake_state initial_snake;
    struct tile initial_board[ROWS][COLS];

    printf("--- Map Setup ---\n");

    int initial_normal_apples = 0;
    int initial_reverse_apples = 0;
    int initial_split_apples = 0;
    int normal_apples_remaining = 0;
    int reverse_apples_remaining = 0;
    int split_apples_remaining = 0;
    int total_initial_apples = 0;

    struct portal_pair portal_pairs[MAX_PORTAL_PAIRS];
    int num_portal_pairs = 0;

    char command;
    while (scanf(" %c", &command) == 1) {
        if (command == 's' || command == 'S') {
            break;
        } else if (command == 'w') {
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
        } else if (command == 'e') {
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
        } else if (command == 'a') {
            char apple_type;
            if (scanf(" %c", &apple_type) != 1) {
                continue;
            }
            int row;
            int col;
            if (scanf("%d %d", &row, &col) != 2) {
                continue;
            }
            if (apple_type != 'n' && apple_type != 'r' && apple_type != 's') {
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
            if (apple_type == 'n') {
                board[row][col].entity = APPLE_NORMAL;
                initial_normal_apples++;
                normal_apples_remaining++;
            } else if (apple_type == 'r') {
                board[row][col].entity = APPLE_REVERSE;
                initial_reverse_apples++;
                reverse_apples_remaining++;
            } else if (apple_type == 's') {
                board[row][col].entity = APPLE_SPLIT;
                initial_split_apples++;
                split_apples_remaining++;
            }
            total_initial_apples = initial_normal_apples
                + initial_reverse_apples
                + initial_split_apples;
        } else if (command == 'p') {
            char direction;
            int row;
            int col;
            if (scanf(" %c %d %d", &direction, &row, &col) != 3) {
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
            if (direction == '^') {
                board[row][col].entity = PASSAGE_UP;
            } else if (direction == 'v') {
                board[row][col].entity = PASSAGE_DOWN;
            } else if (direction == '<') {
                board[row][col].entity = PASSAGE_LEFT;
            } else if (direction == '>') {
                board[row][col].entity = PASSAGE_RIGHT;
            }
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
            int placement_check = can_place_long_wall(board, row, col, direction, length);
            if (placement_check == 1) {
                printf(
                    "ERROR: Invalid position, part of the wall is out of bounds!\n"
                );
                continue;
            } else if (placement_check == 2) {
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
        } else if (command == 't') {
            int first_row;
            int first_col;
            int second_row;
            int second_col;
            if (
                scanf(
                    "%d %d %d %d",
                    &first_row,
                    &first_col,
                    &second_row,
                    &second_col
                ) != 4
            ) {
                continue;
            }
            if (num_portal_pairs >= MAX_PORTAL_PAIRS) {
                printf(
                    "ERROR: Invalid placement, maximum number of portal pairs already reached!\n"
                );
                continue;
            }
            if (!is_position_in_bounds(first_row, first_col)) {
                printf(
                    "ERROR: Invalid position for first portal in pair, %d %d is out of bounds!\n",
                    first_row,
                    first_col
                );
                continue;
            }
            if (!is_tile_empty(board, first_row, first_col)) {
                printf(
                    "ERROR: Invalid tile for first portal in pair, %d %d is occupied!\n",
                    first_row,
                    first_col
                );
                continue;
            }
            if (!is_position_in_bounds(second_row, second_col)) {
                printf(
                    "ERROR: Invalid position for second portal in pair, %d %d is out of bounds!\n",
                    second_row,
                    second_col
                );
                continue;
            }
            if (
                (second_row != first_row || second_col != first_col) &&
                !is_tile_empty(board, second_row, second_col)
            ) {
                printf(
                    "ERROR: Invalid tile for second portal in pair, %d %d is occupied!\n",
                    second_row,
                    second_col
                );
                continue;
            }
            board[first_row][first_col].entity = PORTAL;
            board[second_row][second_col].entity = PORTAL;
            portal_pairs[num_portal_pairs].row1 = first_row;
            portal_pairs[num_portal_pairs].col1 = first_col;
            portal_pairs[num_portal_pairs].row2 = second_row;
            portal_pairs[num_portal_pairs].col2 = second_col;
            num_portal_pairs++;
        } else {
            int discard = getchar();
            while (discard != '\n' && discard != EOF) {
                discard = getchar();
            }
        }
    }

    print_board(board, NO_SNAKE, NO_SNAKE);

    printf("\n--- Spawning Snake ---\n");

    int snake_row = NO_SNAKE;
    int snake_col = NO_SNAKE;
    while (1) {
        printf("Enter the snake's starting position: ");
        int spawn_scan = scanf("%d %d", &snake_row, &snake_col);
        if (spawn_scan == EOF) {
            printf("\n--- Quitting Game ---\n");
            return 0;
        }
        if (spawn_scan != 2) {
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

    board[snake_row][snake_col].entity = BODY_SEGMENT;
    snake_initialise(&snake, snake_row, snake_col);
    snake_copy(&initial_snake, &snake);
    copy_board(board, initial_board);
    print_board(board, snake_row, snake_col);

    printf("\n--- Gameplay Phase ---\n");

    int points = 0;
    int moves_made = 0;
    int apples_eaten = 0;
    int total_apples_remaining = normal_apples_remaining
        + reverse_apples_remaining
        + split_apples_remaining;

    while (1) {
        char gameplay_command;
        int read_result = scanf(" %c", &gameplay_command);
        if (read_result != 1) {
            printf("--- Quitting Game ---\n");
            return 0;
        }

        if (gameplay_command == 'p') {
            int maximum_points_remaining = compute_points_remaining(
                normal_apples_remaining,
                reverse_apples_remaining,
                split_apples_remaining
            );
            double completion_percentage = 100.0;
            if (total_initial_apples > 0) {
                completion_percentage = 100.0 * apples_eaten / total_initial_apples;
            }
            print_game_statistics(
                points,
                moves_made,
                apples_eaten,
                total_apples_remaining,
                completion_percentage,
                maximum_points_remaining
            );
            continue;
        }

        if (gameplay_command == 'r') {
            printf("--- Resetting Map ---\n");
            copy_board(initial_board, board);
            snake_copy(&snake, &initial_snake);
            snake_row = snake.rows[snake.length - 1];
            snake_col = snake.cols[snake.length - 1];
            points = 0;
            moves_made = 0;
            apples_eaten = 0;
            normal_apples_remaining = initial_normal_apples;
            reverse_apples_remaining = initial_reverse_apples;
            split_apples_remaining = initial_split_apples;
            total_apples_remaining = normal_apples_remaining
                + reverse_apples_remaining
                + split_apples_remaining;
            print_board(board, snake_row, snake_col);
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

        int final_row = snake_row + delta_row;
        int final_col = snake_col + delta_col;
        int final_in_bounds = is_position_in_bounds(final_row, final_col);

        int snake_won = 0;
        int snake_lost = 0;
        int ate_reverse_apple = 0;
        int ate_split_apple = 0;

        if (!final_in_bounds) {
            snake_lost = 1;
        } else {
            enum entity target_entity = board[final_row][final_col].entity;

            if (target_entity == PORTAL) {
                struct portal_pair pair = find_portal_pair(
                    portal_pairs,
                    num_portal_pairs,
                    final_row,
                    final_col
                );
                int exit_row = pair.row1;
                int exit_col = pair.col1;
                if (pair.row1 == final_row && pair.col1 == final_col) {
                    exit_row = pair.row2;
                    exit_col = pair.col2;
                }
                final_row = exit_row + delta_row;
                final_col = exit_col + delta_col;
                final_in_bounds = is_position_in_bounds(final_row, final_col);
                if (!final_in_bounds) {
                    snake_lost = 1;
                } else {
                    target_entity = board[final_row][final_col].entity;
                }
            }

            if (final_in_bounds && !snake_lost) {
                if (
                    target_entity == PASSAGE_UP ||
                    target_entity == PASSAGE_DOWN ||
                    target_entity == PASSAGE_LEFT ||
                    target_entity == PASSAGE_RIGHT
                ) {
                    int allowed = 0;
                    if (target_entity == PASSAGE_UP && delta_row == -1 && delta_col == 0) {
                        allowed = 1;
                    } else if (target_entity == PASSAGE_DOWN && delta_row == 1 && delta_col == 0) {
                        allowed = 1;
                    } else if (target_entity == PASSAGE_LEFT && delta_row == 0 && delta_col == -1) {
                        allowed = 1;
                    } else if (target_entity == PASSAGE_RIGHT && delta_row == 0 && delta_col == 1) {
                        allowed = 1;
                    }
                    if (!allowed) {
                        snake_lost = 1;
                    } else {
                        board[final_row][final_col].entity = EMPTY;
                        target_entity = EMPTY;
                    }
                }
            }

            if (final_in_bounds && !snake_lost) {
                if (
                    target_entity == WALL ||
                    target_entity == EXIT_LOCKED ||
                    target_entity == BODY_SEGMENT
                ) {
                    snake_lost = 1;
                } else if (target_entity == EXIT_UNLOCKED) {
                    snake_won = 1;
                } else if (target_entity == APPLE_NORMAL) {
                    board[final_row][final_col].entity = EMPTY;
                    normal_apples_remaining--;
                    total_apples_remaining--;
                    apples_eaten++;
                    points += 5;
                } else if (target_entity == APPLE_REVERSE) {
                    board[final_row][final_col].entity = EMPTY;
                    reverse_apples_remaining--;
                    total_apples_remaining--;
                    apples_eaten++;
                    points += 10;
                    ate_reverse_apple = 1;
                } else if (target_entity == APPLE_SPLIT) {
                    board[final_row][final_col].entity = EMPTY;
                    split_apples_remaining--;
                    total_apples_remaining--;
                    apples_eaten++;
                    points += 20;
                    ate_split_apple = 1;
                }
            }
        }

        board[snake_row][snake_col].entity = BODY_SEGMENT;

        if (final_in_bounds) {
            snake_row = final_row;
            snake_col = final_col;
        }

        if (final_in_bounds && !snake_lost) {
            snake_append_head(&snake, final_row, final_col);

            if (ate_reverse_apple) {
                snake_reverse(&snake);
                snake_row = snake.rows[snake.length - 1];
                snake_col = snake.cols[snake.length - 1];
                if (snake.length > 0) {
                    board[snake.rows[0]][snake.cols[0]].entity = BODY_SEGMENT;
                }
            }

            if (ate_split_apple) {
                int body_segments = snake.length - 1;
                int segments_to_remove = (body_segments + 1) / 2;
                snake_remove_tail_segments(
                    &snake,
                    segments_to_remove,
                    board
                );
                snake_row = snake.rows[snake.length - 1];
                snake_col = snake.cols[snake.length - 1];
            }
        }

        unlock_exits_if_no_apples(board, total_apples_remaining);

        print_board(board, snake_row, snake_col);

        if (snake_won) {
            printf("--- Game Over ---\n");
            printf("Ssslithered out with a full stomach!\n");
            int maximum_points_remaining = compute_points_remaining(
                normal_apples_remaining,
                reverse_apples_remaining,
                split_apples_remaining
            );
            double completion_percentage = 100.0;
            if (total_initial_apples > 0) {
                completion_percentage = 100.0 * apples_eaten / total_initial_apples;
            }
            print_game_statistics(
                points,
                moves_made,
                apples_eaten,
                total_apples_remaining,
                completion_percentage,
                maximum_points_remaining
            );
            return 0;
        }

        if (snake_lost) {
            printf("--- Game Over ---\n");
            printf("Guessss I was the prey today.\n");
            int maximum_points_remaining = compute_points_remaining(
                normal_apples_remaining,
                reverse_apples_remaining,
                split_apples_remaining
            );
            double completion_percentage = 100.0;
            if (total_initial_apples > 0) {
                completion_percentage = 100.0 * apples_eaten / total_initial_apples;
            }
            print_game_statistics(
                points,
                moves_made,
                apples_eaten,
                total_apples_remaining,
                completion_percentage,
                maximum_points_remaining
            );
            return 0;
        }
    }

    return 0;
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

void unlock_exits_if_no_apples(struct tile board[ROWS][COLS], int apples_remaining) {
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

void copy_board(
    struct tile source[ROWS][COLS],
    struct tile destination[ROWS][COLS]
) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            destination[row][col] = source[row][col];
        }
    }
}

void snake_initialise(struct snake_state *snake, int row, int col) {
    snake->length = 1;
    snake->rows[0] = row;
    snake->cols[0] = col;
}

void snake_copy(struct snake_state *destination, const struct snake_state *source) {
    destination->length = source->length;
    for (int index = 0; index < source->length; index++) {
        destination->rows[index] = source->rows[index];
        destination->cols[index] = source->cols[index];
    }
}

void snake_append_head(struct snake_state *snake, int row, int col) {
    if (snake->length < MAX_SNAKE_LENGTH) {
        snake->rows[snake->length] = row;
        snake->cols[snake->length] = col;
        snake->length++;
    }
}

void snake_reverse(struct snake_state *snake) {
    for (int index = 0; index < snake->length / 2; index++) {
        int opposite_index = snake->length - 1 - index;
        int temp_row = snake->rows[index];
        int temp_col = snake->cols[index];
        snake->rows[index] = snake->rows[opposite_index];
        snake->cols[index] = snake->cols[opposite_index];
        snake->rows[opposite_index] = temp_row;
        snake->cols[opposite_index] = temp_col;
    }
}

void snake_remove_tail_segments(
    struct snake_state *snake,
    int segments_to_remove,
    struct tile board[ROWS][COLS]
) {
    int maximum_removable = snake->length - 1;
    if (segments_to_remove <= 0 || maximum_removable <= 0) {
        return;
    }
    if (segments_to_remove > maximum_removable) {
        segments_to_remove = maximum_removable;
    }

    for (int index = 0; index < segments_to_remove; index++) {
        int row = snake->rows[index];
        int col = snake->cols[index];
        board[row][col].entity = EMPTY;
    }

    int new_length = snake->length - segments_to_remove;
    for (int index = 0; index < new_length; index++) {
        snake->rows[index] = snake->rows[index + segments_to_remove];
        snake->cols[index] = snake->cols[index + segments_to_remove];
    }
    snake->length = new_length;
}

struct portal_pair find_portal_pair(
    struct portal_pair portal_pairs[MAX_PORTAL_PAIRS],
    int num_portal_pairs,
    int row,
    int col
) {
    for (int index = 0; index < num_portal_pairs; index++) {
        struct portal_pair pair = portal_pairs[index];
        if ((pair.row1 == row && pair.col1 == col) || (pair.row2 == row && pair.col2 == col)) {
            return pair;
        }
    }

    struct portal_pair fallback = {row, col, row, col};
    return fallback;
}

int compute_points_remaining(
    int normal_apples_remaining,
    int reverse_apples_remaining,
    int split_apples_remaining
) {
    return normal_apples_remaining * 5
        + reverse_apples_remaining * 10
        + split_apples_remaining * 20;
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


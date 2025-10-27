// cs_snake.c
// Written by Mustafa Abdalruhman z5688640 on 21/10/2025
//
// Description: Implementation of CS Snake with advanced setup and gameplay
// features including rival snakes, portals, passages, and explosive apples.

// Provided Libraries
#include <stdio.h>

// Add your own #include statements below this line


// Provided constants
#define ROWS 10
#define COLS 10

#define NO_SNAKE -1

#define MAX_SNAKE_LENGTH (ROWS * COLS)
#define MAX_PORTAL_PAIRS 5
#define MAX_EXPLOSIONS (ROWS * COLS)
#define EXPLOSION_ORIGINAL 1
#define EXPLOSION_RIVAL 2
#define EXPLOSION_NONE 0

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
    int value;
};

// Add your own structs below this line
struct portal_pairs {
    int rows[MAX_PORTAL_PAIRS][2];
    int cols[MAX_PORTAL_PAIRS][2];
    int count;
    int active[MAX_PORTAL_PAIRS];
};

struct snake_body {
    int rows[MAX_SNAKE_LENGTH];
    int cols[MAX_SNAKE_LENGTH];
    int length;
    int head_row;
    int head_col;
};

enum snake_id {
    SNAKE_ORIGINAL = 0,
    SNAKE_RIVAL = 1
};

struct snake_statistics {
    int points;
    int moves_made;
    int apples_eaten;
};

struct explosion {
    int row;
    int col;
    int radius;
    int current_step;
    int active;
};

struct game_state {
    struct tile board[ROWS][COLS];
    struct snake_body snake;
    struct snake_body rival_snake;
    struct portal_pairs portals;
    struct snake_statistics original_stats;
    struct snake_statistics rival_stats;
    int has_rival;
    enum snake_id current_turn;
    int apples_initial;
    int apples_remaining;
    int apples_removed_total;
    int points_remaining_total;
    struct explosion explosions[MAX_EXPLOSIONS];
    int explosion_count;
};

struct movement_context {
    int delta_row;
    int delta_col;
    int attempted_row;
    int attempted_col;
    int new_row;
    int new_col;
    int should_add_segment;
    int consumed_reverse;
    int consumed_split;
    int lose_game;
    int win_game;
    enum entity destination;
    struct snake_body *active_snake;
    struct snake_body *opponent_snake;
    struct snake_statistics *active_stats;
    struct snake_statistics *opponent_stats;
};

enum turn_status {
    TURN_CONTINUE,
    TURN_WIN,
    TURN_LOSE
};

// Provided Function Prototypes
void initialise_board(struct tile board[ROWS][COLS]);
void print_board(const struct game_state *state);
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
void initialise_game_state(struct game_state *state);
void run_setup_phase(struct game_state *state);
int process_setup_command(struct game_state *state, char command);
int handle_wall_command(struct game_state *state);
int handle_exit_command(struct game_state *state);
int handle_apple_command(struct game_state *state);
int handle_passage_command(struct game_state *state);
int handle_portal_command(struct game_state *state);
int handle_long_wall_command(struct game_state *state);
void discard_line(void);
int spawn_snake(
    struct game_state *state,
    struct snake_body *snake,
    struct snake_statistics *stats,
    const char *header,
    const char *prompt
);
int run_spawning_phase(struct game_state *state);
void save_initial_state(
    const struct game_state *state,
    struct game_state *initial_state
);
void run_gameplay_phase(
    struct game_state *state,
    const struct game_state *initial_state
);
int process_reset_command(
    struct game_state *state,
    const struct game_state *initial_state,
    char command
);
int process_statistics_command(const struct game_state *state, char command);
int process_movement_command(
    struct game_state *state,
    enum snake_id snake_id,
    char command,
    enum turn_status *status
);
int initialise_movement_context(
    struct game_state *state,
    enum snake_id snake_id,
    char command,
    struct movement_context *movement
);
void apply_portal_if_needed(
    struct game_state *state,
    struct movement_context *movement
);
void evaluate_destination(
    struct game_state *state,
    struct movement_context *movement
);
void handle_passage_tile(
    struct game_state *state,
    struct movement_context *movement
);
void handle_consumable_tile(
    struct game_state *state,
    struct movement_context *movement
);
void finalize_movement(
    struct game_state *state,
    struct movement_context *movement
);
int is_position_in_bounds(int row, int col);
int is_tile_empty(struct tile board[ROWS][COLS], int row, int col);
int can_place_long_wall(
    struct tile board[ROWS][COLS], int start_row, int start_col,
    char direction, int length
);
void unlock_exits_if_needed(struct game_state *state);
void print_current_statistics(const struct game_state *state);
void reverse_snake(
    int snake_rows[MAX_SNAKE_LENGTH],
    int snake_cols[MAX_SNAKE_LENGTH],
    int snake_length
);
void remove_tail_segments(
    struct tile board[ROWS][COLS],
    int snake_rows[MAX_SNAKE_LENGTH],
    int snake_cols[MAX_SNAKE_LENGTH],
    int *snake_length,
    int segments_to_remove
);
int find_portal_partner(
    const struct portal_pairs *portals,
    int row,
    int col,
    int *partner_row,
    int *partner_col
);
int is_movement_command(char command);
void start_explosion(struct game_state *state, int row, int col, int radius);
int advance_explosions(struct game_state *state);
void clear_explosion_ring(
    struct game_state *state,
    int center_row,
    int center_col,
    int distance
);
int apply_explosion_ring(
    struct game_state *state,
    int center_row,
    int center_col,
    int distance
);
int destroy_entity_by_explosion(
    struct game_state *state,
    int row,
    int col
);
void deactivate_portal_pair(struct game_state *state, int row, int col);
void update_points_for_destroyed_apple(
    struct game_state *state,
    enum entity apple_entity
);
int remove_snake_segment(struct snake_body *snake, int row, int col);
int absolute_value(int number);


// Provided sample main() function (you will need to modify this)
int main(void) {
    printf("Welcome to CS Snake!\n\n");

    struct game_state state;
    struct game_state initial_state;
    initialise_game_state(&state);

    printf("--- Map Setup ---\n");
    run_setup_phase(&state);
    print_board(&state);

    if (!run_spawning_phase(&state)) {
        return 0;
    }

    save_initial_state(&state, &initial_state);

    printf("--- Gameplay Phase ---\n");
    run_gameplay_phase(&state, &initial_state);

    return 0;
}

// Add your function definitions below this line
void initialise_game_state(struct game_state *state) {
    initialise_board(state->board);
    state->snake.length = 0;
    state->snake.head_row = NO_SNAKE;
    state->snake.head_col = NO_SNAKE;
    state->rival_snake.length = 0;
    state->rival_snake.head_row = NO_SNAKE;
    state->rival_snake.head_col = NO_SNAKE;
    state->portals.count = 0;
    for (int index = 0; index < MAX_PORTAL_PAIRS; index++) {
        state->portals.active[index] = 0;
    }
    state->original_stats.points = 0;
    state->original_stats.moves_made = 0;
    state->original_stats.apples_eaten = 0;
    state->rival_stats.points = 0;
    state->rival_stats.moves_made = 0;
    state->rival_stats.apples_eaten = 0;
    state->has_rival = 0;
    state->current_turn = SNAKE_ORIGINAL;
    state->apples_initial = 0;
    state->apples_remaining = 0;
    state->apples_removed_total = 0;
    state->points_remaining_total = 0;
    state->explosion_count = 0;
    for (int index = 0; index < MAX_EXPLOSIONS; index++) {
        state->explosions[index].active = 0;
        state->explosions[index].current_step = 0;
        state->explosions[index].radius = 0;
        state->explosions[index].row = 0;
        state->explosions[index].col = 0;
    }
}

void run_setup_phase(struct game_state *state) {
    char command;
    while (scanf(" %c", &command) == 1) {
        if (command == 's' || command == 'S') {
            if (command == 'S') {
                state->has_rival = 1;
            }
            return;
        }

        if (process_setup_command(state, command)) {
            continue;
        }

        discard_line();
    }
}

int process_setup_command(struct game_state *state, char command) {
    if (command == 'w') {
        return handle_wall_command(state);
    }
    if (command == 'e') {
        return handle_exit_command(state);
    }
    if (command == 'a') {
        return handle_apple_command(state);
    }
    if (command == 'p') {
        return handle_passage_command(state);
    }
    if (command == 't') {
        return handle_portal_command(state);
    }
    if (command == 'W') {
        return handle_long_wall_command(state);
    }

    return 0;
}

int handle_wall_command(struct game_state *state) {
    int row;
    int col;
    if (scanf("%d %d", &row, &col) != 2) {
        discard_line();
        return 1;
    }

    if (!is_position_in_bounds(row, col)) {
        printf(
            "ERROR: Invalid position, %d %d is out of bounds!\n",
            row,
            col
        );
        return 1;
    }
    if (!is_tile_empty(state->board, row, col)) {
        printf(
            "ERROR: Invalid tile, %d %d is occupied!\n",
            row,
            col
        );
        return 1;
    }

    state->board[row][col].entity = WALL;
    return 1;
}

int handle_exit_command(struct game_state *state) {
    int row;
    int col;
    if (scanf("%d %d", &row, &col) != 2) {
        discard_line();
        return 1;
    }

    if (!is_position_in_bounds(row, col)) {
        printf(
            "ERROR: Invalid position, %d %d is out of bounds!\n",
            row,
            col
        );
        return 1;
    }
    if (!is_tile_empty(state->board, row, col)) {
        printf(
            "ERROR: Invalid tile, %d %d is occupied!\n",
            row,
            col
        );
        return 1;
    }

    state->board[row][col].entity = EXIT_LOCKED;
    return 1;
}

int handle_apple_command(struct game_state *state) {
    char apple_type;
    if (scanf(" %c", &apple_type) != 1) {
        discard_line();
        return 1;
    }

    int row = 0;
    int col = 0;
    int radius = 0;
    enum entity apple_entity = EMPTY;
    int apple_points = 0;

    if (apple_type == 'e') {
        if (scanf("%d %d %d", &radius, &row, &col) != 3) {
            discard_line();
            return 1;
        }
        apple_entity = APPLE_EXPLODE;
        apple_points = 20;
    } else {
        if (scanf("%d %d", &row, &col) != 2) {
            discard_line();
            return 1;
        }
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
            return 1;
        }
    }

    if (!is_position_in_bounds(row, col)) {
        printf(
            "ERROR: Invalid position, %d %d is out of bounds!\n",
            row,
            col
        );
        return 1;
    }
    if (!is_tile_empty(state->board, row, col)) {
        printf(
            "ERROR: Invalid tile, %d %d is occupied!\n",
            row,
            col
        );
        return 1;
    }

    struct tile *target_tile = &state->board[row][col];
    target_tile->entity = apple_entity;
    target_tile->value = radius;
    state->apples_initial++;
    state->apples_remaining++;
    state->points_remaining_total += apple_points;
    return 1;
}

int handle_passage_command(struct game_state *state) {
    char passage_direction;
    int row;
    int col;
    if (scanf(" %c %d %d", &passage_direction, &row, &col) != 3) {
        discard_line();
        return 1;
    }

    if (!is_position_in_bounds(row, col)) {
        printf(
            "ERROR: Invalid position, %d %d is out of bounds!\n",
            row,
            col
        );
        return 1;
    }
    if (!is_tile_empty(state->board, row, col)) {
        printf(
            "ERROR: Invalid tile, %d %d is occupied!\n",
            row,
            col
        );
        return 1;
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
        return 1;
    }

    state->board[row][col].entity = passage_entity;
    return 1;
}

int handle_portal_command(struct game_state *state) {
    int row1;
    int col1;
    int row2;
    int col2;
    if (scanf("%d %d %d %d", &row1, &col1, &row2, &col2) != 4) {
        discard_line();
        return 1;
    }

    if (state->portals.count >= MAX_PORTAL_PAIRS) {
        printf(
            "ERROR: Invalid placement, maximum number of portal pairs "
            "already reached!\n"
        );
        return 1;
    }
    if (!is_position_in_bounds(row1, col1)) {
        printf(
            "ERROR: Invalid position for first portal in pair, %d %d "
            "is out of bounds!\n",
            row1,
            col1
        );
        return 1;
    }
    if (!is_tile_empty(state->board, row1, col1)) {
        printf(
            "ERROR: Invalid tile for first portal in pair, %d %d is "
            "occupied!\n",
            row1,
            col1
        );
        return 1;
    }
    if (!is_position_in_bounds(row2, col2)) {
        printf(
            "ERROR: Invalid position for second portal in pair, %d %d "
            "is out of bounds!\n",
            row2,
            col2
        );
        return 1;
    }
    if (
        (row1 != row2 || col1 != col2) &&
        !is_tile_empty(state->board, row2, col2)
    ) {
        printf(
            "ERROR: Invalid tile for second portal in pair, %d %d is "
            "occupied!\n",
            row2,
            col2
        );
        return 1;
    }

    state->board[row1][col1].entity = PORTAL;
    state->board[row2][col2].entity = PORTAL;
    state->portals.rows[state->portals.count][0] = row1;
    state->portals.cols[state->portals.count][0] = col1;
    state->portals.rows[state->portals.count][1] = row2;
    state->portals.cols[state->portals.count][1] = col2;
    state->portals.active[state->portals.count] = 1;
    state->portals.count++;
    return 1;
}

int handle_long_wall_command(struct game_state *state) {
    char direction;
    int row;
    int col;
    int length;
    if (scanf(" %c %d %d %d", &direction, &row, &col, &length) != 4) {
        discard_line();
        return 1;
    }

    if (!is_position_in_bounds(row, col)) {
        printf(
            "ERROR: Invalid position, %d %d is out of bounds!\n",
            row,
            col
        );
        return 1;
    }

    int placement_check = can_place_long_wall(
        state->board,
        row,
        col,
        direction,
        length
    );
    if (placement_check == 1) {
        printf(
            "ERROR: Invalid position, part of the wall is out of bounds!\n"
        );
        return 1;
    }
    if (placement_check == 2) {
        printf(
            "ERROR: Invalid tile, part of the wall is occupied!\n"
        );
        return 1;
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
        state->board[target_row][target_col].entity = WALL;
    }

    return 1;
}

void discard_line(void) {
    int leftover_char = getchar();
    while (leftover_char != '\n' && leftover_char != EOF) {
        leftover_char = getchar();
    }
}

int spawn_snake(
    struct game_state *state,
    struct snake_body *snake,
    struct snake_statistics *stats,
    const char *header,
    const char *prompt
) {
    printf("%s\n", header);

    while (1) {
        int row;
        int col;
        printf("%s", prompt);
        if (scanf("%d %d", &row, &col) != 2) {
            return 0;
        }
        if (!is_position_in_bounds(row, col)) {
            printf(
                "ERROR: Invalid position, %d %d is out of bounds!\n",
                row,
                col
            );
            continue;
        }
        if (!is_tile_empty(state->board, row, col)) {
            printf(
                "ERROR: Invalid tile, %d %d is occupied!\n",
                row,
                col
            );
            continue;
        }

        state->board[row][col].entity = BODY_SEGMENT;
        state->board[row][col].value = 0;
        snake->rows[0] = row;
        snake->cols[0] = col;
        snake->length = 1;
        snake->head_row = row;
        snake->head_col = col;
        stats->points = 0;
        stats->moves_made = 0;
        stats->apples_eaten = 0;
        return 1;
    }
}

int run_spawning_phase(struct game_state *state) {
    if (!spawn_snake(
            state,
            &state->snake,
            &state->original_stats,
            "--- Spawning Snake ---",
            "Enter the snake's starting position: "
        )) {
        return 0;
    }

    if (!state->has_rival) {
        print_board(state);
        state->current_turn = SNAKE_ORIGINAL;
        return 1;
    }

    if (!spawn_snake(
            state,
            &state->rival_snake,
            &state->rival_stats,
            "--- Spawning Rival Snake ---",
            "Enter the rival snake's starting position: "
        )) {
        return 0;
    }

    state->current_turn = SNAKE_ORIGINAL;
    print_board(state);
    return 1;
}

void save_initial_state(
    const struct game_state *state,
    struct game_state *initial_state
) {
    *initial_state = *state;
    initial_state->original_stats.points = 0;
    initial_state->original_stats.moves_made = 0;
    initial_state->original_stats.apples_eaten = 0;
    initial_state->rival_stats.points = 0;
    initial_state->rival_stats.moves_made = 0;
    initial_state->rival_stats.apples_eaten = 0;
    initial_state->apples_removed_total = 0;
    initial_state->current_turn = SNAKE_ORIGINAL;
    initial_state->explosion_count = 0;
    for (int index = 0; index < MAX_EXPLOSIONS; index++) {
        initial_state->explosions[index].active = 0;
        initial_state->explosions[index].current_step = 0;
        initial_state->explosions[index].radius = 0;
        initial_state->explosions[index].row = 0;
        initial_state->explosions[index].col = 0;
    }
}

void run_gameplay_phase(
    struct game_state *state,
    const struct game_state *initial_state
) {
    while (1) {
        char command;
        if (scanf(" %c", &command) != 1) {
            printf("--- Quitting Game ---\n");
            return;
        }

        if (process_reset_command(state, initial_state, command)) {
            continue;
        }
        if (process_statistics_command(state, command)) {
            continue;
        }

        if (!is_movement_command(command)) {
            continue;
        }

        enum snake_id active_id = state->current_turn;
        int explosion_status = advance_explosions(state);
        int active_mask = EXPLOSION_RIVAL;
        int opponent_mask = EXPLOSION_ORIGINAL;
        if (active_id == SNAKE_ORIGINAL) {
            active_mask = EXPLOSION_ORIGINAL;
            opponent_mask = EXPLOSION_RIVAL;
        }
        int opponent_destroyed =
            state->has_rival && (explosion_status & opponent_mask);

        if (explosion_status & active_mask) {
            print_board(state);
            printf("--- Game Over ---\n");
            printf("Guessss I was the prey today.\n");
            if (state->has_rival) {
                if (active_id == SNAKE_ORIGINAL) {
                    printf("Rival snake wins!\n");
                } else {
                    printf("Original snake wins!\n");
                }
            }
            print_current_statistics(state);
            return;
        }

        enum turn_status status = TURN_CONTINUE;
        if (!process_movement_command(state, active_id, command, &status)) {
            continue;
        }

        print_board(state);

        if (status == TURN_LOSE) {
            printf("--- Game Over ---\n");
            printf("Guessss I was the prey today.\n");
            if (state->has_rival) {
                if (active_id == SNAKE_ORIGINAL) {
                    printf("Rival snake wins!\n");
                } else {
                    printf("Original snake wins!\n");
                }
            }
            print_current_statistics(state);
            return;
        }
        if (status == TURN_WIN) {
            printf("--- Game Over ---\n");
            printf("Ssslithered out with a full stomach!\n");
            if (state->has_rival) {
                if (active_id == SNAKE_ORIGINAL) {
                    printf("Original snake wins!\n");
                } else {
                    printf("Rival snake wins!\n");
                }
            }
            print_current_statistics(state);
            return;
        }

        if (opponent_destroyed) {
            printf("--- Game Over ---\n");
            printf("Guessss I was the prey today.\n");
            if (state->has_rival) {
                if (active_id == SNAKE_ORIGINAL) {
                    printf("Original snake wins!\n");
                } else {
                    printf("Rival snake wins!\n");
                }
            }
            print_current_statistics(state);
            return;
        }

        if (state->has_rival) {
            if (state->current_turn == SNAKE_ORIGINAL) {
                state->current_turn = SNAKE_RIVAL;
            } else {
                state->current_turn = SNAKE_ORIGINAL;
            }
        }
    }
}

int process_reset_command(
    struct game_state *state,
    const struct game_state *initial_state,
    char command
) {
    if (command != 'r') {
        return 0;
    }

    printf("--- Resetting Map ---\n");
    *state = *initial_state;
    print_board(state);
    return 1;
}

int process_statistics_command(const struct game_state *state, char command) {
    if (command != 'p') {
        return 0;
    }

    print_current_statistics(state);
    return 1;
}

int process_movement_command(
    struct game_state *state,
    enum snake_id snake_id,
    char command,
    enum turn_status *status
) {
    struct movement_context movement;
    if (!initialise_movement_context(state, snake_id, command, &movement)) {
        return 0;
    }

    movement.active_stats->moves_made++;
    apply_portal_if_needed(state, &movement);
    evaluate_destination(state, &movement);
    finalize_movement(state, &movement);

    if (state->apples_remaining == 0) {
        unlock_exits_if_needed(state);
    }

    if (movement.lose_game) {
        *status = TURN_LOSE;
    } else if (movement.win_game) {
        *status = TURN_WIN;
    } else {
        *status = TURN_CONTINUE;
    }

    return 1;
}

int initialise_movement_context(
    struct game_state *state,
    enum snake_id snake_id,
    char command,
    struct movement_context *movement
) {
    movement->delta_row = 0;
    movement->delta_col = 0;
    if (command == 'w') {
        movement->delta_row = -1;
    } else if (command == 'a') {
        movement->delta_col = -1;
    } else if (command == 's') {
        movement->delta_row = 1;
    } else if (command == 'd') {
        movement->delta_col = 1;
    } else {
        return 0;
    }

    if (snake_id == SNAKE_ORIGINAL) {
        movement->active_snake = &state->snake;
        movement->active_stats = &state->original_stats;
    } else {
        movement->active_snake = &state->rival_snake;
        movement->active_stats = &state->rival_stats;
    }
    movement->opponent_snake = NULL;
    movement->opponent_stats = NULL;
    if (state->has_rival) {
        if (snake_id == SNAKE_ORIGINAL) {
            movement->opponent_snake = &state->rival_snake;
            movement->opponent_stats = &state->rival_stats;
        } else {
            movement->opponent_snake = &state->snake;
            movement->opponent_stats = &state->original_stats;
        }
    }

    movement->attempted_row =
        movement->active_snake->head_row + movement->delta_row;
    movement->attempted_col =
        movement->active_snake->head_col + movement->delta_col;
    movement->new_row = movement->attempted_row;
    movement->new_col = movement->attempted_col;
    movement->should_add_segment = 0;
    movement->consumed_reverse = 0;
    movement->consumed_split = 0;
    movement->lose_game = 0;
    movement->win_game = 0;

    if (!is_position_in_bounds(
            movement->attempted_row,
            movement->attempted_col
        )) {
        movement->lose_game = 1;
        movement->destination = EMPTY;
    } else {
        const struct tile *attempted_tile =
            &state->board[movement->attempted_row][movement->attempted_col];
        movement->destination = attempted_tile->entity;
    }

    return 1;
}

void apply_portal_if_needed(
    struct game_state *state,
    struct movement_context *movement
) {
    if (movement->lose_game || movement->destination != PORTAL) {
        return;
    }

    int partner_row = movement->attempted_row;
    int partner_col = movement->attempted_col;
    if (!find_portal_partner(
            &state->portals,
            partner_row,
            partner_col,
            &partner_row,
            &partner_col
        )) {
        movement->lose_game = 1;
        return;
    }

    movement->new_row = partner_row + movement->delta_row;
    movement->new_col = partner_col + movement->delta_col;
    if (!is_position_in_bounds(movement->new_row, movement->new_col)) {
        movement->lose_game = 1;
        return;
    }

    const struct tile *destination_tile =
        &state->board[movement->new_row][movement->new_col];
    movement->destination = destination_tile->entity;
}

void evaluate_destination(
    struct game_state *state,
    struct movement_context *movement
) {
    if (movement->lose_game) {
        return;
    }

    if (
        movement->destination == WALL ||
        movement->destination == BODY_SEGMENT
    ) {
        movement->lose_game = 1;
        return;
    }
    if (movement->destination == EXIT_LOCKED) {
        if (state->apples_remaining == 0) {
            state->board[movement->new_row][movement->new_col]
                .entity = EXIT_UNLOCKED;
            movement->win_game = 1;
        } else {
            movement->lose_game = 1;
        }
        return;
    }
    if (movement->destination == EXIT_UNLOCKED) {
        movement->win_game = 1;
        return;
    }
    if (movement->destination == EXPLOSION) {
        movement->lose_game = 1;
        return;
    }
    if (
        movement->destination == PASSAGE_UP ||
        movement->destination == PASSAGE_DOWN ||
        movement->destination == PASSAGE_LEFT ||
        movement->destination == PASSAGE_RIGHT
    ) {
        handle_passage_tile(state, movement);
        return;
    }

    handle_consumable_tile(state, movement);
}

void handle_passage_tile(
    struct game_state *state,
    struct movement_context *movement
) {
    int correct_direction = 0;
    if (
        movement->destination == PASSAGE_UP &&
        movement->delta_row == -1
    ) {
        correct_direction = 1;
    } else if (
        movement->destination == PASSAGE_DOWN &&
        movement->delta_row == 1
    ) {
        correct_direction = 1;
    } else if (
        movement->destination == PASSAGE_LEFT &&
        movement->delta_col == -1
    ) {
        correct_direction = 1;
    } else if (
        movement->destination == PASSAGE_RIGHT &&
        movement->delta_col == 1
    ) {
        correct_direction = 1;
    }

    if (!correct_direction) {
        movement->lose_game = 1;
        return;
    }

    movement->should_add_segment = 1;
    struct tile *passage_tile =
        &state->board[movement->new_row][movement->new_col];
    passage_tile->entity = BODY_SEGMENT;
}

void handle_consumable_tile(
    struct game_state *state,
    struct movement_context *movement
) {
    movement->should_add_segment = 1;

    struct tile *consumed_tile =
        &state->board[movement->new_row][movement->new_col];
    int explosion_radius = consumed_tile->value;

    if (
        movement->destination == APPLE_NORMAL ||
        movement->destination == APPLE_REVERSE ||
        movement->destination == APPLE_SPLIT ||
        movement->destination == APPLE_EXPLODE
    ) {
        if (state->apples_remaining > 0) {
            state->apples_remaining--;
        }
        state->apples_removed_total++;
    }

    if (movement->destination == APPLE_NORMAL) {
        movement->active_stats->apples_eaten++;
        movement->active_stats->points += 5;
        state->points_remaining_total -= 5;
    } else if (movement->destination == APPLE_REVERSE) {
        movement->active_stats->apples_eaten++;
        movement->active_stats->points += 10;
        state->points_remaining_total -= 10;
        movement->consumed_reverse = 1;
    } else if (movement->destination == APPLE_SPLIT) {
        movement->active_stats->apples_eaten++;
        movement->active_stats->points += 20;
        state->points_remaining_total -= 20;
        movement->consumed_split = 1;
    } else if (movement->destination == APPLE_EXPLODE) {
        movement->active_stats->apples_eaten++;
        movement->active_stats->points += 20;
        state->points_remaining_total -= 20;
        consumed_tile->value = 0;
        start_explosion(
            state,
            movement->new_row,
            movement->new_col,
            explosion_radius
        );
    }

    if (state->points_remaining_total < 0) {
        state->points_remaining_total = 0;
    }

    consumed_tile->entity = BODY_SEGMENT;
    consumed_tile->value = 0;
}

void finalize_movement(
    struct game_state *state,
    struct movement_context *movement
) {
    struct snake_body *snake = movement->active_snake;
    if (snake == NULL) {
        return;
    }

    if (movement->should_add_segment) {
        int length = snake->length;
        snake->rows[length] = movement->new_row;
        snake->cols[length] = movement->new_col;
        snake->length++;
        snake->head_row = movement->new_row;
        snake->head_col = movement->new_col;

        if (movement->consumed_reverse) {
            reverse_snake(snake->rows, snake->cols, snake->length);
            snake->head_row = snake->rows[snake->length - 1];
            snake->head_col = snake->cols[snake->length - 1];
        }

        if (movement->consumed_split) {
            int body_segments = snake->length - 1;
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
                    state->board,
                    snake->rows,
                    snake->cols,
                    &snake->length,
                    segments_to_remove
                );
            }
            snake->head_row = snake->rows[snake->length - 1];
            snake->head_col = snake->cols[snake->length - 1];
        }
    } else if (movement->win_game || movement->lose_game) {
        if (is_position_in_bounds(movement->new_row, movement->new_col)) {
            snake->head_row = movement->new_row;
            snake->head_col = movement->new_col;
        }
    }
}

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

void unlock_exits_if_needed(struct game_state *state) {
    if (state->apples_remaining != 0) {
        return;
    }

    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            if (state->board[row][col].entity == EXIT_LOCKED) {
                state->board[row][col].entity = EXIT_UNLOCKED;
            }
        }
    }
}

void print_current_statistics(const struct game_state *state) {
    double completion_percentage = 100.0;
    if (state->apples_initial != 0) {
        completion_percentage = 100.0 * state->apples_removed_total /
            (double)state->apples_initial;
    }

    if (state->has_rival) {
        print_game_statistics_with_rival(
            state->original_stats.points,
            state->original_stats.moves_made,
            state->original_stats.apples_eaten,
            state->rival_stats.points,
            state->rival_stats.moves_made,
            state->rival_stats.apples_eaten,
            state->apples_remaining,
            completion_percentage,
            state->points_remaining_total
        );
    } else {
        print_game_statistics(
            state->original_stats.points,
            state->original_stats.moves_made,
            state->original_stats.apples_eaten,
            state->apples_remaining,
            completion_percentage,
            state->points_remaining_total
        );
    }
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

int find_portal_partner(
    const struct portal_pairs *portals,
    int row,
    int col,
    int *partner_row,
    int *partner_col
) {
    for (int index = 0; index < portals->count; index++) {
        if (!portals->active[index]) {
            continue;
        }
        if (portals->rows[index][0] == row && portals->cols[index][0] == col) {
            *partner_row = portals->rows[index][1];
            *partner_col = portals->cols[index][1];
            return 1;
        }
        if (portals->rows[index][1] == row && portals->cols[index][1] == col) {
            *partner_row = portals->rows[index][0];
            *partner_col = portals->cols[index][0];
            return 1;
        }
    }

    return 0;
}

int is_movement_command(char command) {
    return command == 'w' || command == 'a' || command == 's' || command == 'd';
}

int remove_snake_segment(struct snake_body *snake, int row, int col) {
    for (int index = 0; index < snake->length; index++) {
        if (snake->rows[index] == row && snake->cols[index] == col) {
            int was_head = index == snake->length - 1;
            for (int shift = index; shift < snake->length - 1; shift++) {
                snake->rows[shift] = snake->rows[shift + 1];
                snake->cols[shift] = snake->cols[shift + 1];
            }
            snake->length--;
            if (snake->length > 0) {
                snake->head_row = snake->rows[snake->length - 1];
                snake->head_col = snake->cols[snake->length - 1];
            } else {
                snake->head_row = NO_SNAKE;
                snake->head_col = NO_SNAKE;
            }
            if (was_head) {
                return 1;
            }
            return 0;
        }
    }

    return -1;
}

void update_points_for_destroyed_apple(
    struct game_state *state,
    enum entity apple_entity
) {
    if (state->apples_remaining > 0) {
        state->apples_remaining--;
    }
    state->apples_removed_total++;

    int apple_points = 0;
    if (apple_entity == APPLE_NORMAL) {
        apple_points = 5;
    } else if (apple_entity == APPLE_REVERSE) {
        apple_points = 10;
    } else {
        apple_points = 20;
    }

    if (state->points_remaining_total > apple_points) {
        state->points_remaining_total -= apple_points;
    } else {
        state->points_remaining_total = 0;
    }
}

void deactivate_portal_pair(struct game_state *state, int row, int col) {
    for (int index = 0; index < state->portals.count; index++) {
        if (
            state->portals.rows[index][0] == row &&
            state->portals.cols[index][0] == col
        ) {
            state->portals.active[index] = 0;
            return;
        }
        if (
            state->portals.rows[index][1] == row &&
            state->portals.cols[index][1] == col
        ) {
            state->portals.active[index] = 0;
            return;
        }
    }
}

int destroy_entity_by_explosion(
    struct game_state *state,
    int row,
    int col
) {
    struct tile *tile = &state->board[row][col];
    enum entity entity = tile->entity;

    if (entity == EXIT_LOCKED || entity == EXIT_UNLOCKED) {
        return EXPLOSION_NONE;
    }
    int explosion_result = EXPLOSION_NONE;
    if (entity == PORTAL) {
        deactivate_portal_pair(state, row, col);
    } else if (entity == BODY_SEGMENT) {
        int removal = remove_snake_segment(&state->snake, row, col);
        if (removal == 1) {
            explosion_result |= EXPLOSION_ORIGINAL;
        }
        if (removal == -1 && state->has_rival) {
            removal = remove_snake_segment(&state->rival_snake, row, col);
            if (removal == 1) {
                explosion_result |= EXPLOSION_RIVAL;
            }
        }
    } else if (
        entity == APPLE_NORMAL ||
        entity == APPLE_REVERSE ||
        entity == APPLE_SPLIT ||
        entity == APPLE_EXPLODE
    ) {
        update_points_for_destroyed_apple(state, entity);
    }

    tile->entity = EXPLOSION;
    tile->value = 0;
    return explosion_result;
}

// Returns the magnitude of the supplied integer value.
int absolute_value(int number) {
    if (number < 0) {
        return -number;
    }

    return number;
}

void clear_explosion_ring(
    struct game_state *state,
    int center_row,
    int center_col,
    int distance
) {
    if (distance <= 0) {
        return;
    }

    for (int delta_row = -distance; delta_row <= distance; delta_row++) {
        int row = center_row + delta_row;
        int remaining = distance - absolute_value(delta_row);
        int cols_to_check[2];
        cols_to_check[0] = center_col - remaining;
        cols_to_check[1] = center_col + remaining;
        int positions = 2;
        if (remaining == 0) {
            positions = 1;
        }
        for (int index = 0; index < positions; index++) {
            int col = cols_to_check[index];
            if (!is_position_in_bounds(row, col)) {
                continue;
            }
            struct tile *tile = &state->board[row][col];
            if (tile->entity == EXPLOSION) {
                tile->entity = EMPTY;
                tile->value = 0;
            }
        }
    }
}

int apply_explosion_ring(
    struct game_state *state,
    int center_row,
    int center_col,
    int distance
) {
    if (distance <= 0) {
        return 0;
    }

    int explosion_status = EXPLOSION_NONE;
    for (int delta_row = -distance; delta_row <= distance; delta_row++) {
        int row = center_row + delta_row;
        int remaining = distance - absolute_value(delta_row);
        int cols_to_check[2];
        cols_to_check[0] = center_col - remaining;
        cols_to_check[1] = center_col + remaining;
        int positions = 2;
        if (remaining == 0) {
            positions = 1;
        }
        for (int index = 0; index < positions; index++) {
            int col = cols_to_check[index];
            if (!is_position_in_bounds(row, col)) {
                continue;
            }
            enum entity entity = state->board[row][col].entity;
            if (entity == EXIT_LOCKED || entity == EXIT_UNLOCKED) {
                continue;
            }
            explosion_status |= destroy_entity_by_explosion(state, row, col);
        }
    }

    if (state->apples_remaining == 0) {
        unlock_exits_if_needed(state);
    }
    return explosion_status;
}

void start_explosion(struct game_state *state, int row, int col, int radius) {
    if (radius <= 0) {
        return;
    }

    for (int index = 0; index < MAX_EXPLOSIONS; index++) {
        if (state->explosions[index].active) {
            continue;
        }
        state->explosions[index].active = 1;
        state->explosions[index].row = row;
        state->explosions[index].col = col;
        state->explosions[index].radius = radius;
        state->explosions[index].current_step = 1;
        state->explosion_count++;
        apply_explosion_ring(state, row, col, 1);
        return;
    }
}

int advance_explosions(struct game_state *state) {
    if (state->explosion_count == 0) {
        return 0;
    }

    for (int index = 0; index < MAX_EXPLOSIONS; index++) {
        if (!state->explosions[index].active) {
            continue;
        }
        clear_explosion_ring(
            state,
            state->explosions[index].row,
            state->explosions[index].col,
            state->explosions[index].current_step
        );
    }

    int explosion_status = EXPLOSION_NONE;
    int active_total = 0;
    for (int index = 0; index < MAX_EXPLOSIONS; index++) {
        if (!state->explosions[index].active) {
            continue;
        }
        struct explosion *boom = &state->explosions[index];
        if (boom->current_step >= boom->radius) {
            boom->active = 0;
            continue;
        }
        boom->current_step++;
        explosion_status |= apply_explosion_ring(
            state,
            boom->row,
            boom->col,
            boom->current_step
        );
        active_total++;
    }

    state->explosion_count = active_total;
    return explosion_status;
}


// =============================================================================
// EDITABLE Provided Functions
// =============================================================================

// Given a 2D board array, initialise all tiles to EMPTY.
void initialise_board(struct tile board[ROWS][COLS]) {
    for (int row = 0; row < ROWS; row++) {
        for (int col = 0; col < COLS; col++) {
            board[row][col].entity = EMPTY;
            board[row][col].value = 0;
        }
    }
}

// Prints the game board, showing the snake's head position on the board.
void print_board(const struct game_state *state) {
    print_board_line();
    print_board_header();
    print_board_line();
    for (int row = 0; row < ROWS; row++) {
        print_tile_spacer();
        for (int col = 0; col < COLS; col++) {
            printf(" ");
            struct tile tile = state->board[row][col];
            int is_original_head =
                state->snake.length > 0 &&
                row == state->snake.head_row &&
                col == state->snake.head_col;
            int is_rival_head =
                state->has_rival &&
                state->rival_snake.length > 0 &&
                row == state->rival_snake.head_row &&
                col == state->rival_snake.head_col;
            if (is_original_head) {
                printf("^~^");
            } else if (is_rival_head) {
                printf("^v^");
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
            } else if (tile.entity == APPLE_EXPLODE) {
                printf("(%d)", tile.value);
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

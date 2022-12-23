#include "../include/libs/ST7789_t3.h"
#include "../include/keypad.h"

//TODO
//chording
//main menu
//better scaling
//num of mines
//proper flag
//mines left display
//time
//fix annoying "clever" code

void print(const char *n);

#define SCREEN_WIDTH 240
#define SCREEN_HEIGHT 240

#define MINE 0x0F

typedef struct {
    uint8_t *board;
    int selected_x, selected_y;
    int width, height, cell_width, cell_height;
    int num_mines;
} GameState;

const uint16_t neighbour_colours[] = {
    0b0000000000000000,
    0b0000000000011111,
    0b0000011000000000,
    0b1111100000000000,
    0b0000100000011000,
    0b1000000000000000,
    0b0000010000010000,
    0b0000000000000000,
    0b1000010000010000,
};

int count_neighbours(GameState *state, int x, int y) {
    int neighbours = 0;
    for (int dx = -1; dx < 2; dx++) {
        for (int dy = -1; dy < 2; dy++) {
            int bx = x + dx;
            int by = y + dy;
            if (bx < 0 || bx >= state->width ||
                by < 0 || by >= state->height) {
                continue;
            }
            if (state->board[by * state->width + bx] == MINE) {
                neighbours += 1;
            }
        }
    }
    return neighbours;
}

void generate_board(GameState *state) {
    for (int i = 0; i < state->num_mines; i++) {
        int pos;
        //make sure the chosen pos doesn't already have a mine
        do {
            pos = rand() % (state->width * state->height);
        } while (state->board[pos] == MINE);
        state->board[pos] = MINE;
    }

    //number non-mine squares
    for (int y = 0; y < state->height; y++) {
        for (int x = 0; x < state->width; x++) {
            if (state->board[y * state->width + x] == MINE) {
                continue;
            }
            state->board[y * state->width + x] = count_neighbours(state, x, y);
        }
    }
}

//return whether the game should continue
bool reveal(GameState *state, int x, int y) {
    if (state->board[y * state->width + x] & 0x80) {
        //already revealed
        return true;
    }
    if (state->board[y * state->width + x] & 0x40) {
        //flagged, don't reveal
        return true;
    }
    if (state->board[y * state->width + x] == MINE) {
        //explode
        return false;
    }
    
    state->board[y * state->width + x] |= 0x80;
    if (state->board[y * state->width + x] == 0x80) {
        //0 neighbours, recursively propagate
        for (int dx = -1; dx < 2; dx++) {
            for (int dy = -1; dy < 2; dy++) {
                if (x + dx < 0 || x + dx >= state->width ||
                    y + dy < 0 || y + dy >= state->height) {
                    continue;
                }
                if (state->board[(y + dy) * state->width + x + dx] & 0x80) {
                    continue;
                }
                
                reveal(state, x + dx, y + dy);
            }
        }
    }
    
    return true;
}

void draw_centred_char(ST7789_t3 *tft, char c, uint16_t col, uint16_t bg_col, int x, int y, int w, int h, GameState *state) {
    const char str[2] = {c, 0};
    uint16_t charw, charh;
    int16_t _x, _y; //dummy, not used
    tft->getTextBounds((const char*)str, 0, 0, &_x, &_y, &charw, &charh);
    int text_size = min(state->cell_width / charw, state->cell_height / charh);
    charw *= text_size;
    charh *= text_size;
    tft->drawChar(x + w / 2 - charw / 2 + text_size/2, y + h / 2 - charh / 2 + text_size/2, c, col, bg_col, text_size, text_size);
}

void draw_tile(ST7789_t3 *tft, int x, int y, GameState *state) {
    uint8_t tile = state->board[y * state->width + x];
    x = x * state->cell_width + 1;
    y = y * state->cell_height + 1;
    int w = state->cell_width - 2;
    int h = state->cell_height - 2;
    
    uint16_t revealed_col = 0b1100011000011000;
    uint16_t unrevealed_col = 0b0111001110001110;

    if (tile & 0x80) {
        //revealed
        uint8_t neighbours = tile & 0x0F;
        uint16_t col = neighbour_colours[neighbours];

        tft->fillRect(x, y, w, h, revealed_col);
        if (neighbours > 0) {
            draw_centred_char(tft, '0' + neighbours, col, revealed_col, x, y, w, h, state);
        }
    } else if (tile & 0x40) {
        //flagged
        tft->fillRect(x, y, w, h, unrevealed_col);
        draw_centred_char(tft, '!', 0b1111100000000000, unrevealed_col, x, y, w, h, state);
    } else {
        tft->fillRect(x, y, w, h, unrevealed_col);
    }
}

void draw(ST7789_t3 *tft, GameState *state) {
    for (int y = 0; y < state->height; y++) {
        for (int x = 0; x < state->width; x++) {
            draw_tile(tft, x, y, state);
            uint16_t border = state->selected_x == x && state->selected_y == y ? 0b1111111111111111 : 0b1000010000010000;
            tft->drawRect(x * state->cell_width, y * state->cell_height, state->cell_width, state->cell_height, border);
        }
    }
    if (!tft->asyncUpdateActive()) {
        tft->updateScreenAsync();
    }
}

bool check_win(GameState *state) {
    int unrevealed = 0;
    for (int y = 0; y < state->height; y++) {
        for (int x = 0; x < state->width; x++) {
            //if the top bit (i.e. revealed) isn't set
            if (!((state->board[y * state->width + x] & 0x80) >> 7)) {
                unrevealed += 1;
            }
        }
    }
    return unrevealed == state->num_mines;
}

void minesweeper(Keypad *pad, ST7789_t3 *tft) {
    //semi random ish, read from uninitialised analog pin
    srand(analogRead(A7) ^ micros());
    
    const int width = 5;
    const int height = 5;
    //board: mine = 0x0F, non-mine = n where n is the number of neighbours
    //top bit set if revealed (|= 0x80)
    //2nd-top bit set if flagged (|= 0x40)
    uint8_t board[width * height] = {0};

    GameState state = {
        board,
        0, 0,
        width, height, SCREEN_WIDTH / width, SCREEN_HEIGHT / height,
        width * height / 6
    };
    
    generate_board(&state);
    while ((pad->buttons[0] & 0b100) == 0) {
        read_keypad(pad);
        state.selected_x += ((pad->buttons[6] & 0b100) >> 2) && state.selected_x < state.width - 1;
        state.selected_y += ((pad->buttons[9] & 0b100) >> 2) && state.selected_y < state.height - 1;
        state.selected_x -= ((pad->buttons[4] & 0b100) >> 2) && state.selected_x > 0;
        state.selected_y -= ((pad->buttons[1] & 0b100) >> 2) && state.selected_y > 0;
        if (pad->buttons[13] & 0b100) {
            if (!reveal(&state, state.selected_x, state.selected_y)) {
                print("Game over\n");
                return;
            } else if (check_win(&state)) {
                print("Success\n");
                return;
            }
        }
        if (pad->buttons[12] & 0b100) {
            //toggle flagging, if unrevealed
            int idx = state.selected_y * state.width + state.selected_x;
            if (!(state.board[idx] & 0x80)) {
                state.board[idx] ^= 0x40;
            }
        }
        draw(tft, &state);
    }
}
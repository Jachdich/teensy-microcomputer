#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <main.h>
#include <keypad.h>
#include <stdint.h>
#include <string.h>
#include "../include/libs/ST7789_t3.h"
#include "minesweeper.h"

void drawMainMenu() {

}

#define TFT_MOSI  11
#define TFT_SCK   13
#define TFT_DC    37
#define TFT_CS    10
#define TFT_RST   8
#define TFT_BL    3
ST7789_t3 tft = ST7789_t3(TFT_CS, TFT_DC, TFT_RST);

// asm(".global pogger\npogger: .incbin \"limine.sys\"\n");
// extern unsigned char pogger[];

#define K_UP 0xF0
#define K_RIGHT 0xF1
#define K_DOWN 0xF2
#define K_LEFT 0xF3
#define K_CONTROL 0xF4
#define K_SHIFT 0xF5
#define K_SUPER 0xF6
#define K_ALT 0xF7
#define K_CAPS 0xF8
#define K_HOME 0xF9
#define K_END 0xFA
#define K_PAGEUP 0xFB
#define K_PAGEDOWN 0xF9

uint8_t keyboard[] = {
    ' ',  '\n', '\b', 'r',
    'a',  'd',  'e',  's',
    't',  'i',  'o',  'n',

    'w',  'c',  'l',  'h',
    'm',  'u',  'f',  'g',
    'p',  'y',  'b',  'v',

    '(',  ')',  '[',  ']',
    '{',  '}',  '=',  ';',
    '.',  ',',  '"',  ':',

    0x1b,   K_UP,   K_CONTROL, K_SHIFT,
    K_LEFT, ' ',    K_RIGHT,   K_CAPS,
    '\t',   K_DOWN, K_HOME,    K_END,

    'k',  'b',  'j',  'q',
    'x',  'z',  ' ',  '\'',
    '#',  '@',  '\\',  '~',

    '0',  '1',  '2',  '3',
    '4',  '5',  '6',  '7',
    '8',  '9',  '_',  '!',

    '<',  '>',  '+',  '-',
    '^',  '?',  '/',  '*',
    '%',  '|',  '$',  '&',

    ' ',     ' ',   K_PAGEUP, K_PAGEDOWN,
    ' ',     ' ',   ' ',      ' ',
    K_SUPER, K_ALT, '`',      ' '
};

typedef struct {
    int x, y;
} Vec2;

int caps = 0;
int shift = 0;
int ctrl = 0;
uint8_t keyboard_mod = 0;
const uint8_t maxScreenRows = 23;
#define CMD_STRING_LEN 100
char cmd_string[CMD_STRING_LEN];
uint16_t currScreenPos = 0;
#define CHARS_PER_LINE 40
char screenBuffer[CHARS_PER_LINE * maxScreenRows];
Keypad pad;
Vec2 last_cursor_pos = {1, 0};

void scrollDown() {
    for (uint8_t i = 0; i < maxScreenRows; i++) {
        memcpy(screenBuffer + i * CHARS_PER_LINE, screenBuffer + (i + 1) * CHARS_PER_LINE, CHARS_PER_LINE);
    }
    memset(screenBuffer + (maxScreenRows - 1) * CHARS_PER_LINE, 0, CHARS_PER_LINE);
    currScreenPos -= CHARS_PER_LINE;
    tft.fillScreen(ST7735_BLACK);
}

void print(const char n) {
    if (n == '\n') {
        currScreenPos += CHARS_PER_LINE - currScreenPos % CHARS_PER_LINE;
    } else {
        screenBuffer[currScreenPos] = n;
        currScreenPos += 1;
    }
    if (currScreenPos / CHARS_PER_LINE >= maxScreenRows) {
        scrollDown();
    }
}

void print(const char * n) {
    uint16_t len = strlen(n);
    uint16_t copiedLen = 0;
    for (uint16_t i = 0; i < len; i++) {
        if (n[i] == '\n') {
            copiedLen += CHARS_PER_LINE - (currScreenPos + copiedLen) % CHARS_PER_LINE;
        } else {
            *(screenBuffer + currScreenPos + copiedLen) = *(n + i);
            copiedLen++;
        }
    }
    //strcpy(screenBuffer + currScreenPos, n);
    currScreenPos += copiedLen;
    while (currScreenPos / CHARS_PER_LINE >= maxScreenRows) {
        scrollDown();
    }
}

void draw() {
    // tft.fillScreen(ST7735_BLACK);

    bool show_cursor = millis() / 500 % 2;
    Vec2 cursor_pos = (Vec2){(currScreenPos % CHARS_PER_LINE) * 6, (currScreenPos / CHARS_PER_LINE) * 10};
    tft.drawChar(last_cursor_pos.x, last_cursor_pos.y, ' ', ST77XX_WHITE, ST77XX_BLACK, 1, 1);
    
    for (uint8_t line = 0; line < maxScreenRows; line++) {
        for (int row = 0; row < CHARS_PER_LINE; row++) {
            char c = screenBuffer[row + line * CHARS_PER_LINE];
            if (c == 0 || c == ' ') {
                continue;
            }
            tft.drawChar(row * 6, 10 * line, c, ST77XX_WHITE, ST77XX_BLACK, 1, 1);
        }
    }
    if (show_cursor) {
        tft.drawChar(cursor_pos.x, cursor_pos.y, '|', ST77XX_WHITE, ST77XX_BLACK, 1, 1);
        last_cursor_pos = cursor_pos;
    }
}

void prompt() {
    print('>');
}

int strcnt(char *str, char c) {
    int cnt = 0;
    while (*str != 0) {
        if (*str == c) cnt++;
        str++;
    }
    return cnt;
}

void do_command(char * n) {
    print('\n');
    int argc = strcnt(n, ' ') + 1;
    char *argv[argc];
    argv[0] = n;
    int argpos = 1;
    while (*n != 0) {
        if (*n == ' ') {
            *n = 0;
            argv[argpos++] = n + 1;
        }
        n++;
    }
    if (strcmp(argv[0], "st") == 0) {
        // spacetrashLoop(&pad);
    } else if (strcmp(argv[0], "bat") == 0) {
        print("Battery voltage: ");
        int v = analogRead(A9);
        float computed_voltage = ((float)v / 1024.0) * 5.202765957446808;
        int truncated_voltage = computed_voltage * 100;
        char str[6];
        str[0] = '0'+ (truncated_voltage / 100) % 10;
        str[1] = '.';
        str[2] = '0' + (truncated_voltage / 10) % 10;
        str[3] = '0' + truncated_voltage % 10;
        str[4] = '\n';
        str[5] = 0;
        print(str);
    } else if (strcmp(argv[0], "brightness") == 0) {
        if (argc != 2) {
            print("Usage: brightness <value>\n");
        } else {
            int b = atoi(argv[1]);
            if (b < 1 || b > 255) {
                print("Error: invalid brightness value. Must be int, 0 < brightness < 256\n");
            } else {
                analogWrite(TFT_BL, b);
            }
        }
    } else if (strcmp(argv[0], "kb") == 0) {
        keypad_debug(&pad);
    } else if (strcmp(argv[0], "ms") == 0) {
        minesweeper(&pad, &tft);
    } else if (strlen(argv[0]) == 0) {

    } else {
        print("Command not found\n");
    }
    prompt();
}

void inChar(char n) {
    uint16_t len = strlen(cmd_string);
    if (n == '\b') {
        if (len > 0) {
            cmd_string[len - 1] = 0;
            screenBuffer[--currScreenPos] = 0;
        }
    } else if (n == '\n') {
        do_command(cmd_string);
        cmd_string[0] = 0;
    } else {
        if (len < CMD_STRING_LEN - 1) {
            cmd_string[len] = n;
            cmd_string[len + 1] = 0;
            print(n);
        }
    }
}

void read_keyboard(Keypad *pad) {
    keyboard_mod = 0;//(keypad[0][2] & 0b1) | (keypad[2][2] & 0b1) << 1;
    int which_key = -1;
    for (int i = 0; i < 12; i++) {
        if (pad->buttons[i] & 0b100) {
            which_key = i;
            break;
        }
    }
    if (which_key == -1) {
        return;
    }
    
    int left_mod =  pad->buttons[13] & 0b1;
    int right_mod = pad->buttons[12] & 0b1;

    int keyboard_idx = which_key + ((left_mod << 1) | right_mod) * 12 + 48 * ((pad->buttons[which_key] & 0b1000) >> 3);
    uint8_t in = keyboard[keyboard_idx];
    
    //mod key
    if (in > 0xF0) {
        if (in == K_CONTROL) {
            ctrl = 1;
        } else if (in == K_SHIFT) {
            shift = 1;
        } else if (in == K_CAPS) {
            caps = !caps;
        }
    }
    inChar(in);
}

int main() {
    memset(&pad, 0, sizeof(pad));
    cmd_string[0] = 0;
    pinMode(TFT_BL, OUTPUT);
    analogWrite(TFT_BL, 0);
    tft.init(240, 240);
    setup_keypad();
    prompt();

    tft.setRotation(2);
    tft.setTextWrap(false);
    tft.setTextSize(1);
    tft.setTextColor(ST7735_WHITE);
    tft.useFrameBuffer(true);

    while (true) {

        read_keypad(&pad);

        read_keyboard(&pad);
        draw();
        // minesweeper(&pad, &tft);
        if (!tft.asyncUpdateActive()) {
            tft.updateScreenAsync();
        }
        // tft.waitUpdateAsyncComplete();
/*
        currScreenPos = 0;
        for (int j = 0; j < 14; j++) {
            bool newlvl = pad.buttons[j] & 0x1;
            print(newlvl + '0');
            print(' ');
            if (j % 4 == 0) {
                print('\n');
            }
        }
        draw();*/
    }
}

// #include <Arduino.h>
// void setup();
// void loop();
// int main() {
//     setup();
//     while (1) {
//         loop();
//     }
// }

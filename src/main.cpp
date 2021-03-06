 /* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <main.h>
#include <keypad.h>
#include <spacetrash.h>

U8G2_SH1106_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

void drawMainMenu() {

}

char keyboard[][10] = {
    {'q','w','e','r','t','y','u','i','o','p'},
    {'a','s','d','f','g','h','j','k','l',' '},
    {' ','z','x','c','v','b','n','m','.',','},

    {'Q','W','E','R','T','Y','U','I','O','P'},
    {'A','S','D','F','G','H','J','K','L',' '},
    {'\'','Z','X','C','V','B','N','M','<','>'},

    {'1','2','3','4','5','6','7','8','9','0'},
    {'(',')','+','-','*','/','_',';','!',' '},
    {'<','>','?','|','&','^','%','~','#',':'},

    {'{','}','[',']',' ','$','\\','@','`',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
};

char keyboardString[][21] = {
    "q w e r t y u i o p",
    " a s d f g h j k l ",
    "\" z x c v b n m . ,",

    "Q W E R T Y U I O P",
    " A S D F G H J K L ",
    "' Z X C V B N M < >",

    "1 2 3 4 5 6 7 8 9 0",
    " ( ) + - * / _ ; ! ",
    "< > ? | & ^ % ~ # :",

    "{ } [ ]   $ \\ @ `  ",
    "                   ",
    "                   ",
};

uint8_t keyboardX = 0;
uint8_t keyboardY = 0;
uint8_t keyboardMod = 0;
uint8_t maxScreenRows = 4;
#define CMD_STRING_LEN 100
char cmdString[CMD_STRING_LEN];
uint16_t currScreenPos = 0;
#define KB_OFFSET_X 16
#define KB_OFFSET_Y 48
#define DELETE 255
#define CHARS_PER_LINE 26
char screenBuffer[CHARS_PER_LINE * 4];
void drawKeyboard() {
    for (int row = 0; row < 3; row++) {
        u8g2.drawStr(KB_OFFSET_X, KB_OFFSET_Y + row * u8g2.getMaxCharHeight(), keyboardString[row + keyboardMod * 3]);
    }
    u8g2.drawFrame((keyboardX * u8g2.getMaxCharWidth()) * 2 - 1 + KB_OFFSET_X + u8g2.getMaxCharWidth() * (keyboardY % 2),
                    keyboardY * u8g2.getMaxCharHeight() - u8g2.getMaxCharHeight() + KB_OFFSET_Y,
                    u8g2.getMaxCharWidth() + 1, u8g2.getMaxCharHeight() + 1);
}

void scrollDown() {
    for (uint8_t i = 0; i < maxScreenRows; i++) {
        memcpy(screenBuffer + i * CHARS_PER_LINE, screenBuffer + (i + 1) * CHARS_PER_LINE, CHARS_PER_LINE);
    }
    currScreenPos -= CHARS_PER_LINE;
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
        if (currScreenPos / CHARS_PER_LINE >= maxScreenRows) {
        scrollDown();
    }
}

void draw() {
    u8g2.setFontDirection(0);
    u8g2.setFontRefHeightAll();
    u8g2.setFont(u8g2_font_5x7_tf);
    u8g2.setDrawColor(1);
    u8g2.firstPage();
    do {
        drawKeyboard();
        for (uint8_t line = 0; line < maxScreenRows; line++) {
            char buf[CHARS_PER_LINE + 1];
            buf[CHARS_PER_LINE] = 0;
            memcpy(buf, screenBuffer + line * CHARS_PER_LINE, CHARS_PER_LINE);
            u8g2.drawStr(0, 10 * (line + 1), buf); 
        }
    } while(u8g2.nextPage());
}

void inChar(char n) {
    uint16_t len = strlen(cmdString);
    if (n == DELETE) {
        if (len > 0) {
            cmdString[len - 1] = 0;
            screenBuffer[--currScreenPos] = 0;
        }
    } else {
        if (len < CMD_STRING_LEN - 1) {
            cmdString[len] = n;
            cmdString[len + 1] = 0;
            print(n);
        }
    }
}

void prompt() {
    print('>');
}

void doCommand(char * n) {
    print('\n');
    if (strcmp(n, "st") == 0) {
        spacetrashLoop();
    } else if (strlen(n) == 0) {

    } else {
        print("Command not found\n");
    }
    prompt();
}

void readOnscreenKeyboard() {
    keyboardMod = (keypad[2][0] & 0b1) | (keypad[2][2] & 0b1) << 1;
    if (keypad[0][1] == 0b011) {
        keyboardY--;
    }
    if (keypad[2][1] == 0b011) {
        keyboardY++;
    }

    if (keypad[1][0] == 0b011) {
        keyboardX--;
    }

    if (keypad[1][2] == 0b011) {
        keyboardX++;
    }
    
    if (keypad[1][1] == 0b011) {
        inChar(keyboard[keyboardY + keyboardMod * 3][keyboardX]);
    }
    if (keypad[0][2] == 0b011) {
        inChar(DELETE);
    }
    if (keypad[0][0] == 0b011) {
        inChar(' ');
    }

    if (keypad[3][0] == 0b011) {
        doCommand(cmdString);
        cmdString[0] = 0;
    }
}

uint8_t func_data[] = {
    0x04, 0xb0, 0x2d, 0xe5,
    0x00, 0xb0, 0x8d, 0xe2,
    0x45, 0x30, 0xa0, 0xe3,
    0x03, 0x00, 0xa0, 0xe1,
    0x00, 0xd0, 0x4b, 0xe2,
    0x04, 0xb0, 0x9d, 0xe4,
    0x1e, 0xff, 0x2f, 0xe1,
    0x47, 0x43, 0x43, 0x3a,
};


uint8_t entry() {
    return 69;
}

const uint32_t base = 15624;

int main() {
    Serial.begin(9600);
    while (!Serial);

    Serial.println("I'm alive lol");
    uint8_t *func_ram_data = (uint8_t*)(base + 0x04);
    Serial.printf("%lu %lu\r\n", func_ram_data, &entry);
    Serial.flush();
    for (uint8_t i = 0; i < 32; i++) {
        //Serial.printf("%02x ", func_ram_data[i]); Serial.flush();
        func_ram_data[i] = ((uint8_t*)(&entry))[i];
        //Serial.printf("%02x %02x", func_ram_data[i], ((uint8_t*)(&entry))[i]); Serial.flush();
        //Serial.println();
        //Serial.flush();
        //delay(1);
    }
    
    int (*func_in_ram)(void) = (int(*)(void))((uint32_t)func_ram_data | 0x1);
    int ret = func_in_ram();
    Serial.println(ret);
    Serial.flush();
    Serial.println("done");
    
    while (true);
/*
    cmdString[0] = 0;
	u8g2.begin(); 
	setupKeypad();
	prompt();

    while (true) {
        readKeypad();
        readOnscreenKeyboard();
        draw();
    }*/
}


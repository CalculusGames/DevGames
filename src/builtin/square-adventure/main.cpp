#include <iostream>
#include <cmdfx.h>

CmdFX_Sprite* square = 0;

int running = 1;

int onKeyPress(CmdFX_Event* event) {
    CmdFX_KeyEvent* payload = (CmdFX_KeyEvent*) event->data;
    if (square == 0) return 0;

    char c = payload->keyChar;
    switch (c) {
        // Quit
        case 'q':
            Canvas_showCursor();
            Screen_setEchoEnabled(1);
            Screen_setLineBuffered(1);
            running = 0;
            break;
        // Movement
        case 'w':
            Sprite_moveBy(square, 0, -1);
            break;
        case 's':
            Sprite_moveBy(square, 0, 1);
            break;
        case 'a':
            Sprite_moveBy(square, -1, 0);
            break;
        case 'd':
            Sprite_moveBy(square, 1, 0);
            break;
        // Change Color
        case '0':
            Sprite_setForegroundAll(square, 0xFFFFFF);
            break;
        case '1':
            Sprite_setForegroundAll(square, 0xFF0000);
            break;
        case '2':
            Sprite_setForegroundAll(square, 0x00FF00);
            break;
        case '3':
            Sprite_setForegroundAll(square, 0x0000FF);
            break;
        case '4':
            Sprite_setForegroundAll(square, 0xDB09F7);
            break;
        case '5':
            Sprite_setForegroundAll(square, 0x09EBF7);
            break;
        case '6':
            Sprite_setForegroundAll(square, 0xFFD700);
            break;
        case '7':
            Sprite_setForegroundAll(square, 0xFFA700);
            break;
        case '8':
            Sprite_setForegroundAll(square, 0x9C09F7);
            break;
        case '9':
            Sprite_setForegroundAll(square, 0x222222);
            break;
    }

    return 0;
}

int main() {
    Canvas_clearScreen();
    Canvas_hideCursor();

    Screen_setEchoEnabled(0);
    Screen_setLineBuffered(0);

    int width = Canvas_getWidth();
    int height = Canvas_getHeight();

    square = Sprite_create(
        Char2DBuilder_createFilled(3, 3, '#'),
        String2DBuilder_create(3, 3),
        0
    );
    Sprite_draw(width / 2, height / 2, square);
    addCmdFXEventListener(CMDFX_EVENT_KEY, onKeyPress);

    Canvas_setCursor(0, 1);
    std::cout << "Use WASD to move the square and the number keys to change its color." << std::endl << "Press 'Q' to quit." << std::endl;

    while (running) {} // hang

    shutdownCmdFXEvents();
    return 0;
}
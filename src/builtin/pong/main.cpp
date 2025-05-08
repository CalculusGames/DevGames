#include <iostream>
#include <cstring>

#include <cmdfx.h>

static bool _twoPlayer = false;
static bool _running = true;

static CmdFX_Sprite* left = nullptr;
static CmdFX_Sprite* right = nullptr;
static CmdFX_Sprite* ball = nullptr;

int keypress(CmdFX_Event* event) {
    CmdFX_KeyEvent* keyEvent = static_cast<CmdFX_KeyEvent*>(event->data);
    char c = keyEvent->keyChar;
    int code = keyEvent->keyCode;

    // quit
    if (c == 'q' || code == KEY_ESC) {
        _running = false;
        return 0;
    }

    // movement
    if (c == 'w' || code == KEY_UP_ARROW) {
        if (left->y > 0)
            Sprite_moveBy(left, 0, -1);
    }

    if (c == 's' || code == KEY_DOWN_ARROW) {
        if (left->y < Canvas_getHeight())
            Sprite_moveBy(left, 0, 1);
    }

    if (_twoPlayer) {
        if (c == 'i' || code == KEY_UP_ARROW) {
            if (right->y > 0)
                Sprite_moveBy(right, 0, -1);
        }

        if (c == 'k' || code == KEY_DOWN_ARROW) {
            if (right->y < Canvas_getHeight())
                Sprite_moveBy(right, 0, 1);
        }
    }

    return 0;
}

void secondPaddleAI(void* arg) {
    while (_running) {
        int y = right->y;
        int ballY = ball->y;

        if (ballY > y) {
            if (y < Canvas_getHeight())
                Sprite_moveBy(right, 0, 1);
        } else if (ballY < y) {
            if (y > 0)
                Sprite_moveBy(right, 0, -1);
        }

        sleepMillis(50);
    }
}

void ballFunctions(void* arg) {
    int x = ball->x;
    int y = ball->y;

    while (_running) {
        // move ball
        Sprite_moveBy(ball, 1, 0);

        // bounce off walls
        if (y <= 0 || y >= Canvas_getHeight()) {
            Sprite_moveBy(ball, 0, -2);
        }

        // bounce off paddles
        if (Sprite_isColliding(ball, left)) {
            Sprite_moveBy(ball, -2, 0);
        } else if (Sprite_isColliding(ball, right)) {
            Sprite_moveBy(ball, -2, 0);
        }

        sleepMillis(50);
    }
}

void initSprites() {
    char** paddle = Char2DBuilder_createFilled(1, 8, '#');
    left = Sprite_create(paddle, 0, 2);
    right = Sprite_create(paddle, 0, 2);

    char** ballSprite = Char2DBuilder_createFilled(4, 4, '%');
    // remove corners
    ballSprite[0][0] = ' ';
    ballSprite[0][3] = ' ';
    ballSprite[3][0] = ' ';
    ballSprite[3][3] = ' ';

    ball = Sprite_create(ballSprite, 0, 1);
}

int main(int argc, char** argv) {
    Screen_ensureInTerminal();
    Canvas_clearScreen();
    Window_setTitle("Pong");
    Canvas_hideCursor();

    CmdFX_initThreadSafe();
    initSprites();

    int py = Canvas_getHeight() / 2;
    int lx = Canvas_getWidth() / 4;
    int rx = (Canvas_getWidth() * 3) / 4;

    Sprite_draw(lx, py, left);
    Sprite_draw(rx, py, right);
    Sprite_draw(lx, py, ball);

    if (argc > 1 && strcmp(argv[1], "2p") == 0) {
        _twoPlayer = true;
    } else {
        unsigned long ai = CmdFX_launchThread(secondPaddleAI, nullptr);
        CmdFX_detachThread(ai);
    }

    sleepMillis(3000);

    // start game
    addCmdFXEventListener(CMDFX_EVENT_KEY, keypress);
    
    unsigned long ball = CmdFX_launchThread(ballFunctions, nullptr);
    CmdFX_detachThread(ball);

    while (_running) {} // hang application

    // cleanup
    CmdFX_destroyThreadSafe();
    endCmdFXEventLoop();
    shutdownCmdFXEvents();

    return 0;
}
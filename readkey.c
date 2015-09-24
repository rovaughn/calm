#include "readkey.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <termios.h>
#include <string.h>

static struct termios saved_termios;

void init_readkey(void) {
    struct termios work;

    tcgetattr(STDIN_FILENO, &saved_termios);
    memcpy(&work, &saved_termios, sizeof work);

    work.c_lflag     &= (~ECHO & ~ICANON);
    work.c_cc[VMIN]   = 0;
    work.c_cc[VTIME]  = 0;
    tcsetattr(STDIN_FILENO, TCSANOW, &work);
}

void cleanup_readkey(void) {
    tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
}

key_t readkey(void) {
    static key_t queued = 0;
    static enum {
        START, ESC1, ESC2
    } state;

    char c;

    if (queued == 0) {
        ssize_t nread = read(STDIN_FILENO, &c, sizeof c);

        if (nread < 0) {
            switch (errno) {
            case EAGAIN: return KEY_AGAIN;
            default:
                perror("read stdin");
                exit(1);
            }
        } else if (nread != 1) {
            return KEY_BLOCK;
        }
    } else {
        c = queued;
        queued = 0;
    }

    switch (state) {
    case START:
        if (c == '\x1b') {
            state = ESC1;
            return KEY_AGAIN;
        } else {
            return c;
        }
    case ESC1:
        if (c == '[') {
            state = ESC2;
            return KEY_AGAIN;
        } else {
            state = START;
            queued = c;
            return '\x1b';
        }
    case ESC2:
        state = START;
        switch (c) {
        case '3': return KEY_DEL;   break;
        case '4': return KEY_END;   break;
        case 'A': return KEY_UP;    break;
        case 'B': return KEY_DOWN;  break;
        case 'C': return KEY_RIGHT; break;
        case 'D': return KEY_LEFT;  break;
        case 'H': return KEY_HOME;  break;
        default:
            fprintf(stderr, "Unknown escape sequence '%c' (%02x)\n", c, c);
            exit(1);
        }
    }

    return KEY_BLOCK;
}


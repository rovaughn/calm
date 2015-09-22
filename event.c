#include "event.h"
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

static struct termios saved_termios;
static int            caught_signal = 0;
static char           read_back = '\0';

static void handle_signal(int signum) {
  caught_signal = signum;
}

void prepare_events(void) {
  struct termios work;

  tcgetattr(STDIN_FILENO, &saved_termios);

  memcpy(&work, &saved_termios, sizeof work);

  work.c_lflag &= (~ECHO & ~ICANON);
  work.c_cc[VMIN] = 0;
  work.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &work);

  signal(SIGINT, handle_signal);
}

void cleanup_events(void) {
  tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
}

// True if 'c' should generate an E_KEY event.
static bool iskey(char c) {
    return isprint(c) || c == '\n' || c == '\b' || c == '\t';
}

event_t await_event(void) {
    event_t e = {.type = E_NULL};
    ssize_t nread;
    char    c;

    struct timespec req = {.tv_sec = 0, .tv_nsec = 1000000000/70};
    struct timespec rem;

    enum {
        START, // Initial state, keypresses interpreted normally.
        ESC1,  // Just got an \e character, awaiting a [ character.
        ESC2   // Just got \e[, next character is interpreted specially.
    } state = START;

read:
    if (caught_signal != 0) {
        e.type   = E_SIG;
        e.signum = caught_signal;
        return e;
    }

    if (read_back != '\0') {
        nread     = 1;
        c         = read_back;
        read_back = '\0';
    } else {
        nread = read(STDIN_FILENO, &c, sizeof c);

        if (nread < 0) {
            switch (errno) {
            case EAGAIN:
                goto read;
            default:
                perror("read stdin");
                exit(1);
            }
        } else if (nread != 1) {
            goto wait;
        }
    }

    switch (state) {
    case START:
        if (iskey(c)) {
            e.type = E_KEY;
            e.key  = c;
            return e;
        } else if (c == '\x1b') {
            state = ESC1;
            goto read;
        } else {
            fprintf(stderr, "Can't handle character %02x\n", c);
            exit(1);
        }
    case ESC1:
        if (c == '[') {
            state = ESC2;
            goto read;
        } else {
            // The last escape character was actually just an escape and not
            // part of an escape sequence, so we return it and push the current
            // character to read_back so it gets read next time await_event is
            // called.
            e.type    = E_KEY;
            e.key     = '\x1b';
            read_back = c;
            return e;
        }
    case ESC2:
        switch (c) {
        case '3': e.type = E_DELETE; break;
        case '4': e.type = E_END; break;
        case 'A': e.type = E_UP; break;
        case 'B': e.type = E_DOWN; break;
        case 'C': e.type = E_RIGHT; break;
        case 'D': e.type = E_LEFT; break;
        case 'H': e.type = E_HOME; break;
        default:
            fprintf(stderr, "Unknown sequence \\x1b[\\x%02x\n", c);
            exit(1);
        }

        return e;
    }


wait:
    nanosleep(&req, &rem);
    goto read;
}


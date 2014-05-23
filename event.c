#include "event.h"
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <ctype.h>

struct termios saved_termios;

void prepare_events(void) {
  struct termios work;

  tcgetattr(STDIN_FILENO, &saved_termios);
  work = saved_termios;
  work.c_lflag &= (~ECHO & ~ICANON);
  work.c_cc[VMIN] = 0;
  work.c_cc[VTIME] = 0;
  tcsetattr(STDIN_FILENO, TCSANOW, &work);
}

int cleanup_events(void) {
  return tcsetattr(STDIN_FILENO, TCSANOW, &saved_termios);
}

event_t await_event(void) {
  event_t e;

  e.type = E_NULL;

  char cbuf[4] = {0};

  fgets(cbuf, sizeof cbuf, stdin);

  char c = cbuf[0];

  if (c != '\0') {
    if (isprint(c)) {
      e.type = E_KEY;
      e.key  = c;
    } else if (c == '\n') {
      e.type = E_ENTER;
    } else if (c == 127) {
      e.type = E_BACKSPACE;
    } else if (c == 27 && cbuf[2] == 'A') {
      e.type = E_UP;
    } else if (c == 27 && cbuf[2] == 'B') {
      e.type = E_DOWN;
    } else if (c == 27 && cbuf[2] == 'C') {
      e.type = E_RIGHT;
    } else if (c == 27 && cbuf[2] == 'D') {
      e.type = E_LEFT;
    } else if (c == 27 && cbuf[2] == '3') {
      e.type = E_DELETE;
    } else if (c == 9) {
      e.type = E_TAB;
    } else if (c == 27 && cbuf[2] == 'H') {
      e.type = E_HOME;
    } else if (c == 27 && cbuf[2] == 'F') {
      e.type = E_END;
    } else {
      printf("[%d:%d:%d] (%d)\n", cbuf[0], cbuf[1], cbuf[2], c);
      exit(1);
    }
  }

  return e;
}


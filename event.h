#ifndef EVENT_H
#define EVENT_H

typedef enum {
  E_NULL, E_KEY,
  E_ENTER, E_BACKSPACE, E_LEFT, E_RIGHT, E_UP, E_DOWN, E_DELETE, E_TAB, E_HOME,
  E_END,
  E_SIG
} event_type_t;

typedef struct {
  event_type_t type;

  union {
    char key;
    int  signum;
  };
} event_t;

void prepare_events(void);
event_t await_event(void);
void cleanup_events(void);

#endif

#include <stdio.h>

enum ResultVariant { RESULT_OK, RESULT_ERR };

struct Error {
  unsigned int code;
  const char* message;
};

#define deriveResult(T)         \
  union _Result_##T {           \
    struct Error err;           \
    int ok;                     \
  };                            \
  struct Result_##T {           \
    enum ResultVariant type;    \
    union _Result_##T unwrap;   \
  };

#define Result_Err(ok_type, _code, _message) \
  ((struct Result_##ok_type) {               \
    .type = RESULT_ERR,                      \
    .unwrap = (union _Result_##ok_type) {    \
      .err = {                               \
        .code = (_code),                     \
        .message = (_message),               \
      }                                      \
    }                                        \
  })

#define Result_Ok(ok_type, _value)           \
  ((struct Result_##ok_type) {               \
    .type = RESULT_OK,                       \
    .unwrap = (union _Result_##ok_type) {    \
      .ok = (_value)                         \
    }                                        \
  })

// create Result_int
deriveResult(int);

// errors on negative params
struct Result_int area_rectangle(int l, int w) {
  if (l < 0 || w < 0) {
    return Result_Err(int, 1, "invalid dimensions");
  }

  return Result_Ok(int, l * w);
}

int main(void) {
  int l, w;
  char buffer[BUFSIZ];

  printf("input length and width:\n");
  fgets(buffer, BUFSIZ, stdin);
  sscanf(buffer, "%d %d", &l, &w);

  struct Result_int area = area_rectangle(l,w);

  union _Result_int unwrapped = area.unwrap;
  switch (area.type) {
    case RESULT_ERR:
      printf("ERROR: %s (Code %u)\n", unwrapped.err.message, unwrapped.err.code);
      return area.unwrap.err.code;
    case RESULT_OK:
      printf("The area is: %d\n", unwrapped.ok);
  }

  return 0;
}

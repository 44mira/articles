#include <stdio.h>

enum RotateType {
  FULL,
  ANGLE,
};

union _Rotate {
  int rotations;
  float angle;
};

typedef struct {
  enum RotateType type;
  union _Rotate unwrap;
} Rotate;

#define Rotate(_variant, _union) \
  ((Rotate) { .type = _variant, .unwrap = (union _Rotate) _union })

#define Rotate_FULL(_degrees) \
  ((Rotate) { .type = FULL, .unwrap = (union _Rotate) { .rotations = (_degrees) }})

#define Rotate_ANGLE(_angle) ((Rotate) { \
  .type = ANGLE,                         \
  .unwrap = (union _Rotate) {            \
    .angle = (_angle)                    \
  }                                      \
})

int main(void) {
  Rotate right_angle = Rotate_ANGLE(90.0f);
  Rotate doubleturn = Rotate_FULL(2);
  Rotate halfturn = Rotate(ANGLE, { .rotations = 180.0f });

  return 0;
}

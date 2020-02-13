#include <stdio.h>
#include <stdlib.h>
#include <string.h> /* strlen() */

/* =============================================================================
 * Morse code tree |
 *-----------------+                                                      (-)
 *                                                                         |
 *                                  (T -)----------------------------------+---------------------------(. E)
 *                                    |                                                                  |
 *                   (M -)------------+-------------(. N)                              (A -)-------------+-----------(. I)
 *                     |                              |                                  |                             |
 *          (O -)------+-----(. G)            (K -)---+----(. D)                  (W -)--+--(. R)          (U -)-------+----(. S)
 *            |               |                |             |                      |         |              |                |
 *      (-)---+--(.)    (Q -)-+-(. Z)   (Y -)-+-(. C)  (X -)-+-(. B)          (J -)-+-(. P)   +-(. L)    (-)-+-(. F)   (V -)--+----(. H)
 *       |        |               |                              |              |                         |              |           |
 * (0 -)-+-(. 9)  +-(. 8)         +-(. 7)                        +-(. 6)  (1 -)-+                   (2 -)-+        (3 -)-+     (4 -)-+-(. 5)
 *
 * ========================================================================== */

#define MORSE_CODE_OK     0 /* no errors */
#define MORSE_CODE_EINVCH 1 /* invalid character */
#define MORSE_CODE_EINVSQ 2 /* invalid morse code sequence */
#define MORSE_CODE_EBUFOF 3 /* operation will result in a buffer overflow */

#define MORSE_CODE_MAP_LEN (sizeof(MORSE_CODE_MAP) / sizeof(MORSE_CODE_MAP[0]))
#define MORSE_CODE_BIT(a, b, c, d, e, f) \
  (a << 5 | b << 4 | c << 3 | d << 2 | e << 1 | f << 0)

/* binary codes according to the tree above
 * - => 1
 * . => 0
 */
#define MORSE_CODE_A     MORSE_CODE_BIT(0, 0, 0, 1, 0, 1)
#define MORSE_CODE_B     MORSE_CODE_BIT(0, 1, 1, 0, 0, 0)
#define MORSE_CODE_C     MORSE_CODE_BIT(0, 1, 1, 0, 1, 0)
#define MORSE_CODE_D     MORSE_CODE_BIT(0, 0, 1, 1, 0, 0)
#define MORSE_CODE_E     MORSE_CODE_BIT(0, 0, 0, 0, 1, 0)
#define MORSE_CODE_F     MORSE_CODE_BIT(0, 1, 0, 0, 1, 0)
#define MORSE_CODE_G     MORSE_CODE_BIT(0, 0, 1, 1, 1, 0)
#define MORSE_CODE_H     MORSE_CODE_BIT(0, 1, 0, 0, 0, 0)
#define MORSE_CODE_I     MORSE_CODE_BIT(0, 0, 0, 1, 0, 0)
#define MORSE_CODE_J     MORSE_CODE_BIT(0, 1, 0, 1, 1, 1)
#define MORSE_CODE_K     MORSE_CODE_BIT(0, 0, 1, 1, 0, 1)
#define MORSE_CODE_L     MORSE_CODE_BIT(0, 1, 0, 1, 0, 0)
#define MORSE_CODE_M     MORSE_CODE_BIT(0, 0, 0, 1, 1, 1)
#define MORSE_CODE_N     MORSE_CODE_BIT(0, 0, 0, 1, 1, 0)
#define MORSE_CODE_O     MORSE_CODE_BIT(0, 0, 1, 1, 1, 1)
#define MORSE_CODE_P     MORSE_CODE_BIT(0, 1, 0, 1, 1, 0)
#define MORSE_CODE_Q     MORSE_CODE_BIT(0, 1, 1, 1, 0, 1)
#define MORSE_CODE_R     MORSE_CODE_BIT(0, 0, 1, 0, 1, 0)
#define MORSE_CODE_S     MORSE_CODE_BIT(0, 0, 1, 0, 0, 0)
#define MORSE_CODE_T     MORSE_CODE_BIT(0, 0, 0, 0, 1, 1)
#define MORSE_CODE_U     MORSE_CODE_BIT(0, 0, 1, 0, 0, 1)
#define MORSE_CODE_V     MORSE_CODE_BIT(0, 1, 0, 0, 0, 1)
#define MORSE_CODE_W     MORSE_CODE_BIT(0, 0, 1, 0, 1, 1)
#define MORSE_CODE_X     MORSE_CODE_BIT(0, 1, 1, 0, 0, 1)
#define MORSE_CODE_Y     MORSE_CODE_BIT(0, 1, 1, 0, 1, 1)
#define MORSE_CODE_Z     MORSE_CODE_BIT(0, 1, 1, 1, 0, 0)
#define MORSE_CODE_0     MORSE_CODE_BIT(1, 1, 1, 1, 1, 1)
#define MORSE_CODE_1     MORSE_CODE_BIT(1, 0, 1, 1, 1, 1)
#define MORSE_CODE_2     MORSE_CODE_BIT(1, 0, 0, 1, 1, 1)
#define MORSE_CODE_3     MORSE_CODE_BIT(1, 0, 0, 0, 1, 1)
#define MORSE_CODE_4     MORSE_CODE_BIT(1, 0, 0, 0, 0, 1)
#define MORSE_CODE_5     MORSE_CODE_BIT(1, 0, 0, 0, 0, 0)
#define MORSE_CODE_6     MORSE_CODE_BIT(1, 1, 0, 0, 0, 0)
#define MORSE_CODE_7     MORSE_CODE_BIT(1, 1, 1, 0, 0, 0)
#define MORSE_CODE_8     MORSE_CODE_BIT(1, 1, 1, 1, 0, 0)
#define MORSE_CODE_9     MORSE_CODE_BIT(1, 1, 1, 1, 1, 0)
#define MORSE_CODE_SPACE MORSE_CODE_BIT(0, 0, 0, 0, 0, 1)

/* Any uninitialized element will be equal to 0. This will be useful to check for
 * (in)valid sequences.
 */
static const int MORSE_CODE_MAP[] = {
  [MORSE_CODE_A]     = 'A',
  [MORSE_CODE_B]     = 'B',
  [MORSE_CODE_C]     = 'C',
  [MORSE_CODE_D]     = 'D',
  [MORSE_CODE_E]     = 'E',
  [MORSE_CODE_F]     = 'F',
  [MORSE_CODE_G]     = 'G',
  [MORSE_CODE_H]     = 'H',
  [MORSE_CODE_I]     = 'I',
  [MORSE_CODE_J]     = 'J',
  [MORSE_CODE_K]     = 'K',
  [MORSE_CODE_L]     = 'L',
  [MORSE_CODE_M]     = 'M',
  [MORSE_CODE_N]     = 'N',
  [MORSE_CODE_O]     = 'O',
  [MORSE_CODE_P]     = 'P',
  [MORSE_CODE_Q]     = 'Q',
  [MORSE_CODE_R]     = 'R',
  [MORSE_CODE_S]     = 'S',
  [MORSE_CODE_T]     = 'T',
  [MORSE_CODE_U]     = 'U',
  [MORSE_CODE_V]     = 'V',
  [MORSE_CODE_W]     = 'W',
  [MORSE_CODE_X]     = 'X',
  [MORSE_CODE_Y]     = 'Y',
  [MORSE_CODE_Z]     = 'Z',
  [MORSE_CODE_0]     = '0',
  [MORSE_CODE_1]     = '1',
  [MORSE_CODE_2]     = '2',
  [MORSE_CODE_3]     = '3',
  [MORSE_CODE_4]     = '4',
  [MORSE_CODE_5]     = '5',
  [MORSE_CODE_6]     = '6',
  [MORSE_CODE_7]     = '7',
  [MORSE_CODE_8]     = '8',
  [MORSE_CODE_9]     = '9',
  [MORSE_CODE_SPACE] = ' '
};

static const char *morse_code_messages[]= {
  [MORSE_CODE_OK]     = "OK",
  [MORSE_CODE_EINVCH] = "Invalid character",
  [MORSE_CODE_EINVSQ] = "Invalid Morse Code sequence",
  [MORSE_CODE_EBUFOF] = "Operation will result in a buffer overflow"
};

const char *morse_code_error(const int code);
int morse_code_decode(const char *src, const size_t src_len, char *dst, const size_t dst_len, size_t *pos);

int main()
{
  char *src = "-- --- .-. ... . / -.-. --- -.. .";
  char dst[11] = { 0 };
  size_t pos;
  int res;

  res = morse_code_decode(src, strlen(src), dst, sizeof(dst) / sizeof(dst[0]), &pos);

  if (res == MORSE_CODE_OK) {
    printf("OK: dst: '%s'\n", dst);
  } else {
    printf("FAIL: pos: %zu, err: %d, %s\n", pos, res, morse_code_error(res));
  }
  return 0;
}

int morse_code_decode(const char *src, const size_t src_len, char *dst, const size_t dst_len, size_t *pos)
{
  size_t acc = MORSE_CODE_SPACE;
  size_t dst_index = 0;
  size_t src_index = 0;

  if (src_len == 0) {
    return MORSE_CODE_OK;
  }
  for (src_index = 0; src_index <= src_len; src_index++) {
    if (src[src_index] == ' ' || src_index == src_len) {
      if (dst_index + 1 >= dst_len) { /* guard for buffer overflows */
        if (pos != NULL) {
          *pos = src_index;
        }
        return -MORSE_CODE_EBUFOF;
      }
      if (acc >= MORSE_CODE_MAP_LEN) { /* guard for morse code map index range */
        if (pos != NULL) {
          *pos = src_index;
        }
        return -MORSE_CODE_EINVSQ;
      }
      if (MORSE_CODE_MAP[acc] == 0) { /* guard for invalid morse code sequences */
        if (pos != NULL) {
          *pos = src_index;
        }
        return -MORSE_CODE_EINVSQ;
      }
      dst[dst_index] = MORSE_CODE_MAP[acc];
      dst_index++;
      acc = MORSE_CODE_SPACE;
    } else if (src[src_index] == '-') {
      acc = (acc << 1) | 1;
    } else if (src[src_index] == '.') {
      acc = acc << 1;
    } else if (src[src_index] == '/') {
      acc = MORSE_CODE_SPACE;
    } else {
      if (pos != NULL) {
        *pos = src_index;
      }
      return -MORSE_CODE_EINVCH;
    }
  }
  dst[dst_index] = '\0'; /* ensure nul character at the end of the string */
  return MORSE_CODE_OK;
}

const char *morse_code_error(const int code)
{
  return morse_code_messages[-code];
}

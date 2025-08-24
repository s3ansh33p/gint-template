#include <gint/clock.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/prof.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Increase for large factorials
#define BIGINT_MAX_DIGITS 512
typedef struct {
  uint8_t digits[BIGINT_MAX_DIGITS]; // Least significant digit first
  int len;
} bigint_t;

void bigint_init(bigint_t *b, int value) {
  for (int i = 0; i < BIGINT_MAX_DIGITS; i++)
    b->digits[i] = 0;
  b->len = 0;
  while (value > 0) {
    b->digits[b->len++] = value % 10;
    value /= 10;
  }
  if (b->len == 0)
    b->len = 1;
}

void bigint_mul_small(bigint_t *b, int x) {
  int carry = 0;
  for (int i = 0; i < b->len; i++) {
    int prod = b->digits[i] * x + carry;
    b->digits[i] = prod % 10;
    carry = prod / 10;
  }
  while (carry > 0 && b->len < BIGINT_MAX_DIGITS) {
    b->digits[b->len++] = carry % 10;
    carry /= 10;
  }
}

void bigint_to_str(bigint_t *b, char *out, int outsize) {
  int idx = 0;
  for (int i = b->len - 1; i >= 0 && idx < outsize - 1; i--) {
    out[idx++] = '0' + b->digits[i];
  }
  out[idx] = '\0';
}

int main(void) {
  prof_init();
  dclear(C_WHITE);
  int dy = 16;
  int y = 1;
  dtext(1, y, C_BLACK, "Benchmarking...");
  y += dy;
  dupdate();

  // First benchmark: 100,000 dtext calls
  uint32_t elapsed_us = prof_exec({
    for (int i = 1; i <= 100000; i++) {
      dtext(1, y, C_BLACK, "Benchmark");
    }
  });
  y += dy;
  char buf[48];
  snprintf(buf, sizeof(buf), "100k Print: %u us (%u.%06u s)", elapsed_us,
           elapsed_us / 1000000, elapsed_us % 1000000);
  dtext(1, y, C_BLACK, buf);
  y += dy;
  dupdate();

  // Second benchmark: factorial loop using big integer
  bigint_t f;
  char fact_buf[520];
  uint32_t fact_time_us = prof_exec({
    for (int j = 0; j < 500; j++) {
      bigint_init(&f, 1);
      for (int i = 2; i <= 250; i++) {
        bigint_mul_small(&f, i);
      }
    }
  });
  bigint_to_str(&f, fact_buf, sizeof(fact_buf));
  dtext(1, y, C_BLACK, "fact(250):");
  y += dy;
  int len = strlen(fact_buf);
  for (int i = 0; i < len; i += 32) {
    char line[33];
    int chunk = (i + 32 < len) ? 32 : (len - i);
    strncpy(line, fact_buf + i, chunk);
    line[chunk] = '\0';
    dtext(1, y, C_BLACK, line);
    y += dy;
  }
  char sec_buf[48];
  snprintf(sec_buf, sizeof(sec_buf), "Factorial: %u us (%u.%06u s)",
           fact_time_us, fact_time_us / 1000000, fact_time_us % 1000000);
  dtext(1, y, C_BLACK, sec_buf);
  y += dy;
  dupdate();

  // Third benchmark: empty loop 1,000,000 iterations
  volatile int dummy = 0;
  uint32_t empty_time_us = prof_exec({
    for (int i = 0; i < 1000000; i++) {
      dummy++;
    }
  });
  char empty_buf[48];
  snprintf(empty_buf, sizeof(empty_buf), "Loop 1M: %u us (%u.%06u s)",
           empty_time_us, empty_time_us / 1000000, empty_time_us % 1000000);
  dtext(1, y, C_BLACK, empty_buf);
  y += dy;
  dupdate();

  prof_quit();
  getkey();
  return 1;
}

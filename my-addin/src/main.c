#include <gint/clock.h>
#include <gint/display.h>
#include <gint/keyboard.h>
#include <gint/prof.h>
#include <stdio.h>

int main(void) {
  prof_init();
  dclear(C_WHITE);
  dtext(1, 1, C_BLACK, "Benchmarking...");
  dupdate();

  // First benchmark: 100,000 dtext calls
  uint32_t elapsed_us = prof_exec({
    for (int i = 1; i <= 100000; i++) {
      dtext(1, 16, C_BLACK, "Benchmark");
    }
  });
  char buf[48];
  snprintf(buf, sizeof(buf), "100k Print: %u us (%u.%06u s)", elapsed_us,
           elapsed_us / 1000000, elapsed_us % 1000000);
  dtext(1, 32, C_BLACK, buf);
  dupdate();

  // Second benchmark: factorial loop
  unsigned long long f = 1;
  uint32_t fact_time_us = prof_exec({
    unsigned long long local_f = 1;
    for (int j = 0; j < 500; j++) {
      local_f = 1;
      for (int i = 2; i <= 250; i++) {
        local_f *= i;
      }
    }
    f = local_f;
  });
  char fact_buf[32];
  snprintf(fact_buf, sizeof(fact_buf), "f=%llu", f);
  dtext(1, 48, C_BLACK, fact_buf);
  char sec_buf[48];
  snprintf(sec_buf, sizeof(sec_buf), "Factorial: %u us (%u.%06u s)",
           fact_time_us, fact_time_us / 1000000, fact_time_us % 1000000);
  dtext(1, 64, C_BLACK, sec_buf);
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
  dtext(1, 80, C_BLACK, empty_buf);
  dupdate();

  prof_quit();
  getkey();
  return 1;
}

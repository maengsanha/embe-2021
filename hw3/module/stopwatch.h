/*
 * Embedded System Software, 2021
 *
 * stopwatch.h - stop watch information definition
 */

/**
 * struct stopwatch_t - stopwatch information
 *
 * @count:   increased every 0.1 second
 * @fnd_val: the value to write to FND device
 * @paused:  whether paused or not
 */
struct stopwatch_t {
  int count;
  int fnd_val;
  int paused;
};

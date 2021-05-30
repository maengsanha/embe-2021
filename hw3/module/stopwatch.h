/*
 * Embedded System Software, 2021
 *
 * stopwatch.h - stop watch device drvier definition
 */

/**
 * struct stopwatch_t - stopwatch information
 *
 * @count:   stopwatch count
 * @fnd_val: FND device value
 * @paused:  paused
 */
struct stopwatch_t {
  int count;
  int fnd_val;
  int paused;
};
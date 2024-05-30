#ifndef LOGGING_H
#define LOGGING_H

#include <stdint.h>
#include <stdbool.h>

struct /*__attribute__((packed))*/ LogData {
  uint32_t lattitude;
  uint32_t longtitude;
  uint32_t temperature;
  uint32_t humidity;
};

struct /*__attribute__((packed))*/ MetaData {
  uint32_t countOfRecords;
  uint32_t pad32;
  uint64_t pad64;
};

void PerodicLogging(void);

#endif // LOGGING_H

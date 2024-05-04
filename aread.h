#ifndef __AREAD_HEADER__
#define __AREAD_HEADER__

#include <Arduino.h>

class AnalogReader {
private:
  const int pin;
  const unsigned int sample_size;

public:
  AnalogReader(
    const int pin,
    const unsigned int sample_size)
    : pin(pin),
      sample_size(sample_size) {}

  unsigned int as_int() const {
    unsigned long total = 0;
    for (int i = 0; i < this->sample_size; i++) {
      total += (unsigned long)analogRead(this->pin);
    }
    return total / this->sample_size;
  }

  float as_float() const {
    return (float)this->as_int() / (float)1024;
  }
};

class ResistanceReader {
public:
  virtual float as_resistance() const;
};

enum struct read_resistance_result_e {
  OK,
  FAIL_TOO_HIGH_RESISTANCE,
  FAIL_TOO_LOW_RESISTANCE,
};

class VoltageDividingResistanceReader : public AnalogReader, public ResistanceReader {
private:
  const float pullup_resistance;

public:
  VoltageDividingResistanceReader(
    const int pin,
    const unsigned int sample_size,
    const float pullup_resistance)
    : AnalogReader(pin, sample_size),
      pullup_resistance(pullup_resistance) {}

  read_resistance_result_e read_resistance(float *r_ptr) {
    const float x = this->as_float();
    *r_ptr = x / (1 - x) * this->pullup_resistance;
    if (x > 0.95) {
      return read_resistance_result_e::FAIL_TOO_HIGH_RESISTANCE;
    } else if (x < 0.05) {
      return read_resistance_result_e::FAIL_TOO_LOW_RESISTANCE;
    } else {
      return read_resistance_result_e::OK;
    }
  }

  float as_resistance() const {
    float r;
    const read_resistance_result_e result = this->read_resistance(&r);
    if (result != read_resistance_result_e::OK) {
      return NAN;
    }
    return r;
  }
};

#endif /* __AREAD_HEADER__ */
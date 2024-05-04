#ifndef __TGS2450_HEADER__
#define __TGS2450_HEADER__

#include "aread.h"
#include "utils.h"

class _TGS2450_IO {
private:
  const int pin_heat;
  const int pin_sens;
  const ResistanceReader *resistance_reader;

protected:
  _TGS2450_IO(
    const int pin_heat,
    const int pin_sens,
    const ResistanceReader *const resistance_reader)
    : pin_heat(pin_heat),
      pin_sens(pin_sens),
      resistance_reader(resistance_reader) {}

  void setup_io() const {
    pinMode(this->pin_heat, OUTPUT);
    digitalWrite(this->pin_heat, HIGH);
    pinMode(this->pin_sens, OUTPUT);
    digitalWrite(this->pin_heat, HIGH);
  }

  void start_heating() const {
    digitalWrite(this->pin_heat, LOW);
  }

  void end_heating() const {
    digitalWrite(this->pin_heat, HIGH);
  }

  void start_sensing() const {
    digitalWrite(this->pin_sens, LOW);
  }

  void end_sensing() const {
    digitalWrite(this->pin_sens, HIGH);
  }

  float read_resistance() const {
    return this->resistance_reader->as_resistance();
  }
};

class TGS2450 : public _TGS2450_IO {
  static const int HEATING_INTERVAL_MS = 250;
  static const int HEATING_TIME_MS = 8;

  const IntervalTimer timer;

  bool new_value = true;
  float sens_resistance = NAN;

protected:
  void heat() {
    this->start_heating();
    delay(HEATING_TIME_MS);
    this->end_heating();
  }

  void sense() {
    this->start_sensing();
    this->sens_resistance = this->read_resistance();
    this->end_sensing();
    this->new_value = true;
  }

public:
  TGS2450(
    const int pin_heat,
    const int pin_sens,
    const ResistanceReader *resistance_reader,
    const unsigned long sens_interval)
    : _TGS2450_IO(pin_heat, pin_sens, resistance_reader),
      timer(IntervalTimer(HEATING_INTERVAL_MS)) {}

  void setup() {
    this->setup_io();
  }

  void loop() {
    if (this->timer.check_and_update()) {
      this->heat();
      this->sense();
    }
  }

  bool has_new_value() {
    if (!this->new_value) {
      return false;
    }
    this->new_value = false;
    return true;
  }

  float get_resistance() {
    return this->sens_resistance;
  }
};

namespace tgs_utils {

enum struct gas_type_e {
  ETHANOL = 0,
  AMMONIA = 1,
  HYDROGEN_SULFIDE = 2,
  METHYL_MERCAPTAN = 3,
};

extern const int n_gas_types;

float tgs2450_convert(gas_type_e mode, float r_meas, float r_air);

}

class TGS2450Calibrator {
  TGS2450 *gas;

  SampleLPF<float> lpf;
  SampleAverager<float> averager;

  float r = NAN;

protected:
  bool retrieve_sample(float *r_ptr) {
    if (!this->gas->has_new_value()) {
      return false;
    }
    *r_ptr = this->gas->get_resistance();
    return true;
  }

  bool validate_sample(float r) {
    if (isnan(r)) {
      return false;
    }
    if (this->lpf.is_rescalable_sample(r)) {
      return false;
    }
    return true;
  }

public:
  TGS2450Calibrator(TGS2450 *gas)
    : gas(gas),
      lpf(SampleLPF<float>(0.99, 1.02)),
      averager(SampleAverager<float>(30)) {
  }

  int get_progress_total() const {
    return this->averager.get_n_samples();
  }

  int get_progress() const {
    return this->averager.get_count();
  }

  float get_last_r() const {
    return this->r;
  }

  bool loop() {
    this->gas->loop();

    if (!this->retrieve_sample(&this->r)) {
      return true;  // continue
    }
    if (this->validate_sample(this->r)) {
      this->lpf.append_sample(this->r);
      this->averager.append_sample(this->r);
    } else {
      if (!isnan(r)) {
        this->lpf.append_sample(this->r);
      }
      this->averager.clear();
    }
    return !this->averager.is_full();
  }

  float get_calibration_registance() const {
    return this->averager.get_value();
  }
};

#endif /* __TGS2450_HEADER__ */
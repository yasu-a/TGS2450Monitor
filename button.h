#ifndef __BUTTON_HEADER__
#define __BUTTON_HEADER__

#include <Arduino.h>
#include "utils.h"

class PressButton {
  IntervalTimer timer;
  const int pin;
  int state = 0;
  int long_press_count = 0;
  bool flag_pressed = false;
  bool flag_released = false;

  void setup_io() const {
    pinMode(this->pin, INPUT_PULLUP);
  }

public:
  PressButton(const int pin)
    : pin(pin), timer(IntervalTimer(1)) {}

  bool read_state() const {
    return digitalRead(this->pin) == LOW;
  }

  void setup() {
    this->setup_io();
  }

  void loop() {
    if (this->timer.check_and_update()) {
      bool state = this->read_state();
      switch (this->state) {
        case 0:
          if (state) {
            this->state++;
            this->long_press_count = 0;
            this->flag_pressed = true;
            this->flag_released = false;
          }
          break;
        case 1:
          if (state) {
            this->long_press_count++;
          } else {
            this->state++;
          }
          break;
        case 2:
        case 3:
          if (state) {
            this->state = 1;
          } else {
            this->state++;
          }
          break;
        case 4:
          if (state) {
            this->state = 1;
          } else {
            this->state = 0;
            this->flag_released = true;
          }
          break;
      }
    }
  }

  bool is_pressed() {
    return this->flag_pressed;
  }

  bool is_released() {
    return this->flag_released;
  }

  int get_long_press_count() {
    return this->long_press_count;
  }

  void clear_events() {
    this->flag_pressed = false;
    this->flag_released = false;
  }
};

#endif /* __BUTTON_HEADER__ */
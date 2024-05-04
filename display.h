#ifndef __DISPLAY_HEADER__
#define __DISPLAY_HEADER__

#include <LiquidCrystal.h>

class Display {
  const LiquidCrystal *lcd;

public:
  Display(
    LiquidCrystal *lcd)
    : lcd(lcd) {}

  void setup() {
    lcd->begin(16, 2);
    lcd->clear();
  }

  void show_calibration(int progress, int total, float r_last, float r_calib) {
    char buf[32];

    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("CALIBRATING...");
    lcd->setCursor(0, 1);
    sprintf(buf, "%2d/%2d %4d->%4d", progress, total, int(r_last), int(r_calib));
    lcd->print(buf);
  }

  void show_calibration_done(float r_air) {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print("CALIBRATION DONE");
    lcd->setCursor(0, 1);
    lcd->print("r0: ");
    lcd->print(r_air, 2);
  }

  void show_measurement(const char *gas_name, float gas_conc_ppm, float r_meas, float r_air) {
    char buf[8];
    dtostrf(gas_conc_ppm, 5, 2, buf);
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(gas_name);
    lcd->print(" ");
    lcd->print(buf);
    lcd->print(" ppm");
    lcd->setCursor(0, 1);
    lcd->print(r_meas, 0);
    lcd->print("/");
    lcd->print(r_air, 0);
    lcd->print("=");
    lcd->print(r_meas / r_air, 3);
  }
};

#endif /* __DISPLAY_HEADER__ */
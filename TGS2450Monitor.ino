#include <LiquidCrystal.h>
#include "storage.h"
#include "display.h"
#include "tgs2450.h"
#include "aread.h"
#include "button.h"

#define PIN_LCD_RS 12
#define PIN_LCD_EN 11
#define PIN_LCD_D4 5
#define PIN_LCD_D5 4
#define PIN_LCD_D6 3
#define PIN_LCD_D7 2

#define PIN_TGS2450_HEAT 7
#define PIN_TGS2450_SENS 10
#define PIN_TGS2450_OUT A0

#define PIN_BUTTON 9

Storage storage;

Display *display;
TGS2450 *gas;
PressButton *button;

void setup() {
  Serial.begin(9600);

  storage.load();

  static LiquidCrystal lcd(PIN_LCD_RS, PIN_LCD_EN, PIN_LCD_D4, PIN_LCD_D5, PIN_LCD_D6, PIN_LCD_D7);
  display = new Display(&lcd);
  display->setup();

#define TGS_R_PULLUP (470 + 20)
  static VoltageDividingResistanceReader resistance_reader(PIN_TGS2450_OUT, 8, TGS_R_PULLUP);
  gas = new TGS2450(PIN_TGS2450_HEAT, PIN_TGS2450_SENS, &resistance_reader, 1000);
  gas->setup();

  button = new PressButton(PIN_BUTTON);
  button->setup();

  if (button->read_state()) {
    tgs2450_calibration_on_setup();
    for (;;) delay(1000);  // never return
  }
}

int tgs2450_calibration_on_setup() {
  TGS2450Calibrator calib(gas);

  static unsigned long t_updated_ms = 0;
  while (calib.loop()) {
    if (millis() - t_updated_ms >= 500) {
      t_updated_ms = millis();
      display->show_calibration(calib.get_progress(),
                                calib.get_progress_total(),
                                calib.get_last_r(),
                                calib.get_calibration_registance());
    }
  }

  storage.app_data.r_air = calib.get_calibration_registance();
  storage.save();
  display->show_calibration_done(storage.app_data.r_air);
}

tgs_utils::gas_type_e next_gas_type(tgs_utils::gas_type_e gas_type) {
  using namespace tgs_utils;
  int idx = (int)gas_type;
  idx = (idx + 1) % n_gas_types;
  return (gas_type_e)idx;
}

const char *tgs_mode_to_string(tgs_utils::gas_type_e gas_type) {
  static const char *strings[] = {
    "ETH ",
    "NH3 ",
    "NO2 ",
    "CH4S",
  };
  return strings[(int)gas_type];
}

void loop() {
  gas->loop();
  button->loop();

  bool need_display_update = false;
  static tgs_utils::gas_type_e gas_type = (tgs_utils::gas_type_e)0;
  if (button->is_pressed()) {
    gas_type = next_gas_type(gas_type);
    need_display_update = true;
  }
  button->clear_events();

  static float r_meas = NAN;
  if (gas->has_new_value()) {
    float r_meas_retrieved = gas->get_resistance();
    if (r_meas != r_meas_retrieved) {
      need_display_update = true;
      r_meas = r_meas_retrieved;
    }
  }
  if (need_display_update) {
    const char *gas_name = tgs_mode_to_string(gas_type);
    const float r_air = storage.app_data.r_air;
    const float gas_conc_ppm = tgs2450_convert(gas_type, r_meas, r_air);
    display->show_measurement(gas_name, gas_conc_ppm, r_meas, r_air);
  }
}
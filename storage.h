#ifndef __HEADER_STORAGE__
#define __HEADER_STORAGE__

#include <EEPROM.h>

#define STORAGE_EEPROM_ADDRESS_OFFSET 0

typedef struct {
  float r_air;
} storage_appdata_s;

class Storage {
public:
  storage_appdata_s app_data;

  void load() {
    EEPROM.get(STORAGE_EEPROM_ADDRESS_OFFSET, this->app_data);
  }

  void save() {
    EEPROM.put(STORAGE_EEPROM_ADDRESS_OFFSET, this->app_data);
  }
};

#endif /* __HEADER_STORAGE__ */
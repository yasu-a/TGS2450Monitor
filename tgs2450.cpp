#include "tgs2450.h"

const int tgs_utils::n_gas_types = 4;

static double rs_per_r0_to_gas_conc(const double rs_per_r0, const double a, const double b) {
  return pow(10, (log10(rs_per_r0) - b) / a);
}

float tgs_utils::tgs2450_convert(tgs_utils::gas_type_e gas_type, float r_meas, float r_air) {
  const float rs_per_r0 = r_meas / r_air;
  switch (gas_type) {
    case gas_type_e::ETHANOL:
      if (rs_per_r0 > 0.02) {
        return rs_per_r0_to_gas_conc(rs_per_r0, -0.699, -0.301);
      } else {
        return rs_per_r0_to_gas_conc(rs_per_r0, -0.834, -0.031);
      }
      break;
    case gas_type_e::AMMONIA:
      if (rs_per_r0 > 0.4) {
        return rs_per_r0_to_gas_conc(rs_per_r0, -0.509, -0.155);
      } else {
        return rs_per_r0_to_gas_conc(rs_per_r0, -1.125, 0.139);
      }
      break;
    case gas_type_e::HYDROGEN_SULFIDE:
      if (rs_per_r0 > 0.14) {
        return rs_per_r0_to_gas_conc(rs_per_r0, -0.632, -0.854);
      } else {
        return rs_per_r0_to_gas_conc(rs_per_r0, -1.343, -0.854);
      }
      break;
    case gas_type_e::METHYL_MERCAPTAN:
      if (rs_per_r0 > 0.2) {
        return rs_per_r0_to_gas_conc(rs_per_r0, -0.778, -1.106);
      } else {
        return rs_per_r0_to_gas_conc(rs_per_r0, -1.602, -1.537);
      }
      break;
    default:
      return NAN;
  }
}
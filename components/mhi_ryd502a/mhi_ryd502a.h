#pragma once

#include "esphome/components/climate_ir/climate_ir.h"

namespace esphome::mhi_ryd502a {

// Temperature range
static const float MHI_RYD502A_TEMP_MIN = 18.0f;
static const float MHI_RYD502A_TEMP_MAX = 30.0f;
static const float MHI_RYD502A_TEMP_INC = 1.0f;

// Enum to represent horizontal directios
enum HorizontalDirection : uint8_t {
  HORIZONTAL_DIRECTION_LEFT = 0,
  HORIZONTAL_DIRECTION_MIDDLE_LEFT = 1,
  HORIZONTAL_DIRECTION_MIDDLE = 2,
  HORIZONTAL_DIRECTION_MIDDLE_RIGHT = 3,
  HORIZONTAL_DIRECTION_RIGHT = 4,
  HORIZONTAL_DIRECTION_SPLIT = 10,
  HORIZONTAL_DIRECTION_FOCUS = 11,
};

// Enum to represent vertical directions
enum VerticalDirection : uint8_t {
  VERTICAL_DIRECTION_UP = 0,
  VERTICAL_DIRECTION_MIDDLE_UP = 1,
  VERTICAL_DIRECTION_MIDDLE = 2,
  VERTICAL_DIRECTION_MIDDLE_DOWN = 3,
  VERTICAL_DIRECTION_DOWN = 4,
};

// Use climate::CLIMATE_FAN_OFF as the lowest fan speed (not fan off)

class MhiClimate final : public climate_ir::ClimateIR {
 public:
  MhiClimate()
      : climate_ir::ClimateIR(
          MHI_RYD502A_TEMP_MIN, MHI_RYD502A_TEMP_MAX, MHI_RYD502A_TEMP_INC, true, true
          ,{climate::CLIMATE_FAN_AUTO, climate::CLIMATE_FAN_OFF, climate::CLIMATE_FAN_LOW, climate::CLIMATE_FAN_MEDIUM, climate::CLIMATE_FAN_HIGH}
          ,{climate::CLIMATE_SWING_OFF, climate::CLIMATE_SWING_BOTH, climate::CLIMATE_SWING_VERTICAL, climate::CLIMATE_SWING_HORIZONTAL}
          // ,{climate::CLIMATE_PRESET_NONE, climate::CLIMATE_PRESET_ECO, climate::CLIMATE_PRESET_BOOST}
        ) {}

  void set_horizontal_default(HorizontalDirection horizontal_direction) {
    this->default_horizontal_direction_ = horizontal_direction;
  }
  void set_vertical_default(VerticalDirection vertical_direction) {
    this->default_vertical_direction_ = vertical_direction;
  }

 protected:
  /// Transmit via IR the state of this climate controller.
  void transmit_state() override;

  HorizontalDirection default_horizontal_direction_;
  VerticalDirection default_vertical_direction_;
};

}  // namespace esphome::mhi_ryd502a

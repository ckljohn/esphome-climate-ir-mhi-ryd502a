#include "mhi_ryd502a.h"
#include "esphome/core/log.h"

namespace esphome::mhi_ryd502a {

static const char *const TAG = "mhi_ryd502a.climate";

/********************************************************************************
 *  TIMINGS
 *  Space:        Not used
 *  Header Mark:  5000 us
 *  Header Space: 5000 us
 *  Bit Mark:      500 us
 *  Zero Space:    500 us
 *  One Space:    1500 us
 *******************************************************************************/
static const uint32_t MHI_RYD502A_HDR_MARK = 5000;
static const uint32_t MHI_RYD502A_HDR_SPACE = 5000;
static const uint32_t MHI_RYD502A_BIT_MARK = 500;
static const uint32_t MHI_RYD502A_ZERO_SPACE = 500;
static const uint32_t MHI_RYD502A_ONE_SPACE = 1500;

/********************************************************************************
 *
 * Codes
 *
 *******************************************************************************/
static const uint8_t MHI_RYD502A_STATE_LENGTH = 16;

// Power
static const uint8_t MHI_RYD502A_POWER_OFF = 0;
static const uint8_t MHI_RYD502A_POWER_ON = 1;

// Operating Modes
static const uint8_t MHI_RYD502A_MODE_HEAT = 1;
static const uint8_t MHI_RYD502A_MODE_COOL = 2;
static const uint8_t MHI_RYD502A_MODE_DRY = 3;
static const uint8_t MHI_RYD502A_MODE_FAN = 4;
static const uint8_t MHI_RYD502A_MODE_HEAT_COOL = 5;

// Fan control
static const uint8_t MHI_RYD502A_FAN_AUTO = 6;
static const uint8_t MHI_RYD502A_FAN_L1 = 2;
static const uint8_t MHI_RYD502A_FAN_L2 = 3;
static const uint8_t MHI_RYD502A_FAN_L3 = 4;
static const uint8_t MHI_RYD502A_FAN_L4 = 5;

// Swing
static const uint8_t MHI_RYD502A_SWING_HDIR = 5;  // Horizontal Swing
static const uint8_t MHI_RYD502A_SWING_VDIR = 5;  // Vertical Swing

void MhiClimate::transmit_state() {
  // Byte 0-1: Constant: 0x55, 0x00
  // Byte 2: ON/OFF, Mode, Fan speed.
  // Byte 3: Temperature
  // Byte 4-6: Constant: 0x00, 0x00, 0x00
  // Byte 7: ECO mode
  // Byte 8: FAN Vertical
  // Byte 9: FAN Horizontal
  // Byte 10: Powerful mode
  // Byte 11-12: Constant: 0x00, 0x00
  // Byte 13: Checksum
  // Byte 14: Constant 0x01
  // Byte 15: Constant 0xaa
  uint8_t remote_state[MHI_RYD502A_STATE_LENGTH] = {0};

  remote_state[0] = 0x55;
  remote_state[2] = MHI_RYD502A_POWER_ON;
  remote_state[8] = 0x41;
  remote_state[9] = 0x10;
  remote_state[10] = 0x24;
  remote_state[14] = 0x01;
  remote_state[15] = 0xaa;

  // Set mode
  switch (this->mode) {
    case climate::CLIMATE_MODE_OFF:
      remote_state[2] = MHI_RYD502A_POWER_OFF;
    case climate::CLIMATE_MODE_HEAT_COOL:
      remote_state[2] |= MHI_RYD502A_MODE_HEAT_COOL << 1;
      break;
    case climate::CLIMATE_MODE_COOL:
      remote_state[2] |= MHI_RYD502A_MODE_COOL << 1;
      break;
    case climate::CLIMATE_MODE_HEAT:
      remote_state[2] |= MHI_RYD502A_MODE_HEAT << 1;
      break;
    case climate::CLIMATE_MODE_DRY:
      remote_state[2] |= MHI_RYD502A_MODE_DRY << 1;
      break;
    case climate::CLIMATE_MODE_FAN_ONLY:
      remote_state[2] |= MHI_RYD502A_MODE_FAN << 1;
      break;
    default:
      break;
  }

  // Set fan
  uint8_t selected_fan;
  switch (this->fan_mode.value_or(climate::CLIMATE_FAN_ON)) {
    case climate::CLIMATE_FAN_HIGH:
      selected_fan = MHI_RYD502A_FAN_L4;
      break;
    case climate::CLIMATE_FAN_MEDIUM:
      selected_fan = MHI_RYD502A_FAN_L3;
      break;
    case climate::CLIMATE_FAN_LOW:
      selected_fan = MHI_RYD502A_FAN_L2;
      break;
    case climate::CLIMATE_FAN_OFF:
      selected_fan = MHI_RYD502A_FAN_L1;
      break;
    case climate::CLIMATE_FAN_AUTO:
    default:
      selected_fan = MHI_RYD502A_FAN_AUTO;
  }
  remote_state[2] |= selected_fan << 4;

  // Set temperature
  uint8_t temp = (uint8_t) roundf(
        clamp<float>(this->target_temperature, MHI_RYD502A_TEMP_MIN, MHI_RYD502A_TEMP_MAX) - MHI_RYD502A_TEMP_MIN + 2);

  remote_state[3] = uint8_t(temp & 0x0F) << 2;

  // Set swing
  switch (this->swing_mode) {
    case climate::CLIMATE_SWING_BOTH:
      remote_state[8] = remote_state[8] | MHI_RYD502A_SWING_VDIR << 3;  // Vertical Swing
      remote_state[9] = remote_state[9] | MHI_RYD502A_SWING_HDIR << 1;  // Horizontal Swing
      break;
    case climate::CLIMATE_SWING_HORIZONTAL:
      remote_state[8] = remote_state[8] | this->default_vertical_direction_ << 3;
      remote_state[9] = remote_state[9] | MHI_RYD502A_SWING_HDIR << 1;
      break;
    case climate::CLIMATE_SWING_VERTICAL:
      remote_state[8] = remote_state[8] | MHI_RYD502A_SWING_VDIR << 3;

      if (this->default_horizontal_direction_ > 7) {
        // For split and focus
        remote_state[12] = remote_state[12] | this->default_horizontal_direction_;
        remote_state[9] = remote_state[9] | HORIZONTAL_DIRECTION_RIGHT << 1;
      } else {
        remote_state[9] = remote_state[9] | this->default_horizontal_direction_ << 1;
      }
      break;
    case climate::CLIMATE_SWING_OFF:
    default:
      // Off--> default position
      remote_state[8] = remote_state[8] | this->default_vertical_direction_ << 3;

      if (this->default_horizontal_direction_ > 7) {
        // For split and focus
        remote_state[12] = remote_state[12] | this->default_horizontal_direction_;
        remote_state[9] = remote_state[9] | HORIZONTAL_DIRECTION_RIGHT << 1;
      } else {
        remote_state[9] = remote_state[9] | this->default_horizontal_direction_ << 1;
      }
      break;
  }

  // Checksum
  for (int i = 0; i < 13; i++) {
    remote_state[13] += remote_state[i];
  }

  ESP_LOGD(TAG, "Sending: %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X   %02X %02X %02X %02X", remote_state[0],
           remote_state[1], remote_state[2], remote_state[3], remote_state[4], remote_state[5], remote_state[6],
           remote_state[7], remote_state[8], remote_state[9], remote_state[10], remote_state[11], remote_state[12],
           remote_state[13], remote_state[14], remote_state[15]);

  auto transmit = this->transmitter_->transmit();
  auto *data = transmit.get_data();

  data->set_carrier_frequency(38000);

  // Header
  data->mark(MHI_RYD502A_HDR_MARK);
  data->space(MHI_RYD502A_HDR_SPACE);
  // Data
  for (uint8_t i : remote_state) {
    for (uint8_t j = 0; j < 8; j++) {
      data->mark(MHI_RYD502A_BIT_MARK);
      bool bit = i & (1 << j);
      data->space(bit ? MHI_RYD502A_ONE_SPACE : MHI_RYD502A_ZERO_SPACE);
    }
  }
  data->mark(MHI_RYD502A_BIT_MARK);


  transmit.perform();
}

} // namespace esphome::mhi_ryd502a

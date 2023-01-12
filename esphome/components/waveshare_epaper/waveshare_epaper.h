#pragma once

#include "esphome/core/component.h"
#include "esphome/components/spi/spi.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace waveshare_epaper {

class WaveshareEPaper : public PollingComponent,
                        public display::DisplayBuffer,
                        public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST, spi::CLOCK_POLARITY_LOW,
                                              spi::CLOCK_PHASE_LEADING, spi::DATA_RATE_2MHZ> {
 public:
  void set_dc_pin(GPIOPin *dc_pin) { dc_pin_ = dc_pin; }
  float get_setup_priority() const override;
  void set_reset_pin(GPIOPin *reset) { this->reset_pin_ = reset; }
  void set_busy_pin(GPIOPin *busy) { this->busy_pin_ = busy; }
  void set_reset_duration(uint32_t reset_duration) { this->reset_duration_ = reset_duration; }

  void command(uint8_t value);
  void data(uint8_t value);

  virtual void display() = 0;
  virtual void initialize() = 0;
  virtual void deep_sleep() = 0;

  void update() override;

  void fill(Color color) override;

  void setup() override {
    this->setup_pins_();
    this->initialize();
  }

  void on_safe_shutdown() override;

  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  bool wait_until_idle_();

  void setup_pins_();

  void reset_() {
    if (this->reset_pin_ != nullptr) {
      this->reset_pin_->digital_write(false);
      delay(reset_duration_);  // NOLINT
      this->reset_pin_->digital_write(true);
      delay(200);  // NOLINT
    }
  }

  virtual int get_width_controller() { return this->get_width_internal(); };

  uint32_t get_buffer_length_();
  uint32_t reset_duration_{200};

  void start_command_();
  void end_command_();
  void start_data_();
  void end_data_();

  GPIOPin *reset_pin_{nullptr};
  GPIOPin *dc_pin_;
  GPIOPin *busy_pin_{nullptr};
  virtual uint32_t idle_timeout_() { return 1000u; }  // NOLINT(readability-identifier-naming)
};

enum WaveshareEPaperTypeAModel {
  WAVESHARE_EPAPER_1_54_IN = 0,
  WAVESHARE_EPAPER_1_54_IN_V2,
  WAVESHARE_EPAPER_2_13_IN,
  WAVESHARE_EPAPER_2_9_IN,
  WAVESHARE_EPAPER_2_9_IN_V2,
  TTGO_EPAPER_2_13_IN,
  TTGO_EPAPER_2_13_IN_B73,
  TTGO_EPAPER_2_13_IN_B1,
  TTGO_EPAPER_2_13_IN_B74,
};

class WaveshareEPaperTypeA : public WaveshareEPaper {
 public:
  WaveshareEPaperTypeA(WaveshareEPaperTypeAModel model);

  void initialize() override;

  void dump_config() override;

  void display() override;

  void deep_sleep() override {
    if (this->model_ == WAVESHARE_EPAPER_2_9_IN_V2 || this->model_ == WAVESHARE_EPAPER_1_54_IN_V2) {
      // COMMAND DEEP SLEEP MODE
      this->command(0x10);
      this->data(0x01);
    } else {
      // COMMAND DEEP SLEEP MODE
      this->command(0x10);
    }
    this->wait_until_idle_();
  }

  void set_full_update_every(uint32_t full_update_every);

 protected:
  void write_lut_(const uint8_t *lut, uint8_t size);

  int get_width_internal() override;

  int get_height_internal() override;

  int get_width_controller() override;

  uint32_t full_update_every_{30};
  uint32_t at_update_{0};
  WaveshareEPaperTypeAModel model_;
  uint32_t idle_timeout_() override;
};

enum WaveshareEPaperTypeBModel {
  WAVESHARE_EPAPER_2_7_IN = 0,
  WAVESHARE_EPAPER_4_2_IN,
  WAVESHARE_EPAPER_4_2_IN_B_V2,
  WAVESHARE_EPAPER_7_5_IN,
  WAVESHARE_EPAPER_7_5_INV2,
  WAVESHARE_EPAPER_7_5_IN_B_V2,
};

class WaveshareEPaper2P66In : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND DEEP SLEEP
    this->command(0x10);
    this->data(0x01);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper2P7In : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class GDEY029T94 : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class GDEW0154M09 : public WaveshareEPaper {
 public:
  void initialize() override;
  void display() override;
  void dump_config() override;
  void deep_sleep() override;

 protected:
  int get_width_internal() override;
  int get_height_internal() override;

 private:
  static const uint8_t CMD_DTM1_DATA_START_TRANS = 0x10;
  static const uint8_t CMD_DTM2_DATA_START_TRANS2 = 0x13;
  static const uint8_t CMD_DISPLAY_REFRESH = 0x12;
  static const uint8_t CMD_AUTO_SEQ = 0x17;
  static const uint8_t DATA_AUTO_PON_DSR_POF_DSLP = 0xA7;
  static const uint8_t CMD_PSR_PANEL_SETTING = 0x00;
  static const uint8_t CMD_UNDOCUMENTED_0x4D = 0x4D;  //  NOLINT
  static const uint8_t CMD_UNDOCUMENTED_0xAA = 0xaa;  //  NOLINT
  static const uint8_t CMD_UNDOCUMENTED_0xE9 = 0xe9;  //  NOLINT
  static const uint8_t CMD_UNDOCUMENTED_0xB6 = 0xb6;  //  NOLINT
  static const uint8_t CMD_UNDOCUMENTED_0xF3 = 0xf3;  //  NOLINT
  static const uint8_t CMD_TRES_RESOLUTION_SETTING = 0x61;
  static const uint8_t CMD_TCON_TCONSETTING = 0x60;
  static const uint8_t CMD_CDI_VCOM_DATA_INTERVAL = 0x50;
  static const uint8_t CMD_POF_POWER_OFF = 0x02;
  static const uint8_t CMD_DSLP_DEEP_SLEEP = 0x07;
  static const uint8_t DATA_DSLP_DEEP_SLEEP = 0xA5;
  static const uint8_t CMD_PWS_POWER_SAVING = 0xe3;
  static const uint8_t CMD_PON_POWER_ON = 0x04;
  static const uint8_t CMD_PTL_PARTIAL_WINDOW = 0x90;

  uint8_t *lastbuff_ = nullptr;
  void reset_();
  void clear_();
  void write_init_list_(const uint8_t *list);
  void init_internal_();
};

class WaveshareEPaper2P9InB : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper4P2In : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND VCOM AND DATA INTERVAL SETTING
    this->command(0x50);
    this->data(0x17);  // border floating

    // COMMAND VCM DC SETTING
    this->command(0x82);
    // COMMAND PANEL SETTING
    this->command(0x00);

    delay(100);  // NOLINT

    // COMMAND POWER SETTING
    this->command(0x01);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    delay(100);  // NOLINT

    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper4P2InBV2 : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND VCOM AND DATA INTERVAL SETTING
    this->command(0x50);
    this->data(0xF7);  // border floating

    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();

    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check code
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper5P8In : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper5P8InV2 : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND VCOM AND DATA INTERVAL SETTING
    this->command(0x50);
    this->data(0x17);  // border floating

    // COMMAND VCM DC SETTING
    this->command(0x82);
    // COMMAND PANEL SETTING
    this->command(0x00);

    delay(100);  // NOLINT

    // COMMAND POWER SETTING
    this->command(0x01);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    this->data(0x00);
    delay(100);  // NOLINT

    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper7P5In : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper7P5InBV2 : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);  // deep sleep
    this->data(0xA5);     // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper7P5InBV3 : public WaveshareEPaper {
 public:
  bool wait_until_idle_();

  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    this->command(0x02);  // Power off
    this->wait_until_idle_();
    this->command(0x07);  // Deep sleep
    this->data(0xA5);
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;

  void reset_() {
    if (this->reset_pin_ != nullptr) {
      this->reset_pin_->digital_write(true);
      delay(200);  // NOLINT
      this->reset_pin_->digital_write(false);
      delay(5);
      this->reset_pin_->digital_write(true);
      delay(200);  // NOLINT
    }
  };
};

class WaveshareEPaper7P5InBC : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper7P5InV2 : public WaveshareEPaper {
 public:
  bool wait_until_idle_();

  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER OFF
    this->command(0x02);
    this->wait_until_idle_();
    // COMMAND DEEP SLEEP
    this->command(0x07);
    this->data(0xA5);  // check byte
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;

  uint32_t idle_timeout_() override;
};

class WaveshareEPaper7P5InV2alt : public WaveshareEPaper7P5InV2 {
 public:
  bool wait_until_idle_();
  void initialize() override;
  void dump_config() override;

 protected:
  void reset_() {
    if (this->reset_pin_ != nullptr) {
      this->reset_pin_->digital_write(true);
      delay(200);  // NOLINT
      this->reset_pin_->digital_write(false);
      delay(2);
      this->reset_pin_->digital_write(true);
      delay(20);
    }
  };
};

class WaveshareEPaper7P5InHDB : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // deep sleep
    this->command(0x10);
    this->data(0x01);
  }

 protected:
  int get_width_internal() override;

  int get_height_internal() override;
};

class WaveshareEPaper2P13InDKE : public WaveshareEPaper {
 public:
  void initialize() override;

  void display() override;

  void dump_config() override;

  void deep_sleep() override {
    // COMMAND POWER DOWN
    this->command(0x10);
    this->data(0x01);
    // cannot wait until idle here, the device no longer responds
  }

  void set_full_update_every(uint32_t full_update_every);

 protected:
  int get_width_internal() override;

  int get_height_internal() override;

  uint32_t idle_timeout_() override;

  uint32_t full_update_every_{30};
  uint32_t at_update_{0};
};

}  // namespace waveshare_epaper
}  // namespace esphome

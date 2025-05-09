/*
  Based on: https://github.com/wmbusmeters/wmbusmeters/blob/master/src/driver_apatoreitn.cc
  Copyright (C) 2019-2022 Fredrik Öhrström (gpl-3.0-or-later)
*/

#pragma once

#include "driver.h"

#include <vector>
#include <string>

struct ApatorEITN: Driver
{
  ApatorEITN(std::string key = "") : Driver(std::string("apatoreitn"), key) {};
  virtual esphome::optional<std::map<std::string, double>> get_values(std::vector<unsigned char> &telegram) override {
    std::map<std::string, double> ret_val{};

    add_to_map(ret_val, "current_hca", this->get_current_hca(telegram));
    add_to_map(ret_val, "previous_hca", this->get_previous_hca(telegram));
    add_to_map(ret_val, "current_m_hca", this->get_current_m_hca(telegram));
    add_to_map(ret_val, "previous_m_hca", this->get_previous_m_hca(telegram));
    add_to_map(ret_val, "temp_room_avg_c", this->get_temp_room_avg_c(telegram));

    if (ret_val.size() > 0) {
      return ret_val;
    }
    else {
      return {};
    }
  };

private:
  esphome::optional<double> get_current_hca(std::vector<unsigned char> &telegram) {
    esphome::optional<double> ret_val{};
    size_t i = 10;

    if (telegram[i] == 0xB6) {
      i += telegram[i+1] + 2;
    }

    // experimental Apator E-ITN 40
    if ((telegram[8] == 0x09) && (telegram.size() > 52)) {
      i = 43;
    }

    ret_val = (((uint32_t)telegram[i+9] << 8) + (uint32_t)telegram[i+8]);
    ESP_LOGVV(TAG, "Found current_hca with '%f'", ret_val.value());

    return ret_val;
  };

  esphome::optional<double> get_previous_hca(std::vector<unsigned char> &telegram) {
    esphome::optional<double> ret_val{};
    size_t i = 10;
    if (telegram[i] == 0xB6) {
      i += telegram[i+1] + 2;
    }

    // experimental Apator E-ITN 40
    if ((telegram[8] == 0x09) && (telegram.size() > 52)) {
      i = 49;
    }

    ret_val = (((uint32_t)telegram[i+5] << 8) + (uint32_t)telegram[i+4]);
    ESP_LOGVV(TAG, "Found previous_hca with '%f'", ret_val.value());

    return ret_val;
  };

  esphome::optional<double> get_current_m_hca(std::vector<unsigned char> &telegram) {
    esphome::optional<double> ret_val{};

    // experimental Apator E-ITN 40
    if ((telegram[8] == 0x09) && (telegram.size() > 52)) {
      ret_val = (((uint32_t)telegram[20] << 8) + (uint32_t)telegram[19]);
    }
    ESP_LOGVV(TAG, "Found currnet_m_hca with '%f'", ret_val.value());

    return ret_val;
  };

  esphome::optional<double> get_previous_m_hca(std::vector<unsigned char> &telegram) {
    esphome::optional<double> ret_val{};

    // experimental Apator E-ITN 40
    if ((telegram.size() > 52)) {
      ret_val = (((uint32_t)telegram[10] << 8) + (uint32_t)telegram[9]);
    }
    ESP_LOGVV(TAG, "Found previous_m_hca with '%f'", ret_val.value());

    return ret_val;
  };

  esphome::optional<double> get_temp_room_avg_c(std::vector<unsigned char> &telegram) {
    esphome::optional<double> ret_val{};
    size_t i = 10;
    if (telegram[i] == 0xB6) {
      i += telegram[i+1] + 2;
    }

    ret_val = (((uint32_t)telegram[i+15]) + ((uint32_t)telegram[i+14])/256.0);
    ESP_LOGVV(TAG, "Found temp_room_avg with '%f'", ret_val.value());

    return ret_val;
  };
};

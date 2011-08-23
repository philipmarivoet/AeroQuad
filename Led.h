/*
  AeroQuad v2.5 Beta 1 - July 2011
 www.AeroQuad.com
 Copyright (c) 2011 Ted Carancho.  All rights reserved.
 An Open Source Arduino based multicopter.
 
 This program is free software: you can redistribute it and/or modify 
 it under the terms of the GNU General Public License as published by 
 the Free Software Foundation, either version 3 of the License, or 
 (at your option) any later version. 
 
 This program is distributed in the hope that it will be useful, 
 but WITHOUT ANY WARRANTY; without even the implied warranty of 
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the 
 GNU General Public License for more details. 
 
 You should have received a copy of the GNU General Public License 
 along with this program. If not, see <http://www.gnu.org/licenses/>. 
 */

// ***********************************************************************
// ************************ PatternMachine *******************************
// ***********************************************************************

byte led_pattern_1[] = {
  B00010010,
  B00000000,
};

byte led_pattern_2[] = {
  B00000001,
  B00000100,
  B00100000,
  B00001000,
};

byte led_pattern_3[] = {
  B01000000,
  B01000000,
  B01000000,
  B00000000,
  B01000000,
  B00000000,
  B01000000,
  B00000000,
  B00000000,
  B00000000
};

#define PATTERN_STATE_VALID    (1 << 0)
#define PATTERN_STATE_ACTIVE   (1 << 1)
#define PATTERN_STATE_RUNNING  (1 << 2)

typedef struct  LedPattern {
  // creation info
  int step_period_;
  int step_max_;
  byte* pattern_;

  // run-time state
  boolean state_;
  unsigned long timeout_;
  int step_current_;
};

#define PATTERN_MAX  4

class LedMachine {

public: 
  struct LedPattern pattern_[PATTERN_MAX];

  int pin_base_;
  int pin_count_;
  byte current_mask_;

  LedMachine(void) { 
  }

  void initialize() {
    pin_base_ = 43;
    pin_count_ = 7;
    current_mask_ = 0xFF;

    // all patterns off
    for (int index = 0; index < PATTERN_MAX; index++) {
      pattern_[index].state_ = 0;
    }

    // setup pins
    for (int index = 0; index < pin_count_; index++) {
      pinMode(pin_base_ + index, OUTPUT); // LED output
    }

    // add patterns
    set_pattern(0, 500, 2, led_pattern_1);
    set_pattern(1, 100, 4, led_pattern_2);
    set_pattern(2, 300, 10, led_pattern_3);

    // all off
    update(0x00);
  }

  void set_pattern(int index, int step_period, int step_max, byte* pattern_bytes) {
    LedPattern* pattern = &pattern_[index];
    
    pattern->step_period_ = step_period;
    pattern->step_max_ = step_max - 1;
    pattern->pattern_ = pattern_bytes;
    pattern->state_ = PATTERN_STATE_VALID | PATTERN_STATE_ACTIVE;
  }

  void enable_pattern(int index) {
    if (index < PATTERN_MAX) {
      LedPattern* pattern = &pattern_[index];
      
      if ((pattern->state_ & PATTERN_STATE_VALID) && (0 == (pattern->state_ & PATTERN_STATE_ACTIVE))) {
        // restart
        pattern->state_ = PATTERN_STATE_VALID | PATTERN_STATE_ACTIVE;
      }
    }
  }
  
  void disable_pattern(int index) {
    if (index < PATTERN_MAX) {
      LedPattern* pattern = &pattern_[index];
      pattern->state_ &= ~PATTERN_STATE_ACTIVE;
    }
  }

  void update(byte mask) {
    byte update_mask = current_mask_ ^ mask;
    
    for (int index = 0; index < pin_count_; index++) {
      byte bit_mask = 1 << index;
      if (0 != (update_mask & bit_mask)) {
        if (mask & bit_mask) {
          digitalWrite(pin_base_ + index, 1);
        } 
        else {
          digitalWrite(pin_base_ + index, 0);
        }
      }
    }
    
    current_mask_ = mask;
  } 

  void run(void) {
    byte mask;

    unsigned long now = millis();

    mask = 0;
    for (int index = 0; index < PATTERN_MAX; index++) {
      LedPattern* pattern = &pattern_[index];

      if (0 == (pattern->state_ & PATTERN_STATE_ACTIVE)) {
        // skip
        continue;
      }

      if (0 == (pattern->state_ & PATTERN_STATE_RUNNING)) {
        // first time
        pattern->step_current_ = 0;
        pattern->timeout_ = now + pattern->step_period_;
        pattern->state_ |= PATTERN_STATE_RUNNING;
      } 
      else if (pattern->timeout_ <= now) {
        pattern->step_current_++;
        if (pattern->step_current_ > pattern->step_max_) {
          pattern->step_current_ = 0;
        }
        pattern->timeout_ = now + pattern->step_period_;
      }

      mask |= pattern->pattern_[pattern->step_current_];

    }

    update(mask);
  }
};





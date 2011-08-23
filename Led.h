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

// Written by Philip Marivoet

// ***********************************************************************
// *************************** LedMachine ********************************
// ***********************************************************************


#define PATTERN_FLAG_VALID    (1 << 0)
#define PATTERN_FLAG_ACTIVE   (1 << 1)
#define PATTERN_FLAG_RUNNING  (1 << 2)

#define PATTERN_MAX  4

typedef struct  LedPattern {
  // creation info
  int step_period_;
  int step_max_;
  byte* pattern_;

  // run-time state
  boolean flags_;
  unsigned long timeout_;
  int step_current_;
};

class LedMachine {
public: 
  LedMachine(void);
  void initialize(int pin_base, int pin_count);
  void set_pattern(int index, int step_period, int step_max, byte* pattern_bytes);
  void enable_pattern(int index, boolean enabled);
  void run(void);

private:
  void update(byte mask);

private:
  struct LedPattern pattern_[PATTERN_MAX];

  int pin_base_;
  int pin_count_;
  byte current_mask_;
};

//
// implemenatation
//

LedMachine::LedMachine(void) { 
}

void
LedMachine::initialize(int pin_base, int pin_count) {
  pin_base_ = pin_base;
  pin_count_ = pin_count;

  // all patterns off
  for (int index = 0; index < PATTERN_MAX; index++) {
    pattern_[index].flags_ = 0;
  }

  // setup pins for output
  for (int index = 0; index < pin_count_; index++) {
    pinMode(pin_base_ + index, OUTPUT); // LED output
  }

  // all off
  current_mask_ = 0xFF;
  update(0x00);
}


void
LedMachine::set_pattern(int index, int step_period, int step_max, byte* pattern_bytes) {
  if (index < PATTERN_MAX) {
    LedPattern* pattern = &pattern_[index];

    pattern->step_period_ = step_period;
    pattern->step_max_ = step_max - 1;
    pattern->pattern_ = pattern_bytes;
    pattern->flags_ = PATTERN_FLAG_VALID | PATTERN_FLAG_ACTIVE;
  }
}

void
LedMachine::enable_pattern(int index, boolean enabled) {
  if (index < PATTERN_MAX) {
    LedPattern* pattern = &pattern_[index];

    if (pattern->flags_ & PATTERN_FLAG_VALID)
    {
      if (enabled) {
        if (0 == (pattern->flags_ & PATTERN_FLAG_ACTIVE)) {
          // restart
          pattern->flags_ = PATTERN_FLAG_VALID | PATTERN_FLAG_ACTIVE;
        }
      } 
      else {
        pattern->flags_ &= ~PATTERN_FLAG_ACTIVE;
      }
    }
  }
}

void
LedMachine::run(void) {
  byte mask;

  unsigned long now = millis();

  mask = 0;
  for (int index = 0; index < PATTERN_MAX; index++) {
    LedPattern* pattern = &pattern_[index];

    if (0 == (pattern->flags_ & PATTERN_FLAG_ACTIVE)) {
      // skip
      continue;
    }

    if (0 == (pattern->flags_ & PATTERN_FLAG_RUNNING)) {
      // first time
      pattern->step_current_ = 0;
      pattern->timeout_ = now + pattern->step_period_;
      pattern->flags_ |= PATTERN_FLAG_RUNNING;
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

void
LedMachine::update(byte mask) {
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



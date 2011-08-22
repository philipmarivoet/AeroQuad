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
// ******************** Yet Another Led Library **************************
// ***********************************************************************

// period | mask | size | 1 ... n

byte led_schedule_1[] = {
  B00000001,
  B00000010,
  B00000100,
  B00001000
};

byte led_schedule_2[] = {
  B00100000,
  B00110000,
  B00010000,
  B00000000,
};

byte led_schedule_3[] = {
  B00000001,
  B00000000
};

#define SCHEDULE_STATE_VALID    (1 << 0)
#define SCHEDULE_STATE_ACTIVE   (1 << 1)
#define SCHEDULE_STATE_RUNNING  (1 << 2)

typedef struct  LedSchedule {
  // creation info
  int step_period_;
  int step_max_;
  byte* schedule_;

  // run-time state
  boolean state_;
  unsigned long timeout_;
  int step_current_;
};

#define SCHEDULE_MAX  4

class YALL {

public: 
  struct LedSchedule schedule_[SCHEDULE_MAX];

  int pin_base_;
  int pin_count_;
  byte current_mask_;

  YALL(void) { 
  }

  // **********************************************************************
  // The following function calls must be defined inside any new subclasses
  // **********************************************************************
  void initialize() {
    pin_base_ = 43;
    pin_count_ = 7;
    current_mask_ = 0xFF;

    // all schedules off
    for (int index = 0; index < SCHEDULE_MAX; index++) {
      schedule_[index].state_ = 0;
    }

    // setup pins
    for (int index = 0; index < pin_count_; index++) {
      pinMode(pin_base_ + index, OUTPUT); // LED output
    }

    // add schedules
    set_schedule(0, 100, 4, led_schedule_1);
    set_schedule(1, 1000, 4, led_schedule_2);
//    set_schedule(0, 1000, 2, led_schedule_3);

    // all off
    update(0x00);
  }

  void set_schedule(int index, int step_period, int step_max, byte* schedule_bytes) {
    LedSchedule* schedule = &schedule_[index];
    
    schedule->step_period_ = step_period;
    schedule->step_max_ = step_max - 1;
    schedule->schedule_ = schedule_bytes;
    schedule->state_ = SCHEDULE_STATE_VALID | SCHEDULE_STATE_ACTIVE;
  }

  void update(byte mask) {
    for (int index = 0; index < pin_count_; index++) {
      if (mask & (1 << index)) {
        digitalWrite(pin_base_ + index, 1);
      } 
      else {
        digitalWrite(pin_base_ + index, 0);
      }
    }
  } 

  void run(void) {
    byte mask;

    unsigned long now = millis();

    mask = 0;
    for (int index = 0; index < SCHEDULE_MAX; index++) {
      LedSchedule* schedule = &schedule_[index];

      if (0 == (schedule->state_ & SCHEDULE_STATE_ACTIVE)) {
        // next please
        continue;
      }

      if (0 == (schedule->state_ & SCHEDULE_STATE_RUNNING)) {
        // first time
        schedule->step_current_ = 0;
        schedule->timeout_ = now + schedule->step_period_;
        schedule->state_ |= SCHEDULE_STATE_RUNNING;
      } 
      else if (schedule->timeout_ <= now) {
        schedule->step_current_++;
        if (schedule->step_current_ > schedule->step_max_) {
          schedule->step_current_ = 0;
        }
        schedule->timeout_ = now + schedule->step_period_;
      }

      mask |= schedule->schedule_[schedule->step_current_];

    }

    update(mask);
  }
};





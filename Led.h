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

byte ledSchedule[] = {8, 1, 2, 4, 8, 16, 32, 64, 128};

class YALL {
public: 
  int pin_base;
  int pin_count;
  int max_step;
  int current_step;
  byte current_mask;
  int step_delay;
  int step_count;
  
  YALL(void) { 
    // this is the constructor of the object and must have the same name 
    // can be used to initialize any of the variables declared above 
  }

  // **********************************************************************
  // The following function calls must be defined inside any new subclasses
  // **********************************************************************
  void initialize(void) {
    pin_base = 43;
    pin_count = 7;
    
    for (int index = 0; index < pin_count; index++) {
      pinMode(pin_base + index, OUTPUT); // LED output
    }

    max_step = ledSchedule[0];
    current_step = 1;
    current_mask = 0;
    
    step_delay = 10;
    step_count = step_delay;
    
    // all on
    update(0xFF);
  }

  void update(byte mask) {
    for (int index = 0; index < pin_count; index++) {
      if (mask & (1 << index)) {
        digitalWrite(pin_base + index, 1);
      } else {
        digitalWrite(pin_base + index, 0);
      }
    }
  } 
  
  void run(void) {
    byte mask;
    
    mask = ledSchedule[current_step];
    update(mask);
    
    step_count--;
   
    if (step_count <= 0) {
      current_step++;
      step_count = step_delay;

      if (current_step >= max_step) {
        current_step = 1;
      }
    }
  }
};



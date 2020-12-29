/**
  This file is part of the CJMCU-111 rotary button demo distribution
  (https://github.com/ChrisPHL/CJMCU-111-rotary-button-demo).
  Copyright (c) 2020 Christian Pohl, aka ChrisPHL, www.chpohl.de.

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, version 3.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

//#define CJMCU_111_DEBUG 1


byte rotaryPinGa = 2;
byte rotaryPinGb = 3;

#ifdef CJMCU_111_DEBUG
byte rotaryIsrDebugPinA = 4;
byte rotaryIsrDebugPinB = 5;
#endif

volatile enum rotationInputStatus {
  kRotationStatusWaiting, // Ga: HIGH and Gb: HIGH
  kRotationStatusStartLorPush, // Ga: FALLING, Gb: HIGH
  kRotationStatusStartR, // Ga: HIGH, Gb: FALLING
  kRotationStatusTurningL, // Ga: RISING, Gb: LOW
  kRotationStatusTurningR, // Ga: LOW, Gb: RISING
  kRotationStatusWaitForFinishL, // Ga: RISING, Gb: LOW
  kRotationStatusWaitForFinishR, // Ga: LOW, Gb RISING
} rotationStatus;

volatile bool rotaryInputDetected = false;
volatile byte rotationCountL = 0;
volatile byte rotationCountR = 0;
volatile bool rotaryBtnPushed = false;
volatile byte rotaryPushCount = 0;

const byte isrGxDebounceDuration = 2; // msec

volatile unsigned long isrGxTime = 0;
volatile unsigned long isrGxLastTime = 0;


void setup() {
  rotationStatus = kRotationStatusWaiting;

  Serial.begin(115200);
  pinMode(rotaryPinGa, INPUT_PULLUP);
  pinMode(rotaryPinGb, INPUT_PULLUP);

#ifdef CJMCU_111_DEBUG
  pinMode(rotaryIsrDebugPinA, OUTPUT);
  digitalWrite(rotaryIsrDebugPinA, HIGH);
  pinMode(rotaryIsrDebugPinB, OUTPUT);
  digitalWrite(rotaryIsrDebugPinB, HIGH);
#endif

  Serial.println("Starting CJMCU-111 rotary button demo");

  attachInterrupt(digitalPinToInterrupt(rotaryPinGa), isrGx, CHANGE);
  attachInterrupt(digitalPinToInterrupt(rotaryPinGb), isrGx, CHANGE);
}

//===========

void loop() {
  if (rotaryInputDetected) {
    Serial.print("L: ");
    Serial.print(rotationCountL);
    Serial.print("; R: ");
    Serial.print(rotationCountR);
    Serial.print("; SW: ");
    Serial.print(rotaryPushCount);
    Serial.println();
    noInterrupts(); // necessary when reading a multi-byte variable from the ISR
    rotationCountL = 0;
    rotationCountR = 0;
    rotaryPushCount = 0;
    rotaryInputDetected = false;
    interrupts();
  }
}

void isrGx() {
  /*
     This routine relies on not being interrupted by another (higher prioritzed) interrupt.
     This should be the case for the Arduino hardware.
  */
#ifdef CJMCU_111_DEBUG
  digitalWrite(rotaryIsrDebugPinA, LOW);
#endif

  byte rotationInputA = digitalRead(rotaryPinGa);
  byte rotationInputB = digitalRead(rotaryPinGb);

  isrGxTime = millis();

  switch (rotationStatus) {
    case kRotationStatusWaiting:
      if (HIGH == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusStartR;
      }
      else if (LOW == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusStartLorPush;
        isrGxLastTime = isrGxTime; // debounce
      }
      break;

    case kRotationStatusStartLorPush:
      if (HIGH == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusWaiting;
        if ((isrGxTime - isrGxLastTime) > isrGxDebounceDuration) {
          ++rotaryPushCount;
          rotaryInputDetected = true;
        }
      }
      else if (LOW == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusTurningL;
      }
      break;

    case kRotationStatusStartR:
      if (HIGH == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusWaiting;
      }
      else if (LOW == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusTurningR;
      }
      break;

    case kRotationStatusTurningL:
      if (LOW == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusStartLorPush;
      }
      else if (HIGH == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusWaitForFinishL;
      }
      break;

    case kRotationStatusTurningR:
      if (HIGH == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusStartR;
      }
      else if (LOW == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusWaitForFinishR;
      }
      break;

    case kRotationStatusWaitForFinishL:
      if (LOW == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusTurningL;
      }
      else if (HIGH == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusWaiting;
        rotationCountR = 0;
        ++rotationCountL;
        rotaryInputDetected = true;
        isrGxLastTime = isrGxTime; // debounce
      }
      break;

    case kRotationStatusWaitForFinishR:
      if (LOW == rotationInputA && LOW == rotationInputB) {
        rotationStatus = kRotationStatusTurningR;
      }
      else if (HIGH == rotationInputA && HIGH == rotationInputB) {
        rotationStatus = kRotationStatusWaiting;
        rotationCountL = 0;
        ++rotationCountR;
        rotaryInputDetected = true;
        isrGxLastTime = isrGxTime;
      }
      break;
  }

#ifdef CJMCU_111_DEBUG
  digitalWrite(rotaryIsrDebugPinA, HIGH);
#endif
}

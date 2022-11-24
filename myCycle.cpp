#include "myCycle.h"
#include <Arduino.h>

myCycle::myCycle(unsigned long per, bool act) {
  _go     = false;
  _active = act;
  _period = per;
  _start  = millis();
}

// Methods

void myCycle::reInit(unsigned long per, bool act) {
  _go     = false;
  _active = act;
  _period = per;
  _start  = millis();
}

void myCycle::reStart() {
	  _start = millis();
		//msn_08/01/2022 добавил сюда очистку флага сработки таймера, что бы в коде не вызывать
		//отдельно еще и метод Clear
		_go = false;
}

bool myCycle::check() {
  if (millis() - _start >= _period) {
    _start = millis();
    if (_active) {
      _go = true;
    }
  }
  return _go;
}

bool myCycle::go() {
  return _go;
}

void myCycle::clear() {
  _go = false;
}

// Active

bool myCycle::active() {
  return _active;
}

void myCycle::setActive(bool act) {
  _active = act;
}

// Period

unsigned long myCycle::period() {
  return _period;
}

void myCycle::setPeriod(unsigned long per) {
  _period = per;
}
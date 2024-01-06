#include <lamp.h>

TheLamp::TheLamp(){
  current_mode = DEFAULT_MODE;
  lampPin = DEFAULT_LAMP_PIN;
  profile = DEFAULT_PROFILE;
  activate_hours = DEFAULT_ACTIVE_HOUR;
  milestones.push_back(activate_hours.first);
  float am = 0;
  for (auto m: profile){
    am += m;
    milestones.push_back(activate_hours.first + (activate_hours.second - activate_hours.first)*am);
  }
  modes = {{"Idle", -1},
            {"Cold", 0},
            {"Warm", 1},
            {"Full Light", 2}};
}

TheLamp::TheLamp(int _lampPin){
  current_mode = DEFAULT_MODE;
  lampPin = _lampPin;
  profile = DEFAULT_PROFILE;
  activate_hours = DEFAULT_ACTIVE_HOUR;
  milestones.push_back(activate_hours.first);
  float am = 0;
  for (auto m: profile){
    am += m;
    milestones.push_back(activate_hours.first + (activate_hours.second - activate_hours.first)*am);
  }
  modes = {{"Idle", -1},
            {"Cold", 0},
            {"Warm", 1},
            {"Full Light", 2}};
}


bool TheLamp::setup(){
  pinMode(lampPin, OUTPUT);
  digitalWrite(lampPin, LOW);
  return true;
}

bool TheLamp::set_modes(std::map<std::string, int> modes){
  modes = modes;
  return true;
}

bool TheLamp::set_lampPin(int Pin){
  lampPin = Pin;
  setup();
  return true;
}

bool TheLamp::daily_change_mode(int mode_index){
  if (current_mode == mode_index){
    return true; // auto keep auto, manual keep manual
  } else {
    auto _now = now();
    _now = _now % DAILY_SECONDS;
    if (_now > manual_disable_at){
      manual_disable_at = -1; // in the next day, auto by default
      change_mode(mode_index, true); // any mode to auto
    }
    return true;
  }
}

bool TheLamp::change_mode(std::string mode, bool isAuto){
  if (mode == "Retain"){
    return true;
  }
  if (!isAuto){
    auto _now = now();
    _now = _now % DAILY_SECONDS;
    Serial.print("[DEBUG] _now: ");
    Serial.println(_now);
    for (auto m: milestones){
      Serial.print("[DEBUG] milestones ->: ");
      Serial.println(m);
      if (_now < m){
        Serial.print("[DEBUG] manual_disable_at ->: ");
        Serial.println(m);
        manual_disable_at = m;
        break;
      }
    }
  }
  auto _m = modes.find(mode);
  if (_m == modes.end()){
    return false;
  }
  int mode_index =  _m->second;
  Serial.print("[DEBUG] mode_index: ");
  Serial.println(mode_index);
  Serial.print("[DEBUG] current_mode: ");
  Serial.println(current_mode);
  Serial.print("[DEBUG] lampPin: ");
  Serial.println(lampPin);

  if (mode_index == -1){
    idle();
    return true;
  }
  while (current_mode != mode_index){
    delay(100);
    light_down();
    delay(100);
    light_up();
    current_mode+=1;
    current_mode = current_mode % (modes.size()-1);
    Serial.print("[DEBUG] ---- current_mode: ");
    Serial.println(current_mode);
  }
  return true;
}

bool TheLamp::change_mode(int mode_index, bool isAuto){
if (!isAuto){
    auto _now = now();
    _now = _now % DAILY_SECONDS;
    Serial.print("[DEBUG] _now: ");
    Serial.println(_now);
    for (auto m: milestones){
      Serial.print("[DEBUG] milestones ->: ");
      Serial.println(m);
      if (_now < m){
        Serial.print("[DEBUG] manual_disable_at ->: ");
        Serial.println(m);
        manual_disable_at = m;
        break;
      }
    }
  }
  if (mode_index == -1){
    idle();
    return true;
  }
  while (current_mode != mode_index){
    delay(100);
    light_down();
    delay(100);
    light_up();
    current_mode+=1;
    current_mode = current_mode % modes.size();
  }
  return true;
}

bool TheLamp::idle(){
  light_down();
  current_mode = -1;
  return true;
}

bool TheLamp::light_up(){
  Serial.println("[DEBUG]: light up");
  digitalWrite(lampPin, HIGH);
  return true;
}

bool TheLamp::light_down(){
  Serial.println("[DEBUG]: light down");
  digitalWrite(lampPin, LOW);
  return true;
}

bool TheLamp::behave(){
  auto _now = now();
  _now = _now % DAILY_SECONDS;
  int _mode = -1;
  for (auto m: milestones){
    if (_now > m){
      _mode ++; // -1 for idle, 0 for Cold, 1 for Warm, 2 for Full Bright
    }
  }
  daily_change_mode(_mode);
  return true;
}
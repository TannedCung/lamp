#include <iostream>
#include <string>
#include <vector>
#include <bits/stdc++.h>
#include <Arduino.h>
#include <TimeLib.h>

#define DEFAULT_LAMP_PIN 2;
#define DEFAULT_ACTIVE_HOUR {(16 * 60 * 60), (24 * 60 * 60)}
#define DEFAULT_MODE -1; // Idle
#define DEFAULT_PROFILE {0.35, 0.3, 0.35} // Cold, Full Bright, Warm
#define DAILY_SECONDS (24 * 60 * 60)

class TheLamp{
  private: 
    std::vector<double> profile;
    std::vector<unsigned> milestones;
    std::map<std::string, int>modes;
    std::pair<unsigned long, unsigned long>activate_hours; // in seconds 
    TheLamp();
  
  public:
    int current_mode;
    int lampPin;
    time_t manual_disable_at = -1;
    TheLamp(int _lampPin);
    bool setup();
    bool set_modes(std::map<std::string, int> modes);
    bool set_lampPin(int Pin);
    bool change_mode(std::string mode, bool isAuto=false);
    bool change_mode(int mode, bool isAuto=false);
    bool daily_change_mode(int mode);
    bool idle();
    bool light_up();
    bool light_down();
    bool behave();
};
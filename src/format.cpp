#include <string>
#include <cmath>

#include "format.h"

using std::string;
using std::to_string;

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
string Format::ElapsedTime(long seconds) { 
  int hh, ss, mm;
  string str_ss, str_mm,str_hh, time;
  
  hh = seconds/3600;
  mm = (seconds%3600)/60;
  ss = (seconds%3600)%60;

  // Convert to strings
  str_ss = to_string(ss);
  str_mm = to_string(mm);
  str_hh = to_string(hh);
  
  // to output as 2 digit ints
  str_ss.insert(0, 2 - str_ss.length(), '0');
  str_mm.insert(0, 2 - str_mm.length(), '0');
  str_hh.insert(0, 2 - str_hh.length(), '0');

  time = str_hh + ":" + str_mm + ":" + str_ss;
  
  return time;
}
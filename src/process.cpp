#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

#include "process.h"

using std::string;
using std::to_string;
using std::vector;

// Return this process's ID
int Process::Pid() { return pid_; }


// Function modified to work with operator overloading. return type changed to const float
// Return this process's CPU utilization
float Process::CpuUtilization() const {
  long cpu_util{0}, total_time, seconds ;
  string line, utime, stime, cutime, cstime, value;
  
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid_) + LinuxParser::kStatFilename);
    if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      for (int i = 0; i < 20; ++i){
      linestream >> value;
        if(i==14){
          utime = value;
        }if (i==15){
          stime = value;
        }if(i==16){
          cutime = value;
        }if(i==17){
          cstime = value;
        }
      }
      // Calculations from StackOverflow Answer
      total_time = std::stol(utime) + std::stol(stime) + std::stol(cstime) + std::stol(cutime);
      seconds = LinuxParser::UpTime(pid_);
      
      if(seconds>0){
        cpu_util = (total_time / sysconf(_SC_CLK_TCK)) / seconds;
        return cpu_util;
      }else{
        return 0.0f;
      }
    }
  return cpu_util;
}

// Return the command that generated this process
string Process::Command() { 
  string command = LinuxParser::Command(pid_);
  if(command.length()>=25){   // displaying only the first 25 characters
  command = command.substr(0,25);
  command.append("...");
  return command;
  }
  return command;
}

// Return this process's memory utilization
string Process::Ram() { return LinuxParser::Ram(pid_); }

//  Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { return a.CpuUtilization() < this->CpuUtilization(); }
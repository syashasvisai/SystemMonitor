#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value("Linux"); // Default Value 
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

//Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float mem_total = LinuxParser::ParseMemInfo(filterMemTotalString);
  float mem_free = LinuxParser::ParseMemInfo(filterMemFreeString);
  float mem_used = mem_total - mem_free; 
  return mem_used/mem_total;
}

// Accessor function defined to access values of the processor count from meminfo file.

// QUESTION: Can we merge this and the ParseStat function somehow? function overloading with different return types and same parameters is not possible. Is there any other method? 

float LinuxParser::ParseMemInfo(std::string key){
  string line;
  string key_;
  float value;
  std::ifstream filestream(kProcDirectory+kMeminfoFilename); // streams from this file
  if (filestream.is_open()) { // Checks if the file is open
    while (std::getline(filestream, line)) { // Executes upto the last line
      std::istringstream linestream(line); // reads a string until whitespace is reached (<sstream> lib)
      while (linestream >> key_ >> value) { // extracts the keys and values. Here we need only the 1st and 2nd strings
        if (key_ == key) {
          return value;
        }
      }
    }
  }
  return value;
}

// Read and return the system uptime
long LinuxParser::UpTime() {
  long uptime_system;
  string line;
  
  std::ifstream stream(kProcDirectory + kUptimeFilename);
    if (stream.is_open()) {
      stream >> uptime_system;
      return uptime_system;
  }
  return uptime_system;
 
}


// Read and return the number of idle jiffies for the system


// TODO: Read and return utilization
// changed the return type to float based on an answer in the forum https://knowledge.udacity.com/questions/141752
// This function definition is implemented in process.cpp as there was a problem with operator overloading wrt to the return value. Hint taken from an answer in the forum.  https://knowledge.udacity.com/questions/96882

/* float LinuxParser::CpuUtilization(int pid) {
  long cpu_util, total_time, seconds ;
  string line, utime, stime, cutime, cstime, value;
  
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
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
      
      total_time = std::stol(utime) + std::stol(stime) + std::stol(cstime) + std::stol(cutime);
      seconds = LinuxParser::UpTime(pid);
  
      cpu_util = 100 * ((total_time / sysconf(_SC_CLK_TCK)) / seconds);
      
      return cpu_util;
  }
  return cpu_util;
}
*/
// Read and return the total number of processes
int LinuxParser::TotalProcesses() {return LinuxParser::ParseStat(kStatFilename, filterProcesses);}

//  Read and return the number of running processes
int LinuxParser::RunningProcesses() {return LinuxParser::ParseStat(kStatFilename, filterRunningProcesses);}

// Accessor function defined to access values from stat file.
int LinuxParser::ParseStat(std::string file_name, std::string key){
  string line;
  string key_;
  int value;
  std::ifstream filestream(kProcDirectory+file_name); // streams from this file
  if (filestream.is_open()) { // Checks if the file is open
    while (std::getline(filestream, line)) { // Executes upto the last line
      std::istringstream linestream(line); // reads a string until whitespace is reached (<sstream> lib)
      while (linestream >> key_ >> value) { // extracts the keys and values. Here we need only the 1st and 2nd strings
        if (key_ == key) {
          return value;
        }
      }
    }
  }
  return value;
}


// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string cmd_line;
  std::ifstream filestream(kProcDirectory+to_string(pid)+kCmdlineFilename); // streams from this file
  if (filestream.is_open()) { // Checks if the file is open
    std::getline(filestream, cmd_line);
    return cmd_line;
  }
  return cmd_line;
}

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) {
  int ram_ = LinuxParser::ParseStat(to_string(pid) + kStatusFilename, filterProcMem);
  return to_string(ram_/1000);
}

// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  int Uid_ = LinuxParser::ParseStat(to_string(pid) + kStatusFilename, filterUID);
  return to_string(Uid_);
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, key, value, x, user_name;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> x >> value; 
      if (value == uid) {
        user_name = key;
        return user_name;
      }
    }
  } 
  return user_name; }

// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  long uptime_process{0};
  string line, uptime;
  
  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
    if (filestream.is_open()) {
      std::getline(filestream, line);
      std::istringstream linestream(line);
      for (int i = 0; i<=21; ++i){
      linestream >> uptime;
      }
    }
      uptime_process = LinuxParser::UpTime() - std::stol(uptime)/sysconf(_SC_CLK_TCK);
      return uptime_process;
}


// Implemented in processor.cpp as Utilization()

/*float LinuxParser::ProcessorUtilization() {
  string line;
  string key;
  float value;
  std::ifstream filestream(kProcDirectory+kStatFilename); // streams from this file
  if (filestream.is_open()) { // Checks if the file is open
    while (std::getline(filestream, line)) { // Executes upto the last line
      std::istringstream linestream(line); // reads a string until whitespace is reached (<sstream> lib)
      while (linestream >> key >> value) { // extracts the keys and values. Here we need only the 1st and 2nd strings
        if (key == filterCpu) {
          return value; 
         }
      }
    }
  }
  return value;
}*/

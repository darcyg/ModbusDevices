#pragma once
#include <exception>
#include <sstream>
#include "LoggerWidget.h"

class md_exception : public std::runtime_error {
  std::string msg;
public:
  md_exception(const std::string &arg, const char *file, int line) :
    std::runtime_error(arg)
  {
      std::ostringstream o;
      o << "[" << file << ": " << line << "] " << arg;
      msg = o.str();
    }
  ~md_exception() throw() {}
  const char *what() const throw()
  {
    return msg.c_str();
  }
};

#define MD_THROW(msg, ...) throw md_exception(msg, __FILE__, __LINE__);  


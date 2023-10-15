#include "Exception.h"

// Initializing
Exception::Exception() = default;

// Initializing with a message
Exception::Exception(const std::string& error_msg) : _msg(error_msg) {};

// Initializing with a message
Exception::Exception(std::string&& error_msg) : _msg(std::move(error_msg)) {};

// Error output
const char* Exception::what() const noexcept { return _msg.c_str(); }

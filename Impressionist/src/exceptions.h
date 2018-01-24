#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

struct NotImplementedException : public std::exception {
    std::string message;
    NotImplementedException(std::string message) : message(message) {}
    ~NotImplementedException() throw () {}
    const char* what() const throw() { return message.c_str(); }
};

#endif // EXCEPTIONS_H

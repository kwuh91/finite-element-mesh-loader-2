#pragma once

#include <iostream>

class Exception : std::exception {
public:
    // Initializing
    explicit Exception();

    // Initializing with a message
    explicit Exception(const std::string&);

    // Initializing with a message
    explicit Exception(std::string&&);

    // Error output
    [[nodiscard]] const char* what() const noexcept override;

    // Destruct
    ~Exception() override = default;
private:
    std::string _msg;
};

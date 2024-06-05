#pragma once

#include <exception>
#include <string>

class KeyIsMissingInMap: std::exception {
public:
    explicit KeyIsMissingInMap(const std::string& text):
        error_message_(text){}

    const char* what() const noexcept override {
        return error_message_.data();
    }

private:
    std::string_view error_message_;
};

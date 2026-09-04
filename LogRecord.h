#pragma once 
#include<string>
#include<cstddef>

struct LogRecord{
    std::string method;
    std::string path;
    int statusCode;
    std::size_t bytes;
};


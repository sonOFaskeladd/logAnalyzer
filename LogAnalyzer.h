#pragma once
#include "LogRecord.h"
#include<unordered_map>
#include<string>
#include<cstddef>
#include<limits>

class LogAnalyzer{
    private:
        std::size_t totalLines = 0;
        std::size_t validRequests = 0;
        std::size_t malInformedRequests = 0;

        std::unordered_map<std::string, std::size_t> RequestByHTTP;
        std::unordered_map<std::string, std::size_t> RequestByEndpoint;
        std::unordered_map<std::string, std::size_t> StatusCodeCategory;
        std::unordered_map<int, std::size_t>statusCodes;

        std::size_t totalBytes = 0;
        std::size_t minBytes = std::numeric_limits<std::size_t>::max();
        std::size_t maxBytes = 0;

    public:
        void process(const LogRecord& record);
        void recordMalinformed();
        void printSummary();
};


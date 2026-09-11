#pragma once

#include "LogRecord.h"
#include<optional>
#include<string>

std::optional<LogRecord>logParser(const std::string& line);

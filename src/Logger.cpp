/*
 * Copyright 2022 Norbert Takacs
 *
 * SPDX-License-Identifier: GPL-3.0-or-later
 */
#include "Logger.h"

FILE* Logger::f_log = NULL;
TLogLevel Logger::current_log_level = TLogLevel::logERROR;
std::list<std::string> Logger::saved_errors_and_warnings;
std::mutex Logger::guard;
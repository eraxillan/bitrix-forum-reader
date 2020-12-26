/*
 * This file is part of Bitrix Forum Reader.
 *
 * Copyright (C) 2016-2020 Alexander Kamyshnikov <axill777@gmail.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
*/
#include <QtCore/QCoreApplication>

#define CATCH_CONFIG_RUNNER
#include "catch.hpp"

#include <common/logger.h>

namespace {

bool initLogLibrary() {
	try {
		std::cout << "initializing spdlog..." << std::endl;

		// Console multi threaded logger with color
#if defined(Q_OS_ANDROID)
		/*auto android_logger =*/spdlog::android_logger_mt("system", "Bitrix Forum Reader"); // "android"
#elif defined(Q_OS_UNIX)
		/*auto console =*/spdlog::stdout_color_mt("system");
#else
		auto sink = std::make_shared<spdlog::sinks::windebug_sink_st>();
		auto logger = std::make_shared<spdlog::logger>("system", sink);
		spdlog::register_logger(logger);
#endif

		// Customize msg format for all messages
		spdlog::set_pattern("[%^%L%$][%D %H:%M:%S.%e][%P:%t] %v");
		spdlog::set_level(spdlog::level::trace);

		SystemLogger->info("spdlog was successfully initialized");
	}
	// Exceptions will only be thrown upon failed logger or sink construction (not during logging)
	catch (const spdlog::spdlog_ex &ex) {
		std::cout << "spdlog init failed: " << ex.what() << std::endl;
		return false;
	}

	return true;
}

void deinitLogLibrary() {
	SystemLogger->info("deinitializing spdlog...");

	// Release and close all loggers
	spdlog::drop_all();

	std::cout << "spdlog deinit succeeded" << std::endl;
}

} // namespace

int main(int argc, char *argv[]) {
	if (!initLogLibrary())
		return 1;

	QCoreApplication app(argc, argv);
	//app.exec();

	int result = Catch::Session().run(argc, argv);

	deinitLogLibrary();
	return result;
}

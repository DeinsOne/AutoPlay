#include "AutoplayLog.h"

#include "spdlog/async.h"
#include "spdlog/sinks/basic_file_sink.h"
#include <spdlog/sinks/stdout_color_sinks.h>
#include "spdlog/pattern_formatter.h"

namespace APlay {

	std::shared_ptr<spdlog::logger> AutoplayLogger::_consoleLogger;
    std::shared_ptr<spdlog::logger> AutoplayLogger::_fileLogger;

    void AutoplayLogger::Init() {
		std::vector<spdlog::sink_ptr> logSinks;
		logSinks.emplace_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
		logSinks.emplace_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("tmp/autoplay.log", true));

		logSinks[0]->set_pattern("%^[%T] %n: %v%$");
		logSinks[1]->set_pattern("[%T] [%l] %n: %v");

		_consoleLogger = std::make_shared<spdlog::logger>("core", begin(logSinks), end(logSinks));
		spdlog::register_logger(_consoleLogger);
		_consoleLogger->set_level(spdlog::level::trace);
		_consoleLogger->flush_on(spdlog::level::trace);

		_fileLogger = std::make_shared<spdlog::logger>("log", begin(logSinks), end(logSinks));
		spdlog::register_logger(_fileLogger);
		_fileLogger->set_level(spdlog::level::trace);
		_fileLogger->flush_on(spdlog::level::trace);
    }

    void Instrumentor::WriteProfile(const ProfileResult& result) {
        std::stringstream json;

        json << std::setprecision(3) << std::fixed;
        json << ",\n\t\t{";
        json << "\"cat\":\"function\",";
        json << "\"dur\":" << (result.ElapsedTime.count()) << ',';
        json << "\"name\":\"" << result.Name << "\",";
        json << "\"ph\":\"X\",";
        json << "\"pid\":0,";
        json << "\"tid\":" << result.ThreadID << ",";
        json << "\"ts\":" << result.Start.count();
        json << "}";

        std::lock_guard<std::mutex> lock(m_Mutex);
        if (m_CurrentSession) {
            m_OutputStream << json.str();
            m_OutputStream.flush();
        }
    }

    void Instrumentor::WriteHeader() {
        m_OutputStream << "{\n\t\"otherData\": {},\n\t\"traceEvents\": [{}";
        m_OutputStream.flush();
    }

    void Instrumentor::WriteFooter() {
        m_OutputStream << "\n\t]\n}";
        m_OutputStream.flush();
    }

}


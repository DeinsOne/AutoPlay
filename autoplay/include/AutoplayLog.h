// Code based on analogous from Hazel game engine
// https://github/TheCherno/Hazel

#pragma once
#include "AutoplayCore.h"

#include "spdlog/spdlog.h"
#include <mutex>
#include <algorithm>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>

namespace APlay {

    class AutoplayLogger {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetConsoleLogger() { return _consoleLogger; }
        static std::shared_ptr<spdlog::logger>& GetFileLogger() { return _fileLogger; }
    private:
        static std::shared_ptr<spdlog::logger> _consoleLogger;
        static std::shared_ptr<spdlog::logger> _fileLogger;
    };

    using FloatingPointMicroseconds = std::chrono::duration<double, std::micro>;

    struct ProfileResult {
        std::string Name;

        FloatingPointMicroseconds Start;
        std::chrono::microseconds ElapsedTime;
        std::thread::id ThreadID;
    };

    struct InstrumentationSession {
        std::string Name;
    };

}

#define APLAY_CONSOLE_TRACE(...)    ::APlay::AutoplayLogger::GetConsoleLogger()->trace(__VA_ARGS__)
#define APLAY_CONSOLE_INFO(...)     ::APlay::AutoplayLogger::GetConsoleLogger()->info(__VA_ARGS__)
#define APLAY_CONSOLE_WARN(...)     ::APlay::AutoplayLogger::GetConsoleLogger()->warn(__VA_ARGS__)
#define APLAY_CONSOLE_ERROR(...)    ::APlay::AutoplayLogger::GetConsoleLogger()->error(__VA_ARGS__)
#define APLAY_CONSOLE_CRITICAL(...) ::APlay::AutoplayLogger::GetConsoleLogger()->critical(__VA_ARGS__)

// Client log macros
#define APLAY_TRACE(...)         ::APlay::AutoplayLogger::GetFileLogger()->trace(__VA_ARGS__)
#define APLAY_INFO(...)          ::APlay::AutoplayLogger::GetFileLogger()->info(__VA_ARGS__)
#define APLAY_WARN(...)          ::APlay::AutoplayLogger::GetFileLogger()->warn(__VA_ARGS__)
#define APLAY_ERROR(...)         ::APlay::AutoplayLogger::GetFileLogger()->error(__VA_ARGS__)
#define APLAY_CRITICAL(...)      ::APlay::AutoplayLogger::GetFileLogger()->critical(__VA_ARGS__)


namespace APlay {
    class Instrumentor {
    public:
        Instrumentor(const Instrumentor&) = delete;
        Instrumentor(Instrumentor&&) = delete;

        void BeginSession(const std::string& name, const std::string& filepath = "tmp/profile.json") {
            std::lock_guard<std::mutex> lock(m_Mutex);
            if (m_CurrentSession) {
                // If there is already a current session, then close it before beginning new one.
                // Subsequent profiling output meant for the original session will end up in the
                // newly opened session instead.  That's better than having badly formatted
                // profiling output.
                if (AutoplayLogger::GetConsoleLogger()) { // Edge case: BeginSession() might be before Log::Init()
                    APLAY_CONSOLE_ERROR("Instrumentor::BeginSession('{0}') when session '{1}' already open.", name, m_CurrentSession->Name);
                }
                InternalEndSession();
            }
            m_OutputStream.open(filepath);

            if (m_OutputStream.is_open()) {
                m_CurrentSession = new InstrumentationSession({name});
                WriteHeader();
            }
            else {
                if (AutoplayLogger::GetConsoleLogger()) { // Edge case: BeginSession() might be before Log::Init()
                    APLAY_CONSOLE_ERROR("Instrumentor could not open results file '{0}'.", filepath);
                }
            }
        }

        void EndSession() {
            std::lock_guard<std::mutex> lock(m_Mutex);
            InternalEndSession();
        }

        void WriteProfile(const ProfileResult& result);

        static Instrumentor& Get() {
            static Instrumentor instance;
            return instance;
        }
    private:
        Instrumentor()
            : m_CurrentSession(nullptr)
        {
        }

        ~Instrumentor() {
            EndSession();
        }        

        void WriteHeader();

        void WriteFooter();

        // Note: you must already own lock on m_Mutex before
        // calling InternalEndSession()
        void InternalEndSession() {
            if (m_CurrentSession) {
                WriteFooter();
                m_OutputStream.close();
                delete m_CurrentSession;
                m_CurrentSession = nullptr;
            }
        }

    private:
        std::mutex m_Mutex;
        InstrumentationSession* m_CurrentSession;
        std::ofstream m_OutputStream;
    };

    class InstrumentationTimer {
    public:
        InstrumentationTimer(const char* name)
            : m_Name(name), m_Stopped(false)
        {
            m_StartTimepoint = std::chrono::steady_clock::now();
        }

        ~InstrumentationTimer() {
            if (!m_Stopped)
                Stop();
        }

        void Stop() {
            auto endTimepoint = std::chrono::steady_clock::now();
            auto highResStart = FloatingPointMicroseconds{ m_StartTimepoint.time_since_epoch() };
            auto elapsedTime = std::chrono::time_point_cast<std::chrono::microseconds>(endTimepoint).time_since_epoch() - std::chrono::time_point_cast<std::chrono::microseconds>(m_StartTimepoint).time_since_epoch();

            Instrumentor::Get().WriteProfile({ m_Name, highResStart, elapsedTime, std::this_thread::get_id() });

            m_Stopped = true;
        }

    private:
        const char* m_Name;
        std::chrono::time_point<std::chrono::steady_clock> m_StartTimepoint;
        bool m_Stopped;
    };


    namespace InstrumentorUtils {

        template <size_t N>
        struct ChangeResult {
            char Data[N];
        };

        template <size_t N, size_t K>
        constexpr auto CleanupOutputString(const char(&expr)[N], const char(&remove)[K]) {
            ChangeResult<N> result = {};

            size_t srcIndex = 0;
            size_t dstIndex = 0;
            while (srcIndex < N) {
                size_t matchIndex = 0;
                while (matchIndex < K - 1 && srcIndex + matchIndex < N - 1 && expr[srcIndex + matchIndex] == remove[matchIndex])
                    matchIndex++;
                if (matchIndex == K - 1)
                    srcIndex += matchIndex;
                result.Data[dstIndex++] = expr[srcIndex] == '"' ? '\'' : expr[srcIndex];
                srcIndex++;
            }
            return result;
        }
    }
}

#if APLAY_PROFILING
    #if defined(__GNUC__) || (defined(__MWERKS__) && (__MWERKS__ >= 0x3000)) || (defined(__ICC) && (__ICC >= 600)) || defined(__ghs__)
        #define APLAY_FUNC_SIG __PRETTY_FUNCTION__
    #elif defined(__DMC__) && (__DMC__ >= 0x810)
        #define APLAY_FUNC_SIG __PRETTY_FUNCTION__
    #elif (defined(__FUNCSIG__) || (_MSC_VER))
        #define APLAY_FUNC_SIG __FUNCSIG__
    #elif (defined(__INTEL_COMPILER) && (__INTEL_COMPILER >= 600)) || (defined(__IBMCPP__) && (__IBMCPP__ >= 500))
        #define APLAY_FUNC_SIG __FUNCTION__
    #elif defined(__BORLANDC__) && (__BORLANDC__ >= 0x550)
        #define APLAY_FUNC_SIG __FUNC__
    #elif defined(__STDC_VERSION__) && (__STDC_VERSION__ >= 199901)
        #define APLAY_FUNC_SIG __func__
    #elif defined(__cplusplus) && (__cplusplus >= 201103)
        #define APLAY_FUNC_SIG __func__
    #else
        #define APLAY_FUNC_SIG "APLAY_FUNC_SIG unknown!"
    #endif

    #define APLAY_PROFILE_BEGIN_SESSION(name, filepath) ::APlay::Instrumentor::Get().BeginSession(name, filepath)
    #define APLAY_PROFILE_END_SESSION() ::APlay::Instrumentor::Get().EndSession()
    #define APLAY_PROFILE_SCOPE_LINE2(name, line) constexpr auto fixedName##line = ::APlay::InstrumentorUtils::CleanupOutputString(name, "__cdecl ");\
                                               ::APlay::InstrumentationTimer timer##line(fixedName##line.Data)
    #define APLAY_PROFILE_SCOPE_LINE(name, line) APLAY_PROFILE_SCOPE_LINE2(name, line)
    #define APLAY_PROFILE_SCOPE(name) APLAY_PROFILE_SCOPE_LINE(name, __LINE__)
    #define APLAY_PROFILE_FUNCTION() APLAY_PROFILE_SCOPE(APLAY_FUNC_SIG)
#else
    #define APLAY_PROFILE_BEGIN_SESSION(name, filepath)
    #define APLAY_PROFILE_END_SESSION()
    #define APLAY_PROFILE_SCOPE(name)
    #define APLAY_PROFILE_FUNCTION()
#endif


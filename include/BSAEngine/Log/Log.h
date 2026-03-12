#pragma once

#include <memory>
#include <spdlog/spdlog.h>

namespace BSA {

    // ---------------------------------------------------------------
    // Loglama Sistemi
    // İki ayrı logger bulunur:
    //   - Engine Logger : Motor içi mesajlar için
    //   - Client Logger : Kullanıcı (oyun) tarafı mesajlar için
    // ---------------------------------------------------------------
    class Log {
    public:
        static void Init();

        static std::shared_ptr<spdlog::logger>& GetEngineLogger() { return s_EngineLogger; }
        static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }

    private:
        static std::shared_ptr<spdlog::logger> s_EngineLogger;
        static std::shared_ptr<spdlog::logger> s_ClientLogger;
    };

} // namespace BSA

// -------------------------------------------------------------------
// Engine Log Makroları
// -------------------------------------------------------------------
#define BSA_ENGINE_TRACE(...)    ::BSA::Log::GetEngineLogger()->trace(__VA_ARGS__)
#define BSA_ENGINE_INFO(...)     ::BSA::Log::GetEngineLogger()->info(__VA_ARGS__)
#define BSA_ENGINE_WARN(...)     ::BSA::Log::GetEngineLogger()->warn(__VA_ARGS__)
#define BSA_ENGINE_ERROR(...)    ::BSA::Log::GetEngineLogger()->error(__VA_ARGS__)
#define BSA_ENGINE_CRITICAL(...) ::BSA::Log::GetEngineLogger()->critical(__VA_ARGS__)

// -------------------------------------------------------------------
// Client Log Makroları
// -------------------------------------------------------------------
#define BSA_TRACE(...)           ::BSA::Log::GetClientLogger()->trace(__VA_ARGS__)
#define BSA_INFO(...)            ::BSA::Log::GetClientLogger()->info(__VA_ARGS__)
#define BSA_WARN(...)            ::BSA::Log::GetClientLogger()->warn(__VA_ARGS__)
#define BSA_ERROR(...)           ::BSA::Log::GetClientLogger()->error(__VA_ARGS__)
#define BSA_CRITICAL(...)        ::BSA::Log::GetClientLogger()->critical(__VA_ARGS__)

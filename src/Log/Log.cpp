#include "BSAEngine/Log/Log.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>

namespace BSA {

    std::shared_ptr<spdlog::logger> Log::s_EngineLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // Log formatı: [saat:dakika:saniye.milisaniye] [logger adı] mesaj
        // Renk destekli konsol çıktısı + dosya çıktısı

        std::vector<spdlog::sink_ptr> engineSinks;
        engineSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        engineSinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/BSAEngine.log", true));

        engineSinks[0]->set_pattern("%^[%T] [%n] %v%$");
        engineSinks[1]->set_pattern("[%T] [%l] [%n] %v");

        s_EngineLogger = std::make_shared<spdlog::logger>("BSA", engineSinks.begin(), engineSinks.end());
        s_EngineLogger->set_level(spdlog::level::trace);
        s_EngineLogger->flush_on(spdlog::level::trace);
        spdlog::register_logger(s_EngineLogger);

        std::vector<spdlog::sink_ptr> clientSinks;
        clientSinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
        clientSinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("logs/BSAEngine.log", false));

        clientSinks[0]->set_pattern("%^[%T] [%n] %v%$");
        clientSinks[1]->set_pattern("[%T] [%l] [%n] %v");

        s_ClientLogger = std::make_shared<spdlog::logger>("APP", clientSinks.begin(), clientSinks.end());
        s_ClientLogger->set_level(spdlog::level::trace);
        s_ClientLogger->flush_on(spdlog::level::trace);
        spdlog::register_logger(s_ClientLogger);

        s_EngineLogger->info("Loglama sistemi baslatildi.");
    }

} // namespace BSA

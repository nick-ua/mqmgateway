#pragma once

#include "spdlog/spdlog.h"
#ifdef WITH_SYSTEMD
#include "spdlog/sinks/systemd_sink.h"
#endif


namespace modmqttd {

class Log {
    public:
        enum severity
        {
            none,
            critical,
            error,
            warn,
            info,
            debug,
            trace
        };

        static void init_logging(severity level);

        static bool isRunningUnderSystemd();
        static std::shared_ptr<spdlog::logger> new_logger(const std::string name);
};

}

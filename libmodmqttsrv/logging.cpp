#include <cstddef>
#include <sys/stat.h>
#include <string>
#include <ostream>
#include <fstream>
#include <iomanip>
#include "spdlog/spdlog.h"
#ifdef WITH_SYSTEMD
#include "spdlog/sinks/systemd_sink.h"
#endif
#include "logging.hpp"


namespace modmqttd {


std::ostream& operator<< (std::ostream& strm, Log::severity level)
{
    static const char* strings[] =
    {
        "NONE",
        "CRITICAL",
        "ERROR",
        "WARN ",
        "INFO ",
        "DEBUG",
        "TRACE"
    };

    if (static_cast< std::size_t >(level) < sizeof(strings) / sizeof(*strings))
        strm << strings[level];
    else
        strm << static_cast< int >(level);

    return strm;
}


void Log::init_logging(severity level) {
        switch (level)
        {
        case modmqttd::Log::critical:
            spdlog::set_level(spdlog::level::critical);
            break;
        case modmqttd::Log::error:
            spdlog::set_level(spdlog::level::err);
            break;
        case modmqttd::Log::warn:
            spdlog::set_level(spdlog::level::warn);
            break;
        case modmqttd::Log::info:
            spdlog::set_level(spdlog::level::info);
            break;
        case modmqttd::Log::debug:
            spdlog::set_level(spdlog::level::debug);
            break;
        case modmqttd::Log::trace:
            spdlog::set_level(spdlog::level::trace);
            break;
        case modmqttd::Log::none:
        default:
            spdlog::set_level(spdlog::level::off);
            return;
        }
#ifdef WITH_SYSTEMD
    bool log_timestamp = true;

    //check JOURNAL_STREAM indoe vs stderr inode
    const char* js = std::getenv("JOURNAL_STREAM");
    if (js != nullptr) {
        std::string jstr(js);
        size_t t = jstr.find(':');
        if (jstr.size() > 2 &&  t > 0) {
            std::string env_inode(jstr.substr(t+1));
            if (!env_inode.empty()) {
                struct stat file_stat;
                int ret, inode;
                ret = fstat (2, &file_stat);
                if (ret >= 0) {
                    inode = file_stat.st_ino;
                    if (std::to_string(inode) == env_inode)
                        log_timestamp = false;
                }
            };
        }
    }
    
    if (!log_timestamp){
        auto systemd_sink = std::make_shared<spdlog::sinks::systemd_sink_mt>();
        auto logger = std::make_shared<spdlog::logger>("modmqttd", systemd_sink);
        spdlog::set_default_logger(logger);
    }
#endif
}

}

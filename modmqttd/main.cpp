#include <iostream>

#include <getopt.h>
#include <filesystem>
#include "libmodmqttsrv/common.hpp"
#include "libmodmqttsrv/modmqtt.hpp"
#include "config.hpp"
#include "spdlog/spdlog.h"

using namespace std;

modmqttd::ModMqtt server;


int main(int ac, char* av[]) {
    std::string configPath;
    try {
        modmqttd::Log::severity level = modmqttd::Log::severity::info;

        int logLevel;

        struct option cmdOptions[] = {
            {"help", no_argument, nullptr, 'h'},
            {"loglevel", required_argument, nullptr, 'l'},
            {"config", required_argument, nullptr, 'c'},
            {nullptr, 0, nullptr, 0} // End of options
        };

        int opt;
        while ((opt = getopt_long(ac, av, "hl:c:", cmdOptions, nullptr)) != -1) {
            switch (opt) {
                case 'h':
                    std::cout << "Arguments:\n" << "  --help\t\tproduce help message\n" \
                        << "  --loglevel arg\tset log level 1-6, higher is more verbose\n" \
                        << "  --config arg\t\tpath to configuration file\n";
                    return EXIT_SUCCESS;
                case 'l':
                    logLevel = std::stoi(optarg); // Convert string to int
                    level = (modmqttd::Log::severity)(logLevel);
                    if (logLevel < 0 || logLevel > 6) {
                        std::cerr << "Log level must be between 0 and 6.\n";
                        return 1;
                    }
                    break;
                case 'c':
                    configPath = optarg; // Get the config path
                    break;
                default:
                    std::cerr << "Unknown option: " << char(optopt) << "\n";
                    return 1;
            }
        }

        modmqttd::Log::init_logging(level);
        // TODO add version information
        spdlog::info("modmqttd is starting");

        server.init(configPath);
        server.start();

        spdlog::info("modmqttd stopped"); 
        return EXIT_SUCCESS;
    } catch (const YAML::BadFile& ex) {
        if (configPath == "")
            configPath = std::filesystem::current_path().native();
        std::string msg = "Failed to load configuration from "s + configPath;
        spdlog::critical("{}", msg.c_str() );
    } catch (const std::exception& ex) {
        spdlog::critical("{}",  ex.what());
    } catch (...) {
        spdlog::critical("Unknown initialization error occured");
    }
    return EXIT_FAILURE;
}

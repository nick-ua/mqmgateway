#include <iostream>

#include <boost/program_options.hpp>
#include <boost/filesystem.hpp>
#include "libmodmqttsrv/common.hpp"
#include "libmodmqttsrv/modmqtt.hpp"
#include "config.hpp"
#include "spdlog/spdlog.h"

namespace args = boost::program_options;
using namespace std;

modmqttd::ModMqtt server;


int main(int ac, char* av[]) {
    std::string configPath;
    try {
        args::options_description desc("Arguments");

        int logLevel;

        desc.add_options()
            ("help", "produce help message")
            ("loglevel, l", args::value<int>(&logLevel), "setup logging: 0 off, 1-6 sets loglevel, higher is more verbose")
            ("config, c", args::value<string>(&configPath), "path to configuration file")
        ;

        args::variables_map vm;
        args::store(args::parse_command_line(ac, av, desc), vm);
        args::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return EXIT_SUCCESS;
        }

        modmqttd::Log::severity level = modmqttd::Log::severity::info;
        if (vm.count("loglevel")) {
            level = (modmqttd::Log::severity)(vm["loglevel"].as<int>());
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
            configPath = boost::filesystem::current_path().native();
        std::string msg = "Failed to load configuration from "s + configPath;
        spdlog::critical("{}", msg.c_str() );
    } catch (const std::exception& ex) {
        spdlog::critical("{}",  ex.what());
    } catch (...) {
        spdlog::critical("Unknown initialization error occured");
    }
    return EXIT_FAILURE;
}

#pragma once


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
};

}

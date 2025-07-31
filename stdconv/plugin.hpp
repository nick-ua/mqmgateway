#pragma once

#include "libmodmqttconv/converterplugin.hpp"

class StdConvPlugin : ConverterPlugin {
    public:
        virtual std::string getName() const { return "std"; }
        virtual DataConverter* getConverter(const std::string& name);
        virtual ~StdConvPlugin() {}
};

extern "C" __attribute__((__visibility__("default"))) StdConvPlugin converter_plugin;
StdConvPlugin converter_plugin;

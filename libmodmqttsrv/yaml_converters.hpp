#pragma once

#include <chrono>
#include <regex>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string.hpp>

#include <yaml-cpp/yaml.h>
#include "libmodmqttsrv/exceptions.hpp"
#include "libmodmqttsrv/config.hpp"

template<>
struct YAML::convert<modmqttd::ModbusNetworkConfig::RtuSerialMode> {
    static bool decode(const YAML::Node& node, modmqttd::ModbusNetworkConfig::RtuSerialMode& rhs) {
        auto str = node.as<std::string>();
        if (str == "rs232") {
            rhs = modmqttd::ModbusNetworkConfig::RtuSerialMode::RS232;
        } else if (str == "rs485") {
            rhs = modmqttd::ModbusNetworkConfig::RtuSerialMode::RS485;
        } else {
            return false;
        }
        return true;
    }
};

template<>
struct YAML::convert<modmqttd::ModbusNetworkConfig::RtuRtsMode> {
    static bool decode(const YAML::Node& node, modmqttd::ModbusNetworkConfig::RtuRtsMode& rhs) {
        auto str = node.as<std::string>();
        if (str == "down") {
            rhs = modmqttd::ModbusNetworkConfig::RtuRtsMode::DOWN;
        } else if (str == "up") {
            rhs = modmqttd::ModbusNetworkConfig::RtuRtsMode::UP;
        } else {
            return false;
        }
        return true;
    }
};

template<>
struct YAML::convert<std::chrono::milliseconds> {
    static bool decode(const YAML::Node& node, std::chrono::milliseconds& value) {
        const std::regex re("([0-9]+)(ms|s|min)");
        std::cmatch matches;
        std::string strval = node.as<std::string>();

        if (!std::regex_match(strval.c_str(), matches, re))
            throw modmqttd::ConfigurationException(node.Mark(), "Invalid time specification");

        int mval = std::stoi(matches[1]);
        std::string unit = matches[2];
        if (unit == "s")
            mval *= 1000;
        else if (unit == "min")
            mval *= 1000 * 60;

        value = std::chrono::milliseconds(mval);
        return true;
    }
};

/**
 * A converter for list of integers in format
 * 1,2,3,4-5,6
 *
 * For single number std::pair<number, number> is returned
*/

template<>
struct YAML::convert<std::vector<std::pair<int,int>>> {
    static int toNumber(const YAML::Node& node, const std::string& val) {
        try {
            size_t idx = 0;
            int ret = std::stoi(val, &idx);
            if (idx != val.size())
                throw modmqttd::ConfigurationException(node.Mark(), std::string("Conversion to number failed, unknown char at ") + std::to_string(idx) + " position in " + val);
            return ret;
        } catch (const std::invalid_argument& ex) {
            throw modmqttd::ConfigurationException(node.Mark(), std::string("Conversion to number or number list failed for [") + val + "]");
        } catch (const std::out_of_range& ex) {
            throw modmqttd::ConfigurationException(node.Mark(), "Conversion to number or number list contains number that is out of range");
        }
    }

    static bool decode(const YAML::Node& node, std::vector<std::pair<int,int>>& value) {
        const std::regex re_range("\\s*([0-9]+)-([0-9]+)\\s*");

        std::string strval(node.as<std::string>());
        std::regex re_trim("^\\s+|\\s+$");
        strval = std::regex_replace(strval, re_trim, "");

        std::istringstream iss(strval);
        std::string t;
        while (std::getline(iss, t, ','))
        {
            std::cmatch matches;
            std::pair<int,int> item;
            if (std::regex_match(t.c_str(), matches, re_range)) {
                item.first = toNumber(node, matches[1]);
                item.second = toNumber(node, matches[2]);
            } else {
                item.first = toNumber(node, t);
                item.second = item.first;
            }
            value.push_back(item);
        }

        return true;
    }
};


template<>
struct YAML::convert<std::vector<std::string>> {
    static bool decode(const YAML::Node& node, std::vector<std::string>& value) {
        std::string strval(node.as<std::string>());
        std::istringstream iss(strval);
        std::string token;
        while (std::getline(iss, token, ','))
        {
            std::regex re_trim("^\\s+|\\s+$");
            token = std::regex_replace(token, re_trim, "");
            value.push_back(token);
        }

        return true;
    }
};


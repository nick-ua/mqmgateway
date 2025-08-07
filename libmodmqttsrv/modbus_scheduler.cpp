#include "modbus_scheduler.hpp"
#include "modbus_types.hpp"

#include "spdlog/spdlog.h"


namespace modmqttd {

std::map<int, std::vector<std::shared_ptr<RegisterPoll>>>
ModbusScheduler::getRegistersToPoll(
    std::chrono::steady_clock::duration& outDuration,
    const std::chrono::time_point<std::chrono::steady_clock>& timePoint
) {
    std::map<int, std::vector<std::shared_ptr<RegisterPoll>>> ret;

    //_logger->trace("initial outduration {}",std::chrono::duration_cast<std::chrono::milliseconds>(outDuration).count()); 

    outDuration = std::chrono::steady_clock::duration::max();
    for(std::map<int, std::vector<std::shared_ptr<RegisterPoll>>>::const_iterator slave = mRegisterMap.begin();
        slave != mRegisterMap.end(); slave++)
    {
        for(std::vector<std::shared_ptr<RegisterPoll>>::const_iterator reg_it = slave->second.begin();
            reg_it != slave->second.end(); reg_it++)
        {
            const RegisterPoll& reg = **reg_it;

            auto time_passed = timePoint - reg.mLastRead;
            auto time_to_poll = reg.mRefresh;

            //_logger->trace("time passed:  {}",std::chrono::duration_cast<std::chrono::milliseconds>(time_to_poll).count()); 

            if (time_passed >= reg.mRefresh) {
                _logger->trace("Register {}.{} (0x{:x}.0x{:x}) added, last read {}ms ago", \
                    slave->first, \
                    reg.mRegister, \
                    slave->first, \
                    reg.mRegister, \
                    std::chrono::duration_cast<std::chrono::milliseconds>(time_passed).count() \
                );
                ret[slave->first].push_back(*reg_it);
            } else {
                time_to_poll = reg.mRefresh - time_passed;
            }

            if (outDuration > time_to_poll) {
                outDuration = time_to_poll;
                _logger->trace("Wait duration set to {}ms as next poll for register {}.{} (0x{:x}.0x{:x})", \
                                    std::chrono::duration_cast<std::chrono::milliseconds>(time_to_poll).count(), \
                                    slave->first, \
                                    reg.mRegister, \
                                    slave->first, \
                                    reg.mRegister \
                );
            }
        }
    }

    return ret;
}

std::shared_ptr<RegisterPoll>
ModbusScheduler::findRegisterPoll(const MsgRegisterValues& pValues) const {

    std::map<int, std::vector<std::shared_ptr<RegisterPoll>>>::const_iterator slave = mRegisterMap.find(pValues.mSlaveId);
    if (slave != mRegisterMap.end()) {
        std::vector<std::shared_ptr<RegisterPoll>>::const_iterator reg_it = std::find_if(
            slave->second.begin(), slave->second.end(),
            [&pValues](const std::shared_ptr<RegisterPoll>& item) -> bool { return item->overlaps(pValues); }
        );
        if (reg_it != slave->second.end()) {
            return *reg_it;
        }
    }

    return std::shared_ptr<RegisterPoll>();
}

}

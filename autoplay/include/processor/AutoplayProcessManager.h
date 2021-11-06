#pragma once

#include <functional>
#include <memory>
#include <thread>

namespace APlay {

    class IAutoplayProcessManager {
    public:
        virtual std::size_t GetThreadsCount() = 0;
        virtual void _execute() = 0;
        virtual void _delegate() = 0;
    };

    // Data must be iterable
    template<typename T, typename _Iterator>
    std::shared_ptr<IAutoplayProcessManager> CreateProcessManager(
        const T& data,
        const std::function<int(const T& container)> func,
        const std::function<std::size_t(const T& data)>& manager = [](const T& data) -> std::size_t {
            return data.size() > std::thread::hardware_concurrency() ? data.size() / std::thread::hardware_concurrency() : data.size();
        }
    );

}

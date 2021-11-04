#include "AutoplayProcessManager.h"
#include "AutoplayLog.h"

#include <condition_variable>
#include <mutex>
#include <vector>
#include "opencv2/opencv.hpp"

namespace APlay {

    template<typename T, typename _Iterator>
    class AutoplayProcessManager : public IAutoplayProcessManager {
    public:
        AutoplayProcessManager(
            const T& data,
            const std::function<int(const T& container)> func,
            const std::function<std::size_t(const T& data)> manager
        ) : _data(data), _completed(0) {
            _function = func;
            _manager = manager;
            _threadsCount = _manager(_data);
            if (data.size() == 1) _threadsCount = 1;
            _delegate();
        }

        ~AutoplayProcessManager() {
            APLAY_PROFILE_SCOPE("AutoplayProcessManager::~AutoplayProcessManager()");
        }

        virtual std::size_t GetThreadsCount() override {
            return _threadsCount;
        }

        virtual void _execute() override;

    private:
        std::size_t _threadsCount;
        std::vector<std::thread> _threads;
        std::condition_variable _con;
        std::mutex _mtx;

        std::function<int(const T& container)> _function;
        std::function<std::size_t(const T& data)> _manager;

        T _data;
        std::size_t _completed;

    protected:
        virtual void _delegate() override;
    };


    template<typename T, typename _Iterator>
    void AutoplayProcessManager<T, _Iterator>::_delegate() {
        APLAY_PROFILE_FUNCTION();
        if (_threadsCount > _data.size()) _threadsCount = _data.size();

        int step = (_data.size() / _threadsCount) - (_data.size() % _threadsCount);

        auto _threadFunction = [&](const T& ctn) {
            APLAY_PROFILE_SCOPE("ProcesseManager [&](PointVec::iterator begin, PointVec::iterator end) -> int");
            _function(ctn);
            _completed += step;
            _con.notify_one();
        };

        for (int i = 0; i <= _data.size() - step; i += step ) {
            T container(_data.begin() + i, _data.begin() + i + step);
            _threads.emplace_back(std::thread(_threadFunction, container));
        }

        if (_data.size() % step ) {
            T container(_data.end() - (_data.size() % step), _data.end());
            _threads.emplace_back(std::thread(_threadFunction, container));   
        }

       if (_data.size() % _threadsCount > 0) _threadsCount++;
    }

    template<typename T, typename _Iterator>
    void AutoplayProcessManager<T, _Iterator>::_execute() {
        APLAY_PROFILE_FUNCTION();
        for (int i = 0; i < _threads.size(); i++) {
            _threads.at(i).detach();
        }
        std::unique_lock<std::mutex> lk(_mtx);
        // _con.wait(lk, [&]() { return _completed >= _data.size(); });
        _con.wait_for(lk, std::chrono::milliseconds(5000), [&]() { return _completed >= _data.size(); } );
    }


}


namespace APlay {

    using PointVec = std::vector<std::vector<cv::Point>>;
    using LongIntVec = std::vector<std::size_t>;

    // PointVec
    template<> std::shared_ptr<IAutoplayProcessManager> CreateProcessManager<PointVec, PointVec::iterator>(
        const PointVec & data,
        // const std::function<int(PointVec::iterator begin, PointVec::iterator end)> func,
        const std::function<int(const PointVec& container)> func,
        const std::function<std::size_t(const PointVec& data)>& manager
    ) {
        APLAY_PROFILE_FUNCTION();
        return std::make_shared<AutoplayProcessManager<PointVec, PointVec::iterator>>(data, func, manager);
    }

    // LongIntVec
    template<> std::shared_ptr<IAutoplayProcessManager> CreateProcessManager<LongIntVec, LongIntVec::iterator>(
        const LongIntVec & data,
        // const std::function<int(LongIntVec::iterator begin, LongIntVec::iterator end)> func,
        const std::function<int(const LongIntVec& container)> func,
        const std::function<std::size_t(const LongIntVec& data)>& manager
    ) {
        APLAY_PROFILE_FUNCTION();
        return std::make_shared<AutoplayProcessManager<LongIntVec, LongIntVec::iterator>>(data, func, manager);
    }

    /**
     * !!!Override containers here!!!
    */

}

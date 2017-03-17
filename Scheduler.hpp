#ifndef SCHEDULER_HPP
#define SCHEDULER_HPP

#include <thread>
#include <chrono>

class Scheduler
{
private:
    Scheduler(const Scheduler& rhs) = delete;
    Scheduler& operator=(const Scheduler& rhs) = delete;
public:
    Scheduler() = default;
    
    template <typename Func, typename ...Args>
    void run_in(std::chrono::seconds timeout, const Func &f, Args ...args) 
    {
    std::thread local([timeout, f, args...]() {
        std::this_thread::sleep_for(timeout);
        f(args...);
    });
    local.detach();
    }

    template <typename Func, typename ...Args>
    void run_at(std::chrono::system_clock::time_point tp, const Func &f, Args ...args) {
        if (std::chrono::system_clock::now() > tp)
            tp += std::chrono::hours(24);
        std::thread local([tp, f, args...]() {
            std::this_thread::sleep_until(tp);
            f(args...);
        });
        local.detach();
    }
    
    static std::chrono::system_clock::time_point stotp(const std::string &s) {
    struct tm * now;
    time_t getcurrTime = time(NULL);
    now = localtime(&getcurrTime);
    now->tm_hour = stoi(s.substr(0, 2));
    now->tm_min = stoi(s.substr(3));
    now->tm_sec = 0;
    getcurrTime = mktime(now);
    return std::chrono::system_clock::from_time_t(getcurrTime);
    }
    
    template <typename Func, typename ...Args>
    void run_every(const bool &interrupt, const std::chrono::system_clock::duration &dur, 
    const Func &f, Args ...args)
    {
        while (interrupt) {
            std::thread thr([dur, f, &args...](){
                f(args...);
                std::this_thread::sleep_for(dur);
            });
            thr.join();
        }
    }
    ~Scheduler() = default;
};

#endif // SCHEDULER_HPP

//
// Created by liquidcore7 on 3/16/17.
//

#ifndef REMINDERBOT2_MISC_HPP
#define REMINDERBOT2_MISC_HPP

#include <string>
#include <tgbot/tgbot.h>
#include "Scheduler.hpp"
#include "DBase.hpp"
#include <regex>
#include <algorithm>

std::pair<std::string, std::string> parse_set_command(const std::string &s)
{
    if (std::count(s.begin(), s.end(), ' ') < 2)
        return {"ERR", "Both notification text and time must be entered"};
    auto beg = std::find(s.begin(), s.end(), ' '),
            end = std::find(s.rbegin(), s.rend(), ' ').base();
    std::string notification(beg + 1, end - 1), time(end, s.end());
    std::regex timePattern("\\d\\d:\\d\\d");
    if (!std::regex_match(time, timePattern) ||
        std::stoi(time.substr(0, 2)) > 23 ||
        std::stoi(time.substr(3)) > 59)
        return {"ERR", "Invalid time format: use HH:MM"};
    return {notification, time};
};

long parse_period(std::string p)
{
    for (auto &c : p)
        c = std::tolower(c);
    auto firstCh = std::find_if(p.begin(), p.end(), [] (const char & ch) { return isalpha(ch);});
    std::regex durPattern("[:digit:]+");
    if (!std::regex_match(std::string(p.begin(), firstCh - 1), durPattern))
        return 0;
    if (p.find("min") != std::string::npos)
        return std::stol(std::string(p.begin(), firstCh));
    else if (p.find("hr") != std::string::npos)
        return std::stol(std::string(p.begin(), firstCh)) * 60;
    else
        return 0;
}

std::pair<std::string, long> parse_period_str(const std::string &s)
{
    if (std::count(s.begin(), s.end(), ' ') < 2)
        return {"ERR", 0};
    auto txtbeg = std::find(s.begin(), s.end(), ' '), txtend = std::find(s.rbegin(), s.rend(), ' ').base();
    return {std::string(txtbeg + 1, txtend - 1), parse_period(std::string(txtend, s.end()))};
};

void startup_dblaunch(const TgBot::Bot &b, DBase &d, Scheduler &s)
{
    sqlite3pp::query fnd(d._db, "SELECT * FROM notifications ORDER BY userid, rtime ASC");
    for (auto nt : fnd)
    {
        std::string Time, txt, id, per;
        nt.getter() >> txt >> Time >> per >> id;
        auto addtotime = std::chrono::hours(d.getGMT(std::stol(id)));
        s.run_at(Scheduler::stotp(Time) - addtotime, [&b, txt, &s, &d, id] () {
            bool a = true;
            std::thread([&b, txt, &a, &s, &d, id](){s.run_every(a, std::chrono::hours(24),
                                                                                   [&b, txt, &d, &a, id] () {b.getApi().sendMessage(std::stol(id), txt); a = d.findRem(txt);});}).detach();});
    }
}


#endif //REMINDERBOT2_MISC_HPP

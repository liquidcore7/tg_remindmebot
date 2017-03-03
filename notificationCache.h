//
// Created by liquidcore7 on 2/28/17.
//

#ifndef TG_REMINDMEBOT_NOTIFICATIONCACHE_H
#define TG_REMINDMEBOT_NOTIFICATIONCACHE_H

#include <algorithm>
#include <regex>
#include <fstream>

const static std::string dumpfile = "remindbot-cache.dmp";

std::pair<std::string, std::string> parse(const std::string &s)
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

void useCache(const TgBot::Bot &bot)
{
    std::string command;
    std::vector<std::string> chatIDs;
    std::ifstream read(dumpfile);
    while (getline(read, command)) {
        chatIDs.push_back(command);
    }
    read.close();
    for (const auto &filename : chatIDs) {
        std::ifstream reminders_read(dumpfile + filename);
        std::string notification;
        while (getline(reminders_read, notification)) {
            auto sep = parse(notification);
            run_separate(bot, std::stol(filename), str_to_dur(sep.second), sep.first);
        }
        reminders_read.close();
    }
}

void cyclic(bool b, const TgBot::Bot &bot) {       //run this not from main thread
    while (b) {
        std::thread thr([bot](){
            useCache(bot);
            std::this_thread::sleep_for(std::chrono::hours(24));
        });
        thr.join();
    }
}


#endif //TG_REMINDMEBOT_NOTIFICATIONCACHE_H

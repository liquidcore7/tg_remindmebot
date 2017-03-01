//
// Created by liquidcore7 on 2/26/17.
//

#ifndef TG_REMINDMEBOT_TIMEHANDLING_H
#define TG_REMINDMEBOT_TIMEHANDLING_H

#include <chrono>
#include <thread>
#include <ctime>
#include <iostream>
#include <tgbot/tgbot.h>

std::chrono::seconds str_to_dur(const std::string &time_str)
{
    struct tm * now, usrTime = {0};
    time_t getcurrTime = time(NULL);
    now = localtime(&getcurrTime);
    usrTime.tm_year = now->tm_year;
    usrTime.tm_mon = now->tm_mon;
    usrTime.tm_mday = now->tm_mday;
    usrTime.tm_hour = stoi(time_str.substr(0, 2));
    usrTime.tm_min = stoi(time_str.substr(3));
    usrTime.tm_sec = 0;
    auto diff =  std::chrono::seconds(static_cast<unsigned long>
                                      (difftime(mktime(&usrTime), getcurrTime)));
    if (diff >= std::chrono::seconds(0))
        return diff;
    else
        return std::chrono::seconds(60*60*24  + diff.count());
}

void run_separate(const TgBot::Bot &b, const int64_t &chatid,
                  std::chrono::seconds timeout, const std::string &reply) {
    std::thread local([b, chatid, timeout, reply]() {
        std::this_thread::sleep_for(timeout);
        b.getApi().sendMessage(chatid, reply);
    });
    local.detach();
    std::cout << "separate thread called" << std::endl;
};

#endif //TG_REMINDMEBOT_TIMEHANDLING_H

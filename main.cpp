#include "Scheduler.hpp"
#include "DBase.hpp"
#include "Misc.hpp"
#include <tgbot/tgbot.h>
#include <iostream>

using TgBot::Bot;
using TgBot::Message;

//TODO: period full support

int main() {
    //Init:
    Bot botObj("TOKEN");
    Scheduler sq;
    DBase dbcache;
    startup_dblaunch(botObj, dbcache, sq);
    //hello
    botObj.getEvents().onCommand("start", [&botObj, &dbcache] (const Message::Ptr mp) {
        if (dbcache.findUsr(mp->chat->id))
            botObj.getApi().sendMessage(mp->chat->id, "No need to run this for the second time!");
        else {
        TgBot::InlineKeyboardMarkup::Ptr gmtlist(new TgBot::InlineKeyboardMarkup);
        for (int i = -11; i <= 12; ++i) {
            TgBot::InlineKeyboardButton::Ptr zone(new TgBot::InlineKeyboardButton);
            zone->text = "GMT" + ((i > 0) ? std::string(" +") : std::string(" ")) + std::to_string(i);
            zone->callbackData = std::to_string(i);
            gmtlist->inlineKeyboard.push_back({zone});
        }
        botObj.getApi().sendMessage(mp->chat->id, "Please choose your timezone:", false,0, gmtlist);
        botObj.getEvents().onCallbackQuery([&botObj, &dbcache] (const TgBot::CallbackQuery::Ptr clptr){
            dbcache.addUsr(clptr->message->chat->id, std::stoi(clptr->data));
            botObj.getApi().sendMessage(clptr->message->chat->id, "Timezone set. Now you can add reminders!");
        });
    }});
    //end
    botObj.getEvents().onCommand("stop", [&botObj, &dbcache] (Message::Ptr mptr) {
        if (!dbcache.findUsr(mptr->chat->id))
            botObj.getApi().sendMessage(mptr->chat->id, "No need to run this twice");
        else {
        dbcache.removeUsr(mptr->chat->id);
        botObj.getApi().sendMessage(mptr->chat->id, "Thank you for using RemindMe bot. Good luck!");
    }});
   botObj.getEvents().onCommand("set", [&botObj, &dbcache, &sq] (Message::Ptr mptr) {
        auto details = parse_set_command(mptr->text);
        if (details.first == "ERR")
            botObj.getApi().sendMessage(mptr->chat->id, details.second);
        else
        {
            botObj.getApi().sendMessage(mptr->chat->id, "Notification set.");
            dbcache.addRem(details.first, details.second, mptr->chat->id);
            auto addtotime = std::chrono::hours(dbcache.getGMT(mptr->chat->id));
            sq.run_at(Scheduler::stotp(details.second) - addtotime, [&botObj, details, mptr, &sq, &dbcache] () {
                bool a = true;
            std::thread([&botObj, details, mptr, &a, &sq, &dbcache](){sq.run_every(a, std::chrono::hours(24),
                                                                     [&botObj, details, mptr, &dbcache, &a] () {botObj.getApi().sendMessage(mptr->chat->id, details.first); a = dbcache.findRem(details.first);});}).detach();});
        }
    });
    botObj.getEvents().onCommand("remove", [&botObj, &dbcache] (Message::Ptr mp)
    {
        auto lines = dbcache.getRems(mp->chat->id);
        if (lines.size() == 1 && lines.front() == "No notifications set. Try to add one") {
            botObj.getApi().sendMessage(mp->chat->id, lines.front());
        } else
        {
        TgBot::InlineKeyboardMarkup::Ptr variants(new TgBot::InlineKeyboardMarkup);
        for (const std::string &singleR : lines) {
            TgBot::InlineKeyboardButton::Ptr remBtn(new TgBot::InlineKeyboardButton);
            remBtn->text = singleR;
            remBtn->callbackData = singleR;
            variants->inlineKeyboard.push_back({remBtn});
        }
        botObj.getApi().sendMessage(mp->chat->id, "Select reminder to remove:",
        false, 0, variants);
        botObj.getEvents().onCallbackQuery([&botObj, &dbcache] (const TgBot::CallbackQuery::Ptr cqptr)
                                           {
                                               dbcache.removeRem(cqptr->data);
                                               botObj.getApi().sendMessage(cqptr->message->chat->id, "Notification removed.");
                                           });
    }});
    botObj.getEvents().onCommand("setperiod", [&botObj, &dbcache] (const Message::Ptr mpt)
    {
        auto pr = parse_period_str(mpt->text);
        if (pr.first == "ERR")
            botObj.getApi().sendMessage(mpt->chat->id, "Syntax error: string must contain (part of) notification`s text and period in hr or min.");
        else if (!dbcache.findRem(pr.first))
            botObj.getApi().sendMessage(mpt->chat->id, "Notification not found!");
        else
        {
            dbcache.setPeriod(pr.first, mpt->chat->id, pr.second);
            botObj.getApi().sendMessage(mpt->chat->id, "Period for notification set.");
        }
    });
    //run
    TgBot::TgLongPoll longPoll(botObj);
    while (true)
        longPoll.start();
    return 0;
}

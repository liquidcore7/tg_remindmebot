#include "timeHandling.h"
#include "notificationCache.h"

using namespace std;
using namespace TgBot;

static bool BotRunning = true;
const string helloMessage("RemindMe bot there!");

int main() {
    //Initiate bot:
    Bot botObj("362898856:AAHVVdCjzYTOnaxCoFGEFJ7j7t15vCzTfEk");
    //update every 24h
    thread([botObj](){cyclic(BotRunning, botObj);}).detach();
    //say hello, initiate cache:
    botObj.getEvents().onCommand("start", [&botObj] (Message::Ptr mes) {
        botObj.getApi().sendMessage(mes->chat->id, helloMessage);
        ofstream wr(dumpfile, ios::app | ios::out);
        wr << mes->chat->id << '\n';
        wr.close();
    });
    //settime:
    botObj.getEvents().onCommand("set", [&botObj] (Message::Ptr msg)
    {
        auto repl = parse(msg->text);
        if (repl.first == "ERR")
            botObj.getApi().sendMessage(msg->chat->id, repl.second);
        else
        {
            botObj.getApi().sendMessage(msg->chat->id, "Notification set to "
                                                       + repl.second);
            run_separate(botObj, msg->chat->id, str_to_dur(repl.second), repl.first);
            ofstream wr(dumpfile + to_string(msg->chat->id), ios::app | ios::out);    // see notificationCache.h
            wr << msg->text << '\n';
            wr.close();
        }
    });
    //interrupt func:
    signal(SIGINT, [] (int s) {BotRunning = false;});
    //run:
    TgLongPoll longPollservice(botObj);
    while (BotRunning)
        longPollservice.start();
    return 0;
}
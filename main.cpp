#include "timeHandling.h"
#include "notificationCache.h"

using namespace std;
using namespace TgBot;

static bool BotRunning = true;
const string helloMessage("RemindMe bot there!");

int main() {
    //Initiate bot:
    Bot botObj("362898856:AAHVVdCjzYTOnaxCoFGEFJ7j7t15vCzTfEk");
    //say hello:
    botObj.getEvents().onCommand("start", [&botObj] (Message::Ptr mes) {
        botObj.getApi().sendMessage(mes->chat->id, helloMessage);
    });
    //settime:
    botObj.getEvents().onCommand("set", [&botObj] (Message::Ptr msg)
    {
        auto repl = parse(msg->text);
        if (repl.first == "ERR")
            botObj.getApi().sendMessage(msg->chat->id, repl.second);
        else
            run_separate(botObj, msg, str_to_dur(repl.second), repl.first);
    });
    //interrupt func:
    signal(SIGINT, [] (int s) {BotRunning = false;});
    //run:
    TgLongPoll longPollservice(botObj);
    while (BotRunning)
        longPollservice.start();
    return 0;
}
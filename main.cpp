#include "timeHandling.h"

using namespace std;
using namespace TgBot;

static bool BotRunning = true;
const string helloMessage("RemindMe bot there!");

string gettime_tg(const string& s)
{
    return s.substr(s.find_first_of("0123456789"));
}

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
        run_separate(botObj, msg, str_to_dur(gettime_tg(msg->text)));
    });
    //interrupt func:
    signal(SIGINT, [] (int s) {BotRunning = false;});
    //run:
    TgLongPoll longPollservice(botObj);
    while (BotRunning)
        longPollservice.start();
    return 0;
}
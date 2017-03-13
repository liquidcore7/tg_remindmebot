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
        wr.open(dumpfile + to_string(mes->chat->id), ios::app | ios::out);
        wr << timezone_diff(mes) / 3600 << '\n';
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
            run_separate(botObj, msg->chat->id, str_to_dur(repl.second) -
                                                std::chrono::seconds(timezone_diff(msg)), repl.first);
            ofstream wr(dumpfile + to_string(msg->chat->id), ios::app | ios::out);    // see notificationCache.h
            wr << msg->text << '\n';
            wr.close();
        }
    });
    //list reminders
    botObj.getEvents().onCommand("list", [&botObj](Message::Ptr msg)
    {
        ifstream readR(dumpfile + to_string(msg->chat->id));
        string line, total;
        readR >> line;
        line.clear();
        while (getline(readR, line)) {
            auto pr = parse(line);
            total += pr.first + " (" + pr.second + ")\n";
        }
        readR.close();
        if (total.empty())
            total = "No reminders set. Try to add one!";
        botObj.getApi().sendMessage(msg->chat->id, total);
    });
    //erase reminder
    botObj.getEvents().onCommand("erase", [&botObj](Message::Ptr msg) {
        if (count(msg->text.begin(), msg->text.end(), ' ') < 1)
            botObj.getApi().sendMessage(msg->chat->id, "Enter the name of reminder you want to delete.");
        else {
            ifstream readall(dumpfile + to_string(msg->chat->id));
            string line, total, to_erase = msg->text.substr(msg->text.find(' '));
            while (getline(readall, line)) {
                if (line.find(to_erase) == std::string::npos)
                    total += line + "\n";
            }
            readall.close();
            ofstream newFile(dumpfile + to_string(msg->chat->id));
            newFile << total;
            newFile.close();
            botObj.getApi().sendMessage(msg->chat->id, "Reminder erased.");
        }
    });
    botObj.getEvents().onCommand("timezone", [&botObj](const Message::Ptr mp) {
        botObj.getApi().sendMessage(mp->chat->id, to_string(timezone_diff(mp) / 3600));
    });
    //interrupt func:
    signal(SIGINT, [] (int s) {BotRunning = false;});
    //run:
    TgLongPoll longPollservice(botObj);
    while (BotRunning)
        longPollservice.start();
    return 0;
}

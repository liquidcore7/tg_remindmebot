#ifndef DBASE_HPP
#define DBASE_HPP

#include "sqlite3pp/sqlite3pp.h"
#include <fstream>
#include <tgbot/tgbot.h>
#include <vector>

namespace typ_commands {
    const std::string first_start_comm_usr(
    "CREATE TABLE users(\nchatid INTEGER PRIMARY KEY,\n"
    "gmt INTEGER\n);");
    const std::string first_start_comm_notify(
    "CREATE TABLE notifications(\n"
    "rtext TEXT,\nrtime  TEXT,\nperiod INTEGER,\n"
    "userid INTEGER,\nFOREIGN KEY(userid) REFERENCES users(chatid)\n);");
    const std::string insert_user_comm(
    "INSERT INTO users (chatid, gmt) VALUES (?, ?);");
    const std::string insert_reminder_comm(
    "INSERT INTO notifications (rtext, rtime, period, userid) VALUES (?, ?, ?, ?);");
    const std::string mod_time_comm(
    "UPDATE notifications SET rtime=:tm WHERE userid=:id AND rtext=:txt;");
    const std::string mod_text_comm(
    "UPDATE notifications SET rtext=:txt WHERE userid=:id AND rtime=:tm;");
    const std::string mod_period_comm(
            "UPDATE notifications SET period=:pr WHERE userid=:id AND rtext=:txt");
    const std::string removeRem_comm(
    "DELETE FROM notifications WHERE rtext=?;");
    const std::string removeUsr_comm1(
    "DELETE FROM notifications WHERE userid=:id;");
    const std::string removeUsr_comm2(
            "DELETE FROM users WHERE chatid=:id;");
};

class DBase
{
private:
    DBase(const DBase& rhs) = delete;
    DBase& operator=(const DBase& rhs) = delete;
    sqlite3pp::database _db;
    bool empty() {std::ifstream in("tg_cache.db", std::ifstream::ate | std::ifstream::binary);
    return !in.tellg();}
public:
    DBase() {
        _db.connect("tg_cache.db", SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE);
        if(empty()) {
            sqlite3pp::command first(_db, typ_commands::first_start_comm_usr.c_str()); 
            first.execute();
            sqlite3pp::command second(_db, typ_commands::first_start_comm_notify.c_str());
            second.execute();
            }
    }
    void addUsr(const long id, const int gmt)
    {
        sqlite3pp::command cm(_db, typ_commands::insert_user_comm.c_str());
        cm.bind(1, std::to_string(id), sqlite3pp::nocopy);
        cm.bind(2, std::to_string(gmt), sqlite3pp::nocopy);
        cm.execute();
    }
    void addRem(const std::string &tx, const std::string &tim, const long &usr, const int period = 24)
    {
        sqlite3pp::command cm(_db, typ_commands::insert_reminder_comm.c_str());
        cm.binder() << tx << tim << std::to_string(period) << std::to_string(usr);
        cm.execute();
    }
    void removeRem(const std::string &tx)
    {
        sqlite3pp::command cm(_db, typ_commands::removeRem_comm.c_str());
        cm.binder() << tx;
        cm.execute();
    }
    void removeUsr(const long &id)
    {
        sqlite3pp::command cm(_db, typ_commands::removeUsr_comm1.c_str());
        cm.bind(":id", std::to_string(id), sqlite3pp::nocopy);
        sqlite3pp::command cm2(_db, typ_commands::removeUsr_comm2.c_str());
        cm2.bind(":id", std::to_string(id), sqlite3pp::nocopy);
        cm.execute();
        cm2.execute();
    }

    int getGMT(const long &user)
    {
        std::string qur("SELECT users.gmt FROM users WHERE chatid=\"" + std::to_string(user) + "\";");
        sqlite3pp::query qq(_db, qur.c_str());
        auto el = qq.begin();
        return (*el).get<int>(0);
    }

    std::vector<std::string> getRems(const long &user)
    {
        std::string request("SELECT notifications.rtext FROM notifications WHERE"
                                    " userid=\"" + std::to_string(user) + "\";");
        sqlite3pp::query qr(_db, request.c_str());
        std::vector<std::string> cont;
        for (auto k : qr) {
            std::string notif;
            k.getter() >> notif;
            cont.push_back(notif);
        }
        if (cont.empty())
            cont.push_back("No notifications set. Try to add one");
        return cont;
    }

    bool findUsr(const long &id)
    {
        sqlite3pp::query fnd(_db, ("SELECT users.chatid FROM users WHERE chatid="
        + std::to_string(id) + ";").c_str());
        std::string res;
        for (auto e : fnd)
            e.getter() >> res;
        return res == std::to_string(id);
    }

    bool findRem(const std::string &rtxt)
    {
        sqlite3pp::query fnd(_db, ("SELECT * FROM notifications WHERE rtext=\""
        + rtxt + "\";").c_str());
        std::string res;
        for (auto e : fnd)
            e.getter() >> res;
        return res == rtxt;
    }

    void setPeriod(const std::string &ntxt, const long &id, const long period)
    {
        sqlite3pp::command set(_db, typ_commands::mod_period_comm.c_str());
        set.bind(":pr", std::to_string(period), sqlite3pp::nocopy);
        set.bind(":id", std::to_string(id), sqlite3pp::nocopy);
        set.bind(":txt", ntxt, sqlite3pp::nocopy);
        set.execute();
    }

    friend void startup_dblaunch(const TgBot::Bot &, DBase &, Scheduler &);

    ~DBase() {_db.disconnect();};

};

#endif // DBASE_HPP

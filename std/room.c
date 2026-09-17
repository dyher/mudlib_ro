// std/room.c — room/map base class
inherit "/feature/dbase";
inherit "/feature/name";

void setup() {}

string query_short()
{
    return query("short") ? query("short") : name();
}

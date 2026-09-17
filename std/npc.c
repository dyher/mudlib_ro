// std/npc.c — NPC base class
inherit "/feature/dbase";
inherit "/feature/name";

// 子類覆寫這個來定義互動行為
void on_talk(object who)
{
    write(name() + " nods at you.\n");
}

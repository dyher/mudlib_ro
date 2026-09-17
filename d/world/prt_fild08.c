// d/world/prt_fild08.c — Prontera Field 08 (beginner hunting)
inherit "/std/room";

void create()
{
    set("short", "Prontera Field 08");
    set("long", "Grasslands outside Prontera. Weak monsters roam here.");
    set("exits", ([ "south": "/d/world/prontera", "north": "/d/world/prt_fild00" ]));
    set("mobs", ({ 1002, 1007, 1008 }));
}

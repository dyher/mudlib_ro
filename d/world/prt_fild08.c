// d/world/prt_fild08.c — Prontera Field 08 (hunting ground)
inherit "/std/room";

void create()
{
    set("short", "Prontera Field 08");
    set("long", "Grasslands outside Prontera. Porings and Fabres roam here.");
    set("exits", ([ "south": "/d/world/prontera" ]));
    set("mobs", ({ 1002, 1007, 1008 }));
}

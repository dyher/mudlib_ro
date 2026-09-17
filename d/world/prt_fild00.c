// d/world/prt_fild00.c — Prontera Field 00 (advanced hunting)
inherit "/std/room";

void create()
{
    set("short", "Prontera Field 00");
    set("long", "Deeper wilderness far from town. Stronger monsters lurk here.");
    set("exits", ([ "south": "/d/world/prt_fild08" ]));
    set("mobs", ({ 1009, 1012, 1004 }));
}

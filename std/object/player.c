// std/object/player.c — player data object (preloaded so login can new() it)

string name;
mapping stats;

object S() { return find_object("/std/system/storage"); }

void setup(string n)
{
    name = n;
    stats = ([ "hp":100, "mp":50, "level":1, "exp":0 ]);
}

void restore()
{
    string c = S()->load_text("/data/players/" + name + ".txt");
    string *l;
    if (c) {
        l = explode(c, "\n");
        if (sizeof(l) >= 4)
            stats = ([ "hp":to_int(l[0]), "mp":to_int(l[1]),
                       "level":to_int(l[2]), "exp":to_int(l[3]) ]);
    }
}

void save()
{
    S()->save_text("/data/players/" + name + ".txt",
        stats["hp"] + "\n" + stats["mp"] + "\n" +
        stats["level"] + "\n" + stats["exp"] + "\n");
}

string query_name() { return name; }
mapping query_stats() { return stats; }

void look() { write("You are in Prontera Square. (placeholder map)\n"); }

void show_stats()
{
    write("Name: " + name + "\n");
    write("HP: " + stats["hp"] + "  MP: " + stats["mp"] +
          "  Lv: " + stats["level"] + "  Exp: " + stats["exp"] + "\n");
}

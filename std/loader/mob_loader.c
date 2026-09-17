// std/loader/mob_loader.c — rAthena mob_db.txt loader

private mapping mob_db = ([]);

void load_mob_db(string path)
{
    string content, *lines;
    int i;
    content = read_file(path);
    if (!content) return;
    lines = explode(content, "\n");
    for (i = 0; i < sizeof(lines); i++) {
        string line = lines[i], *f;
        int id;
        if (line == "" || line[0..1] == "//") continue;
        f = explode(line, ",");
        if (sizeof(f) < 8) continue;
        id = to_int(f[0]);
        mob_db[id] = ([
            "id": id, "name": f[1], "key": lower_case(f[1]),
            "level": to_int(f[2]), "hp": to_int(f[3]),
            "atk": to_int(f[4]), "def": to_int(f[5]),
            "base_exp": to_int(f[6]), "job_exp": to_int(f[7]),
            "zeny_min": to_int(f[8]), "zeny_max": to_int(f[9])
        ]);
    }
    debug_message("mob_loader: loaded " + sizeof(mob_db) + " mobs\n");
}

mapping query_mob(int id) { return mob_db[id]; }
mapping query_all_mobs()  { return mob_db; }

mapping query_mob_by_name(string name)
{
    int *ids, i;
    string key = lower_case(name);
    ids = keys(mob_db);
    for (i = 0; i < sizeof(ids); i++)
        if (mob_db[ids[i]]["key"] == key) return mob_db[ids[i]];
    return 0;
}

void create() { load_mob_db("/db/sample/mob_db.txt"); }

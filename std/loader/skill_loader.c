// std/loader/skill_loader.c — rAthena skill_db.txt loader

private mapping skill_db = ([]);

void load_skill_db(string path)
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
        if (sizeof(f) < 7) continue;
        id = to_int(f[0]);
        skill_db[id] = ([
            "id": id, "name": f[1], "key": lower_case(f[1]),
            "job_id": to_int(f[2]), "max_lv": to_int(f[3]),
            "sp_cost": to_int(f[4]), "type": f[5], "power": to_int(f[6])
        ]);
    }
    debug_message("skill_loader: loaded " + sizeof(skill_db) + " skills\n");
}

mapping query_skill(int id) { return skill_db[id]; }
mapping query_all_skills()  { return skill_db; }

mapping query_skill_by_name(string name)
{
    int *ids, i;
    string key = lower_case(name);
    ids = keys(skill_db);
    for (i = 0; i < sizeof(ids); i++)
        if (skill_db[ids[i]]["key"] == key) return skill_db[ids[i]];
    return 0;
}

void create() { load_skill_db("/db/sample/skill_db.txt"); }

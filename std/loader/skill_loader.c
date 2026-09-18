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
            "sp_cost": to_int(f[4]), "type": f[5], "power": to_int(f[6]), "element": to_int(f[7])
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


/* Find the longest skill name that is a prefix of input.
 * Returns ({ skill_id, target_name }) or 0 if no match. */
mixed *find_skill_in_string(string input)
{
    int *ids, i, best_id = -1, best_len = 0;
    string lower_input = lower_case(input);
    ids = keys(skill_db);
    for (i = 0; i < sizeof(ids); i++) {
        string key = skill_db[ids[i]]["key"];
        int klen = strlen(key);
        if (strlen(lower_input) < klen) continue;
        if (lower_input[0..klen-1] != key) continue;
        if (strlen(lower_input) != klen && lower_input[klen] != 32) continue;
        if (klen > best_len) {
            best_len = klen;
            best_id = ids[i];
        }
    }
    if (best_id < 0) return 0;
    if (strlen(input) > best_len + 1)
        return ({ best_id, input[best_len+1..] });
    return ({ best_id, "" });
}

void create() { load_skill_db("/db/sample/skill_db.txt"); }

// std/loader/db_loader.c — rAthena item_db.txt loader

private mapping item_db = ([]);

void load_item_db(string path)
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
        if (sizeof(f) < 10) continue;
        id = to_int(f[0]);
        item_db[id] = ([
            "id": id, "name": f[1], "type": to_int(f[2]),
            "buy": to_int(f[3]), "sell": to_int(f[4]), "weight": to_int(f[5]),
            "atk": f[6] != "" ? to_int(f[6]) : 0,
            "def": f[7] != "" ? to_int(f[7]) : 0,
            "range": to_int(f[8]), "slots": to_int(f[9])
        ]);
    }
    debug_message("db_loader: loaded " + sizeof(item_db) + " items\n");
}

mapping query_item(int id) { return item_db[id]; }
mapping query_all_items() { return item_db; }
int query_item_count() { return sizeof(item_db); }

mapping query_item_by_name(string name)
{
    int *ids, i;
    string key = lower_case(name);
    ids = keys(item_db);
    for (i = 0; i < sizeof(ids); i++)
        if (lower_case(item_db[ids[i]]["name"]) == key) return item_db[ids[i]];
    return 0;
}

void create() { load_item_db("/db/sample/item_db.txt"); }

// std/loader/drop_loader.c — rAthena drop table loader

private mapping drop_db = ([]);

void load_drop_db(string path)
{
    string content, *lines;
    int i;
    content = read_file(path);
    if (!content) return;
    lines = explode(content, "\n");
    for (i = 0; i < sizeof(lines); i++) {
        string line = lines[i], *f;
        int mob, item, rate;
        if (line == "" || line[0..1] == "//") continue;
        f = explode(line, ",");
        if (sizeof(f) < 3) continue;
        mob = to_int(f[0]);
        item = to_int(f[1]);
        rate = to_int(f[2]);
        if (!drop_db[mob]) drop_db[mob] = ({});
        drop_db[mob] += ({ ([ "item_id": item, "rate": rate ]) });
    }
    debug_message("drop_loader: loaded drops for " + sizeof(drop_db) + " mobs\n");
}

mapping *query_drops(int mob_id) { return drop_db[mob_id]; }

void create() { load_drop_db("/db/sample/drop_db.txt"); }

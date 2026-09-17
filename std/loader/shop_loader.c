// std/loader/shop_loader.c — shop catalog loader

private mapping shop_db = ([]);

void load_shop_db(string path)
{
    string content, *lines;
    int i;
    content = read_file(path);
    if (!content) return;
    lines = explode(content, "\n");
    for (i = 0; i < sizeof(lines); i++) {
        string line = lines[i], *f;
        string key;
        int item, price;
        if (line == "" || line[0..1] == "//") continue;
        f = explode(line, ",");
        if (sizeof(f) < 3) continue;
        key = f[0];
        item = to_int(f[1]);
        price = to_int(f[2]);
        if (!shop_db[key]) shop_db[key] = ({});
        shop_db[key] += ({ ([ "item_id": item, "price": price ]) });
    }
    debug_message("shop_loader: loaded " + sizeof(shop_db) + " shops\n");
}

mapping *query_shop(string key) { return shop_db[key]; }

void create() { load_shop_db("/db/sample/shop_db.txt"); }

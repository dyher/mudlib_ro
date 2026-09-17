// db_loader.c — rAthena item_db.txt loader
// Reads rAthena CSV format and provides query interface

private mapping item_db = ([]);

// rAthena item_db.txt format:
// ID,Name,Type,Buy,Sell,Weight,ATK,DEF,Range,Slots,Job,Upper,Gender,Loc,wLV,eLV,View,Script

void load_item_db(string path)
{
    string content;
    string *lines;
    int i;
    
    content = read_file(path);
    if (!content) {
        debug_message("db_loader: failed to read " + path + "\n");
        return;
    }
    
    lines = explode(content, "\n");
    
    for (i = 0; i < sizeof(lines); i++) {
        string line = lines[i];
        string *fields;
        int id;
        
        // Skip comments and empty lines
        if (line == "" || line[0..1] == "//") continue;
        
        // Parse CSV (simplified - doesn't handle quoted fields with commas)
        fields = explode(line, ",");
        
        if (sizeof(fields) < 10) continue;
        
        id = to_int(fields[0]);
        
        item_db[id] = ([
            "id": id,
            "name": fields[1],
            "type": to_int(fields[2]),
            "buy": to_int(fields[3]),
            "sell": to_int(fields[4]),
            "weight": to_int(fields[5]),
            "atk": to_int(fields[6]),
            "def": fields[7] != "" ? to_int(fields[7]) : 0,
            "range": to_int(fields[8]),
            "slots": to_int(fields[9])
        ]);
    }
    
    debug_message("db_loader: loaded " + sizeof(item_db) + " items from " + path + "\n");
}

mapping query_item(int id)
{
    return item_db[id];
}

mapping query_all_items()
{
    return item_db;
}

int query_item_count()
{
    return sizeof(item_db);
}

void create()
{
    load_item_db("/db/sample/item_db.txt");
}

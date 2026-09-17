// iteminfo.c — Query rAthena item info from db_loader

int main(string arg)
{
    object loader;
    mapping item;
    int id;
    
    if (!arg || arg == "") {
        write("Usage: iteminfo <item_id>\n");
        return 1;
    }
    
    id = to_int(arg);
    loader = find_object("/std/loader/db_loader");
    
    if (!loader) {
        write("db_loader not loaded.\n");
        return 1;
    }
    
    item = loader->query_item(id);
    
    if (!item) {
        write("Item ID " + id + " not found.\n");
        return 1;
    }
    
    write("=== Item Info ===\n");
    write("ID: " + item["id"] + "\n");
    write("Name: " + item["name"] + "\n");
    write("Type: " + item["type"] + "\n");
    write("Buy: " + item["buy"] + "\n");
    write("Sell: " + item["sell"] + "\n");
    write("Weight: " + item["weight"] + "\n");
    write("ATK: " + item["atk"] + "\n");
    write("DEF: " + item["def"] + "\n");
    write("Range: " + item["range"] + "\n");
    write("Slots: " + item["slots"] + "\n");
    
    return 1;
}

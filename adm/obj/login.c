// login.c — Minimal login for RO mudlib (uses input_to for line reading)

void logon()
{
    object loader;
    
    write("\n");
    write("====================================\n");
    write(" Neolith RO Mudlib (rAthena compat)\n");
    write("====================================\n");
    
    loader = find_object("/std/loader/db_loader");
    if (!loader)
        loader = call_other("/adm/obj/master", "load_object_as_root", "/std/loader/db_loader");
    
    if (loader) write("db_loader ready: " + loader->query_item_count() + " items.\n");
    else        write("db_loader unavailable.\n");
    
    write("\nType 'iteminfo <id>' to query items.\n");
    write("Example: iteminfo 1201\n");
    write("\n> ");
    input_to("process_input");
}

// Called by driver with the next line the user types
void process_input(string line)
{
    string cmd, arg;
    
    if (!line || line == "") { write("\n> "); input_to("process_input"); return; }
    
    if (sscanf(line, "%s %s", cmd, arg) != 2) { cmd = line; arg = ""; }
    
    if (cmd == "quit" || cmd == "exit") {
        write("Goodbye!\n");
        destruct(this_object());
        return;
    }
    
    if (cmd == "iteminfo") {
        call_other("/cmds/iteminfo", "main", arg);
    } else {
        write("Unknown command: " + cmd + "\n");
    }
    
    write("\n> ");
    input_to("process_input");
}

void net_dead() { destruct(this_object()); }

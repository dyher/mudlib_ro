// master.c — Minimal master for RO mudlib

string get_root_uid() { return "Root"; }
string get_bb_uid()   { return "Backbone"; }

object connect(int port)
{
    object login_ob;
    mixed err;
    
    debug_message("RO master: connect() port=" + port + "\n");
    
    err = catch(login_ob = clone_object("/adm/obj/login"));
    if (err) { write("Login failed: " + err + "\n"); return 0; }
    
    export_uid(login_ob);
    return login_ob;
}

// Master has Root euid, so it can create objects on behalf of login
// Load a room/map object (master has Root euid)
object load_room(string path)
{
    return load_object(path);
}

// Load an NPC object (master has Root euid)
object load_npc(string path)
{
    return load_object(path);
}

object create_player_for(string char_name)
{
    object p = new("/std/object/player");
    if (p) p->setup(char_name);
    return p;
}


// Driver calls epilog() at boot to get list of objects to preload
string *epilog()
{
    return ({
        "/std/system/storage",
        "/std/system/accountd",
        "/std/loader/db_loader",
        "/std/loader/job_loader",
        "/std/loader/mob_loader",
        "/std/loader/skill_loader",
        "/std/loader/drop_loader",
        "/std/loader/shop_loader",
        "/std/system/game_lib",
        "/std/system/npcscript",
        "/adm/obj/updater",
        "/std/object/player",
        "/cmds/iteminfo"
    });
}

// Driver calls preload(file) for each entry; master has Root euid here
void preload(string file)
{
    mixed err;
    err = catch(load_object(file));
    if (err) debug_message("preload FAILED " + file + ": " + err + "\n");
    else     debug_message("preloaded " + file + " (euid:" + geteuid() + ")\n");
}

// Fallback: let any object ask master (which has euid) to load something
object load_object_as_root(string file)
{
    return load_object(file);
}

void log_error(string file, string message)
{
    debug_message("COMPILE ERROR in " + file + ":\n" + message + "\n");
}

mixed compile_object(string file) { return 0; }

// master.c — Minimal master for RO mudlib

string get_root_uid() { return "Root"; }
string get_bb_uid()   { return "Backbone"; }

string simul_efun_object()
{
    return "/adm/obj/simul_efun";
}

object connect(int port)
{
    object ob;
    
    // 路由 6900 端口到 RO 二進制協議處理器
    if (port == 6900) {
        ob = new("/obj/ro/login");
        return ob;
    }
    
    // 路由 8080 端口到 HTTP API 處理器
    if (port == 8081) {
        return new("/adm/obj/ws_api");
    }
    if (port == 8080) {
        ob = new("/adm/obj/http_api");
        return ob;
    }
    
    // 預設 Telnet 端口 (例如 5001)
    ob = new("/std/object/player");
    return ob;
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
        "/cmds/iteminfo",
        "/adm/obj/test_sqlite",
        "/adm/obj/test_crypto"
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

// ES2 style: determine the UID for newly created objects
// This ensures login objects and other critical objects get Root UID
string creator_file(string file)
{
    // All objects get Root UID for our simple RO architecture
    return "Root";
}

string privileged_file(string file)
{
    if (file[0..4] == "/adm/") return "Root";
    return 0;
}

string object_name(string file)
{
    return file;
}

// ES2 style: allow objects to set their effective UID
int valid_seteuid(object ob, string euid)
{
    return 1;  // allow all seteuid operations for our simple RO architecture
}

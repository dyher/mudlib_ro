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

// Driver calls epilog() at boot to get list of objects to preload
string *epilog()
{
    return ({
        "/std/loader/db_loader",
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

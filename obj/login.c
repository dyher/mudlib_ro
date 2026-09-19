// obj/login.c - ES2 style login object
// This is the FIRST interactive object - holds the connection initially.
// After login completes, exec() transfers interactive status to the user body.

inherit "/feature/save";

static object my_body;

void logon();
void net_dead();

object body() { return my_body; }

void set_body(object body)
{
    if (geteuid(previous_object()) != "Root") return 0;
    my_body = body;
}

void logon()
{
    object logind;
    seteuid(getuid());
    logind = load_object("/adm/daemons/logind");
    if (logind) {
        logind->logon(this_object());
    } else {
        write("Login system unavailable.\n");
        destruct(this_object());
    }
}

void net_dead()
{
    if (my_body) my_body->net_dead();
    destruct(this_object());
}

void receive_message(string type, string str)
{
    if (!interactive(this_object())) return;
    if (str) receive(str);
}

nomask string query_save_file()
{
    string id = query("id");
    if (!stringp(id)) return 0;
    return "/data/users/" + id[0..0] + "/" + id;
}

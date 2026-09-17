// std/system/accountd.c — account management (preloaded)

string acc_path(string n) { return "/data/accounts/" + n + ".txt"; }

object S() { return find_object("/std/system/storage"); }

int exists_account(string n)
{
    return S()->exists(acc_path(n)) >= 0;
}

int create_account(string n, string p)
{
    if (exists_account(n)) return 0;
    return S()->save_text(acc_path(n), p + "\n\n");
}

int check_account(string n, string p)
{
    string c = S()->load_text(acc_path(n));
    string *lines;
    if (!c) return 0;
    lines = explode(c, "\n");
    return (sizeof(lines) > 0 && lines[0] == p);
}

string query_char(string n)
{
    string c = S()->load_text(acc_path(n));
    string *lines;
    if (!c) return 0;
    lines = explode(c, "\n");
    if (sizeof(lines) > 1 && lines[1] != "") return lines[1];
    return 0;
}

int set_char(string n, string ch)
{
    string c = S()->load_text(acc_path(n));
    string *lines;
    if (!c) return 0;
    lines = explode(c, "\n");
    return S()->save_text(acc_path(n), lines[0] + "\n" + ch + "\n");
}

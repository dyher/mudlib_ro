// adm/obj/simul_efun.c - Standard simul_efuns

// base_name: returns the file name of an object without the clone suffix (#xxx)
string base_name(mixed ob) {
    string name;
    int i;
    if (stringp(ob)) return ob;
    if (!objectp(ob)) return 0;
    name = file_name(ob);
    i = strsrch(name, "#");
    if (i != -1) name = name[0..i-1];
    return name;
}

// assure_file: ensure the directory path for a file exists
void assure_file(string file) {
    string dir;
    string *parts;
    string current;
    int i;

    if (!file || file == "") return;

    // 找出最後一個 / 的位置
    i = strsrch(file, "/", -1);
    if (i <= 0) return; // 沒有目錄，或在根目錄

    dir = file[0..i-1];
    if (dir == "") return;

    // 遞迴建立目錄
    parts = explode(dir, "/");
    current = "";
    foreach(string p in parts) {
        if (p == "") { current = "/"; continue; } // 處理絕對路徑開頭的 /
        if (current == "/" || current == "") current += p;
        else current += "/" + p;

        // file_size 回傳 -2 代表是目錄，-1 代表不存在
        if (file_size(current) == -2) continue; 
        mkdir(current);
    }
}

// lower_case: convert string to lowercase
string lower_case(string str) {
    int i, n, c;
    string r;
    if (!stringp(str)) return str;
    n = strlen(str);
    r = "";
    for (i = 0; i < n; i++) {
        c = str[i];
        if (c >= 'A' && c <= 'Z') c += 32;
        r += sprintf("%c", c);
    }
    return r;
}




// message: ES2 standard message broadcasting simul_efun
void message(string msg_class, string msg, mixed target, object avoid)
{
    object *targets;
    object *inv;
    object ob;
    object login_obj;
    int i;
    
    if (stringp(target)) {
        ob = find_object(target);
        if (ob) target = ob;
        else return;
    }
    
    if (objectp(target)) {
        if (target->is_player()) {
            if (target != avoid) {
                login_obj = target->query("login_obj");
                if (objectp(login_obj)) tell_object(login_obj, msg);
                else tell_object(target, msg);
            }
        } else {
            inv = all_inventory(target);
            for (i = 0; i < sizeof(inv); i++) {
                if (inv[i] != avoid && inv[i]->is_player()) {
                    login_obj = inv[i]->query("login_obj");
                    if (objectp(login_obj)) tell_object(login_obj, msg);
                    else tell_object(inv[i], msg);
                }
            }
        }
    } else if (pointerp(target)) {
        targets = target;
        for (i = 0; i < sizeof(targets); i++) {
            if (objectp(targets[i]) && targets[i] != avoid && targets[i]->is_player()) {
                login_obj = targets[i]->query("login_obj");
                if (objectp(login_obj)) tell_object(login_obj, msg);
                else tell_object(targets[i], msg);
            }
        }
    }
}

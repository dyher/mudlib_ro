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

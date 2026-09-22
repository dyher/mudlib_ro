// /std/system/accountd.c - 帳號管理 Daemon
#define DB_PATH "/tmp/ro_accounts.db"

void create() {
    int db = sqlite3_open(DB_PATH);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT)");
    sqlite3_close(db);
    debug_message("ACCOUNTD: ✅ Initialized (table created)\n");
}

int register_account(string user, string pass) {
    int db = sqlite3_open(DB_PATH);
    string sql = "INSERT INTO accounts (username, password) VALUES ('" + user + "', '" + pass + "')";
    mixed err = catch(sqlite3_exec(db, sql));
    sqlite3_close(db);
    return !err;
}

int verify_account(string user, string pass) {
    int db = sqlite3_open(DB_PATH);
    string sql = "SELECT password FROM accounts WHERE username='" + user + "'";
    // sqlite3_exec 現在對 SELECT 會返回編碼結果字串！
    mixed res = sqlite3_exec(db, sql);
    sqlite3_close(db);
    
    debug_message("ACCOUNTD: verify [" + user + "] -> res: [" + res + "]\n");
    
    if (!stringp(res) || res == "ERROR" || res == "") return 0;
    
    // res 格式: "password\nactual_password"
    string *lines = explode(res, "\n");
    if (sizeof(lines) > 1) {
        return (lines[1] == pass);
    }
    return 0;
}

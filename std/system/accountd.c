// 強制宣告底層 Efun，繞過 LPC 詞法分析器的黑盒
string sqlite3_query(int, string);

// /std/system/accountd.c
#define DB_PATH "/tmp/ro_accounts.db"

void create() {
    int db = sqlite3_open(DB_PATH);
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, username TEXT UNIQUE, password TEXT)");
    sqlite3_close(db);
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
    string res = sqlite3_query(db, sql);
    sqlite3_close(db);
    
    if (res == "ERROR" || res == "") return 0;
    
    // res 格式: "password\ntarget_password"
    string *lines = explode(res, "\n");
    if (sizeof(lines) > 1) {
        return (lines[1] == pass);
    }
    return 0;
}

void create() {
    object db;
    mixed err;
    
    debug_message("DB: Opening database...\n");
    err = catch(db = sqlite3_open("/tmp/test.db"));
    if (err) {
        debug_message("DB: Open failed: " + err + "\n");
        return;
    }
    
    debug_message("DB: Creating table...\n");
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS accounts (id INTEGER PRIMARY KEY, user TEXT, pass TEXT)");
    
    debug_message("DB: Inserting data...\n");
    sqlite3_exec(db, "INSERT INTO accounts (user, pass) VALUES ('admin', 'password')");
    
    debug_message("DB: Closing...\n");
    sqlite3_close(db);
    
    debug_message("DB: ✅ Test completed!\n");
}

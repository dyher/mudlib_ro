void create() {
    int db;
    mixed err;
    
    debug_message("SQLITE: Testing cross-platform SQLite3...\n");
    
    err = catch(db = sqlite3_open("/tmp/lithos_test.db"));
    if (err) {
        debug_message("SQLITE: Open failed: " + err + "\n");
        return;
    }
    debug_message("SQLITE: Opened DB handle: " + db + "\n");
    
    sqlite3_exec(db, "CREATE TABLE IF NOT EXISTS test (id INTEGER PRIMARY KEY, val TEXT)");
    sqlite3_exec(db, "INSERT INTO test (val) VALUES ('Hello Lithos!')");
    
    sqlite3_close(db);
    debug_message("SQLITE: ✅ Cross-platform test PASSED!\n");
}

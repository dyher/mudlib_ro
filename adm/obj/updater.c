// adm/obj/updater.c — hot-reload manager
// 讓遊戲運行中就能更新資料和物件，不用重啟伺服器。

void reload_all_data()
{
    write("Reloading all data files...\n");
    find_object("/std/loader/db_loader")->load_item_db("/db/sample/item_db.txt");
    find_object("/std/loader/job_loader")->load_job_db("/db/sample/job_db.txt");
    find_object("/std/loader/skill_loader")->load_skill_db("/db/sample/skill_db.txt");
    find_object("/std/loader/mob_loader")->load_mob_db("/db/sample/mob_db.txt");
    find_object("/std/loader/drop_loader")->load_drop_db("/db/sample/drop_db.txt");
    find_object("/std/loader/shop_loader")->load_shop_db("/db/sample/shop_db.txt");
    write("All data reloaded!\n");
}

void update_object(string path)
{
    object ob;
    write("Updating " + path + "...\n");
    ob = find_object(path);
    if (ob) {
        destruct(ob);
        write("  Destructed old object.\n");
    }
    ob = load_object(path);
    if (ob) write("  Reloaded successfully.\n");
    else write("  Failed to reload!\n");
}

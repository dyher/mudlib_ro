// std/loader/job_loader.c — rAthena job_db.txt loader

private mapping job_db = ([]);

void load_job_db(string path)
{
    string content, *lines;
    int i;
    content = read_file(path);
    if (!content) return;
    lines = explode(content, "\n");
    for (i = 0; i < sizeof(lines); i++) {
        string line = lines[i], *f;
        int id;
        if (line == "" || line[0..1] == "//") continue;
        f = explode(line, ",");
        if (sizeof(f) < 10) continue;
        id = to_int(f[0]);
        job_db[id] = ([
            "id": id, "name": f[1],
            "base_hp": to_int(f[2]), "base_sp": to_int(f[3]),
            "str": to_int(f[4]), "agi": to_int(f[5]), "vit": to_int(f[6]),
            "int": to_int(f[7]), "dex": to_int(f[8]), "luk": to_int(f[9])
        ]);
    }
    debug_message("job_loader: loaded " + sizeof(job_db) + " jobs\n");
}

mapping query_job(int id)      { return job_db[id]; }
mapping query_all_jobs()       { return job_db; }
int query_job_count()          { return sizeof(job_db); }

void create() { load_job_db("/db/sample/job_db.txt"); }

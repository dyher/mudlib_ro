// std/system/storage.c — preloaded file I/O gateway (runs with Root euid)

int save_text(string path, string content)
{
    rm(path);
    return write_file(path, content);
}

string load_text(string path)
{
    return read_file(path);
}

int exists(string path)
{
    return file_size(path);
}

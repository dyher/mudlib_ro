void create() {
    string key = "dGhlIHNhbXBsZSBub25jZQ==";
    string magic = "258EAFA5-E914-47DA-95CA-C5AB0DC85B11";
    string sha1_res, b64_res;
    
    debug_message("CRYPTO: Testing SHA1 and Base64...\n");
    
    // 測試 SHA1
    mixed err1 = catch(sha1_res = sha1(key + magic));
    if (err1) debug_message("CRYPTO: SHA1 failed: " + err1 + "\n");
    else debug_message("CRYPTO: SHA1 OK (len: " + sizeof(sha1_res) + ")\n");
    
    // 測試 Base64
    mixed err2 = catch(b64_res = base64_encode(sha1_res));
    if (err2) debug_message("CRYPTO: Base64 failed: " + err2 + "\n");
    else debug_message("CRYPTO: Base64 OK: " + b64_res + "\n");
}

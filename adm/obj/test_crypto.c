void create() {
    string key = "dGhlIHNhbXBsZSBub25jZQ==";
    string accept;
    
    debug_message("CRYPTO: Testing websocket_accept()...\n");
    
    mixed err = catch(accept = websocket_accept(key));
    if (err) debug_message("CRYPTO: Failed: " + err + "\n");
    else debug_message("CRYPTO: ✅ WebSocket Accept: " + accept + "\n");
    
    debug_message("CRYPTO: Testing sha1()...\n");
    string hex = sha1("hello");
    debug_message("CRYPTO: ✅ SHA1('hello'): " + hex + "\n");
}

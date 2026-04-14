<h1>=BTC= JSON Extension</h1>
<hr></hr>
<h2>Provides a .dll for Windows and .so for Linux</h2>
ArmaToJSON is an Arma 3 extension (.dll) that gives SQF scripts the ability to read, write, and manage files on the local filesystem using JSON as the data format. It bridges the gap between Arma's scripting environment and the host machine's file system.

What it does

Through callExtension, SQF code can call the following operations:

* getFile — read a file and return its content as a JSON string

* createFile — create a new file with specified content

* copyFile — copy a file from one path to another

* deleteFile — delete a file at a given path

* renameFile — rename or move a file

* retrieveList — list files and directories at a given path

Large outputs are handled automatically via a chunked storage system. If the response exceeds Arma's output buffer limit which as of >2.20 is 20480 bytes, the extension splits it into indexed chunks that SQF retrieves sequentially.

When to use it

Use this extension when your mission or mod needs to:

* Persist data between sessions (player stats, loadouts, logs) without a database or web server

* Read configuration files from disk at runtime

* Write structured output from SQF to disk for external tools or scripts to consume

* Interface with other local software that reads/writes files

The current API allows you to:

* <b>btc_json_fnc_createFile</b> Generate a file by passing [PATH:STRING, DATA:ANY] as arguments, will save whatever is in data, provided it's a json compatible format or a hashmap, in path, example:
<code>["./armatojson/test.json", createHashMap] call btc_json_fnc_createFile;</code> 
will save an empty JSON object to ./armatojson/test.json

* <b>btc_json_fnc_retrieveList</b> Retrieves the list of JSON files recursively starting  from [PATH:STRING].
<code>["."] call btc_json_fnc_retrieveList</code>
Will retrieve all JSON files starting from Arma 3 executable root and will recursively search all folders.

* <b>btc_json_fnc_copyFile</b> Copies JSON file in [PATH:STRING] to [NEWPATH:STRING].
<code>["./armatojson/test.json", "./armatojson/test_copy.json"] call btc_json_fnc_copyFile</code>
Will copy test.json to ./armatojson/test_copy, WARNING copy CAN overwrite.

* <b>btc_json_fnc_deleteFile</b> Deletes JSON file in [PATH:STRING].
<code>["./armatojson/test.json"] call btc_json_fnc_deleteFile</code>
Will delete test.json

* <b>btc_json_fnc_getFile</b> Retrieves JSON file in [PATH:STRING].
<code>["./armatojson/test.json"] call btc_json_fnc_getFile</code>
Will retrieve test.json and return it as an hashmap

* <b>btc_json_fnc_renameFile</b> Renames JSON file in [PATH:STRING] to [NEWPATH:STRING].
<code>["./armatojson/test.json", "./armatojson/test_rename.json"] call btc_json_fnc_renameFile</code>
Will rename test.json to test_rename.json

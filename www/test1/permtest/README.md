# Permission Test Guide (GET / autoindex)

This directory contains files and subdirectories created specifically to test
HTTP GET permission handling in webserv.

All tests should be executed using curl, since it exposes HTTP status codes
and headers, which browsers may hide.

The server is assumed to be running with:
./webserv config/default.conf


------------------------------------------------------------
## 1) File permission tests
------------------------------------------------------------

Test how the server behaves when serving regular files with different chmod values.

Commands:
```

curl -i http://localhost:8080/test1/permtest/files/f666.txt

curl -i http://localhost:8080/test1/permtest/files/f644.txt

curl -i http://localhost:8080/test1/permtest/files/f444.txt

curl -i http://localhost:8080/test1/permtest/files/f222.txt

curl -i http://localhost:8080/test1/permtest/files/f200.txt

curl -i http://localhost:8080/test1/permtest/files/f000.txt
```


Expected results:
```

File        chmod   Expected HTTP status
----------------------------------------
f666.txt    666     200          OK
f644.txt    644     200          OK
f444.txt    444     200          OK
f222.txt    222     403      Forbidden
f200.txt    200     403      Forbidden
f000.txt    000     403      Forbidden
```


Interpretation:
- If any readable file (666, 644, 444) returns 403, the read-permission logic is wrong.
- If any non-readable file (222, 200, 000) returns 200, the server is leaking forbidden files.


------------------------------------------------------------
## 2) Directory redirect behavior (trailing slash)
------------------------------------------------------------


Directories must be accessed with a trailing '/' so that relative links work correctly.

Command:
```
curl -i http://localhost:8080/test1/permtest/dirs/d755
```


Expected result:
```

HTTP/1.1 301 Moved Permanently
Location: /test1/permtest/dirs/d755/
```

This confirms correct URL normalization for directories.


------------------------------------------------------------
## 3) Directory autoindex tests
------------------------------------------------------------

Allowed directory (listable + traversable):
```
curl -i http://localhost:8080/test1/permtest/dirs/d755/
```

Expected:
```
HTTP/1.1 200 OK
Content-Type: text/html; charset=utf-8
```
An HTML directory listing should be returned.


Forbidden directory cases:
```
curl -i http://localhost:8080/test1/permtest/dirs/d311/

curl -i http://localhost:8080/test1/permtest/dirs/d666/

curl -i http://localhost:8080/test1/permtest/dirs/d644/

curl -i http://localhost:8080/test1/permtest/dirs/d600/
```

Expected results:
```

Directory   chmod   Reason                                   Expected
----------------------------------------------------------------------
d311        311     Traversable but not listable (X only)    403
d666        666     No execute permission                    403
d644        644     No execute permission                    403
d600        600     No execute permission                    403
```
##
### Here’s a copy-paste bash script that creates a test tree with each chmod case (files + directories) and prints the exact curl commands to run.

It assumes your server root for that location resolves to something like www/test1/ (as in our config). Adjust BASE if needed.

```#!/usr/bin/env bash
set -euo pipefail

# Adjust this to match your filesystem root used by location /test1
BASE="www/test1/permtest"

echo "[+] Creating test tree in: $BASE"
rm -rf "$BASE"
mkdir -p "$BASE"
echo "OK" > "$BASE/README.txt"

###############################################################################
# FILE CASES
###############################################################################
mkdir -p "$BASE/files"

make_file() {
  local name="$1"
  local mode="$2"
  local path="$BASE/files/$name"
  printf "content for %s\n" "$name" > "$path"
  chmod "$mode" "$path"
  echo "  - file: $path (chmod $mode)"
}

echo "[+] Creating file permission cases"
make_file "f666.txt" 666
make_file "f644.txt" 644
make_file "f444.txt" 444
make_file "f222.txt" 222
make_file "f200.txt" 200
make_file "f000.txt" 000

###############################################################################
# DIRECTORY CASES (autoindex / traversal)
###############################################################################
mkdir -p "$BASE/dirs"

make_dir() {
  local name="$1"
  local mode="$2"
  local dir="$BASE/dirs/$name"
  mkdir -p "$dir"
  echo "inside $name" > "$dir/inside.txt"
  chmod "$mode" "$dir"
  echo "  - dir : $dir (chmod $mode)"
}

echo "[+] Creating directory permission cases"
make_dir "d755" 755   # list + traverse OK (autoindex should work)
make_dir "d311" 311   # traverse OK, list not OK (autoindex should be 403)
make_dir "d700" 700   # owner only (depends on user running server; usually OK for same user)
make_dir "d666" 666   # no X -> cannot enter (403)
make_dir "d644" 644   # no X -> cannot enter (403)
make_dir "d600" 600   # no X -> cannot enter (403)

###############################################################################
# PRINT TEST COMMANDS
###############################################################################
echo
echo "============================================================"
echo "Now run these curls (adjust URL path if your location differs)"
echo "============================================================"
echo
echo "# FILES (expect 200 for readable, 403 for unreadable)"
echo "curl -i http://localhost:8080/test1/permtest/files/f666.txt"
echo "curl -i http://localhost:8080/test1/permtest/files/f644.txt"
echo "curl -i http://localhost:8080/test1/permtest/files/f444.txt"
echo "curl -i http://localhost:8080/test1/permtest/files/f222.txt"
echo "curl -i http://localhost:8080/test1/permtest/files/f200.txt"
echo "curl -i http://localhost:8080/test1/permtest/files/f000.txt"
echo
echo "# DIRECTORIES (autoindex outcomes)"
echo "# d755: should show HTML listing (200) when requesting with trailing slash"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d755/"
echo
echo "# d711: traverse OK but listing not allowed -> should be 403 (autoindex)"
echo "curl -i http://localhost:8080/test1/permtest/dirs/311/"
echo
echo "# d700: depends if server runs as same user who owns the dir"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d700/"
echo
echo "# d666/d644/d600: no execute -> should be 403 (cannot traverse)"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d666/"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d644/"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d600/"
echo
echo "# Redirect behavior test (should be 301 to add slash)"
echo "curl -i http://localhost:8080/test1/permtest/dirs/d755"
echo
echo "[+] Done."
```

Notes:
- Restore permissions locally (optional but recommended) so you can keep working comfortably:
```
chmod -R u+rwx www/test1/permtest
```
- These tests assume the server is running as the directory owner.
- Directory listing (autoindex) requires both read (R) and execute (X) permissions.
- curl is preferred over browsers because it shows

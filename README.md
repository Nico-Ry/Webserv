# webserv


webserv/  
├─ README.md  
├─ LICENSE  
├─ Makefile  
├─ .gitignore
├─ .clang-format                 # optional, C++98-friendly style (no features beyond C++98)
├─ include/                      # public headers
│  ├─ webserv.hpp
│  ├─ core/
│  │  ├─ Server.hpp
│  │  ├─ EventLoop.hpp          # select/poll/epoll/kqueue abstraction
│  │  ├─ Socket.hpp
│  │  └─ Client.hpp
│  ├─ http/
│  │  ├─ Request.hpp
│  │  ├─ Response.hpp
│  │  ├─ Router.hpp
│  │  └─ Status.hpp
│  ├─ config/
│  │  ├─ Config.hpp
│  │  └─ Route.hpp
│  ├─ cgi/
│  │  ├─ Cgi.hpp
│  │  └─ Env.hpp
│  └─ util/
│     ├─ Buffer.hpp
│     ├─ File.hpp
│     ├─ String.hpp
│     └─ Logger.hpp
├─ src/                          # implementation (.cpp only, C++98)
│  ├─ main.cpp
│  ├─ core/
│  ├─ http/
│  ├─ config/
│  ├─ cgi/
│  └─ util/
├─ config/                       # sample configuration files used in defense
│  ├─ default.conf
│  ├─ uploads.conf
│  └─ cgi_php.conf
├─ www/                          # static website(s) used for demos
│  ├─ index.html
│  ├─ assets/
│  └─ error/                     # default error pages (40x, 50x)
├─ cgi-bin/                      # simple CGI scripts for tests
│  ├─ hello.py
│  └─ echo.py
├─ uploads/                      # target directory for POST uploads (gitkeep only)
│  └─ .gitkeep
├─ tests/                        # black-box tests & stress tests
│  ├─ blackbox/
│  │  ├─ test_get.py
│  │  ├─ test_post_upload.py
│  │  ├─ test_delete.py
│  │  └─ test_cgi.py
│  ├─ tools/
│  │  ├─ bombard.py              # simple concurrency/stress script
│  │  └─ compare_with_nginx.md   # notes/how-to for header/behavior comparison
│  └─ data/
│     └─ large_body.bin
├─ tools/                        # developer helpers (not part of the binary)
│  ├─ run.sh                     # ./run.sh config/default.conf
│  ├─ valgrind.sh                # memory checks (Linux)
│  └─ gen_conf_minimal.py        # tiny config generator for quick tests
└─ .github/
   └─ workflows/
      └─ ci.yml                  # builds + runs a tiny set of black-box tests

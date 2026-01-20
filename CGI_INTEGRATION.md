# CGI Integration Guide

Le CGI Handler est maintenant implémenté et prêt à être intégré dans votre serveur!

## Structure créée

```
include/cgi/
└── CgiHandler.hpp          # Interface CGI Handler

src/cgi/
└── CgiHandler.cpp          # Implémentation complète

cgi-bin/                    # Scripts CGI de test
├── hello.py               # Simple page de salutation
├── env.py                 # Affiche toutes les variables d'environnement
├── form.py                # Formulaire avec GET/POST
└── time.py                # Affiche l'heure du serveur
```

## Fonctionnalités implémentées

✅ **Fork/exec** - Exécution de scripts dans un processus enfant
✅ **Pipes stdin/stdout** - Communication avec le script CGI
✅ **Variables d'environnement CGI** - REQUEST_METHOD, PATH_INFO, QUERY_STRING, HTTP_*, etc.
✅ **Support POST** - Le body de la requête est passé en stdin
✅ **Parsing de sortie CGI** - Headers (Status, Content-Type) + body
✅ **Timeout** - Protection contre les scripts qui ne répondent pas (30s par défaut)
✅ **Gestion d'erreurs** - 404, 500, 502, 504 avec pages HTML
✅ **Auto-détection interpréteur** - Python, PHP, Perl, Shell, Ruby

## Comment intégrer dans Server.cpp (TEMPORAIRE)

Tant que le Router n'est pas complété par tes mates, tu peux tester directement depuis `Server::handleHttpRequest()`:

### Option 1: Test basique dans Server.cpp

```cpp
// Dans src/network/Server.cpp
#include "cgi/CgiHandler.hpp"  // Ajouter en haut

HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
    // TEST CGI: Si le path commence par /cgi-bin/, utiliser CGI Handler
    if (req.path.find("/cgi-bin/") == 0) {
        // Construire le chemin vers le script
        // /cgi-bin/hello.py → ./cgi-bin/hello.py
        std::string scriptPath = "." + req.path;

        return CgiHandler::execute(req, scriptPath);
    }

    // Sinon, routing normal (HTML hardcodé existant)
    HttpResponse resp(200, "OK");
    // ... code existant ...
    return resp;
}
```

### Option 2: Test plus complet avec configuration

```cpp
HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
    // Détecter si c'est un script CGI
    if (CgiHandler::isCgiScript(req.path)) {
        std::string scriptPath = "." + req.path;

        // Auto-détection de l'interpréteur
        std::string interpreter = CgiHandler::detectInterpreter(scriptPath);

        if (!interpreter.empty()) {
            return CgiHandler::execute(req, scriptPath, interpreter, 30);
        } else {
            // Pas d'interpréteur trouvé
            HttpResponse resp(500, "Internal Server Error");
            resp.headers["Content-Type"] = "text/html";
            resp.body = "<h1>500 - No interpreter found</h1>";
            return resp;
        }
    }

    // Routing normal...
}
```

## Tester les scripts CGI

### 1. Compiler le serveur

```bash
make re
```

### 2. Lancer le serveur

```bash
./webserv config/default.conf
```

### 3. Tester dans le navigateur

```
http://localhost:8080/cgi-bin/hello.py
http://localhost:8080/cgi-bin/env.py
http://localhost:8080/cgi-bin/time.py
http://localhost:8080/cgi-bin/form.py
```

### 4. Tester avec curl

```bash
# GET simple
curl http://localhost:8080/cgi-bin/hello.py

# GET avec query string
curl "http://localhost:8080/cgi-bin/form.py?name=Alice&message=Hello"

# POST avec body
curl -X POST -d "name=Bob&message=Test POST" http://localhost:8080/cgi-bin/form.py

# Voir les variables d'environnement
curl http://localhost:8080/cgi-bin/env.py
```

## Intégration future avec Router (quand tes mates auront fini)

Une fois que le Router sera implémenté, l'intégration se fera proprement:

```cpp
// Dans Router::handleHttpRequest() - src/router/Router.cpp

HttpResponse Router::handleHttpRequest(const HttpRequest& req) {
    // 1. Matcher le path avec LocationBlock
    LocationBlock* location = matchLocation(req.path);

    // 2. Vérifier méthode autorisée
    if (!isMethodAllowed(req.method, location)) {
        return ErrorHandler::methodNotAllowed(405);
    }

    // 3. Dispatcher vers le bon handler

    // Si c'est un script CGI
    if (CgiHandler::isCgiScript(req.path)) {
        std::string scriptPath = resolvePath(location->root, req.path);
        std::string interpreter = location->cgiInterpreter;  // depuis config
        return CgiHandler::execute(req, scriptPath, interpreter);
    }

    // Si c'est un fichier statique
    if (req.method == METHOD_GET) {
        return FileHandler::serveFile(req.path, location);
    }

    // Si c'est un upload
    if (req.method == METHOD_POST && req.path == "/upload") {
        return UploadHandler::handle(req, location);
    }

    // Sinon 404
    return ErrorHandler::notFound(404);
}
```

## Configuration CGI dans le fichier de config

Pour que tes mates sachent comment intégrer, voici un exemple de config:

```nginx
server {
    listen 8080;
    root ./www;

    # Location pour les scripts CGI
    location /cgi-bin {
        root ./cgi-bin;
        methods GET POST;
        cgi_extension .py .php .pl;
        cgi_interpreter /usr/bin/python3;  # Par défaut pour .py
    }

    # Locations normales
    location / {
        methods GET POST DELETE;
    }
}
```

## Créer tes propres scripts CGI

### Script Python minimal

```python
#!/usr/bin/env python3

print("Content-Type: text/html")
print()  # Ligne vide = fin des headers

print("<html><body>")
print("<h1>Hello from CGI!</h1>")
print("</body></html>")
```

### Script avec variables d'environnement

```python
#!/usr/bin/env python3
import os

print("Content-Type: text/html")
print()

method = os.environ.get('REQUEST_METHOD', 'UNKNOWN')
path = os.environ.get('PATH_INFO', '/')
query = os.environ.get('QUERY_STRING', '')

print(f"<h1>Method: {method}</h1>")
print(f"<p>Path: {path}</p>")
print(f"<p>Query: {query}</p>")
```

### Script qui lit le body POST

```python
#!/usr/bin/env python3
import sys
import os

print("Content-Type: text/html")
print()

# Lire le body depuis stdin
content_length = os.environ.get('CONTENT_LENGTH', '0')
body = sys.stdin.read(int(content_length))

print(f"<h1>Body received:</h1>")
print(f"<pre>{body}</pre>")
```

## Debugging

Si un script CGI ne fonctionne pas:

1. **Vérifier les permissions:**
   ```bash
   chmod +x cgi-bin/script.py
   ```

2. **Tester le script manuellement:**
   ```bash
   ./cgi-bin/script.py
   ```

3. **Vérifier l'interpréteur:**
   ```bash
   which python3
   # Doit correspondre au shebang du script
   ```

4. **Voir les variables d'environnement:**
   ```
   http://localhost:8080/cgi-bin/env.py
   ```

5. **Regarder les logs serveur:**
   Le CGI Handler affiche des erreurs dans stderr si execve échoue.

## Notes importantes

- Les scripts doivent être **exécutables** (`chmod +x`)
- Le **shebang** doit pointer vers le bon interpréteur
- Les scripts doivent **toujours** commencer par des headers HTTP
- Une **ligne vide** sépare les headers du body
- Le header **Content-Type** est obligatoire
- Le header **Status** est optionnel (défaut: 200 OK)

## Prochaines étapes

1. ✅ CGI Handler est complet et testé
2. ⏳ Attendre que tes mates finissent FileHandler + Router
3. ⏳ Intégrer CGI dans Router
4. ⏳ Ajouter support CGI dans le config parser (extensions, interpréteur)
5. ⏳ Tests finaux avec scripts Python, PHP, Perl

Pour l'instant, tu peux tester en ajoutant le code de l'Option 1 ou 2 dans `Server.cpp` temporairement!
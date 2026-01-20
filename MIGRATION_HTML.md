# Migration: HTML hardcodé → Fichiers HTML séparés

## Situation actuelle

Le HTML est généré dynamiquement dans `Server.cpp` via des fonctions:
- `getCommonCSS()` - lignes 22-103
- `generateHomePage()` - lignes 110-203
- `generateTestPage()` - lignes 210-354
- `generateUploadPage()` - lignes 361-453
- `generate404Page()` - lignes 460-501

Ces fonctions sont appelées dans `Server::handleHttpRequest()` (lignes 815-853).

## Nouvelle structure

Les fichiers HTML sont maintenant organisés dans:
```
www/
├── css/
│   └── style.css
└── pages/
    ├── index.html
    ├── test.html
    ├── upload.html
    └── 404.html
```

## Étapes pour migrer

### Étape 1: Implémenter FileHandler

Créer `include/handlers/FileHandler.hpp` et `src/handlers/FileHandler.cpp`:

```cpp
class FileHandler {
public:
    // Lire un fichier depuis le disque et retourner HttpResponse
    static HttpResponse serveFile(const std::string& filepath,
                                  const std::string& root);

    // Vérifier si un fichier existe
    static bool fileExists(const std::string& path);

    // Déterminer le chemin complet (root + uri)
    static std::string resolvePath(const std::string& root,
                                   const std::string& uri);
};
```

Fonctionnalités à implémenter:
- Ouvrir et lire le fichier avec `std::ifstream`
- Détecter le MIME type avec la fonction existante `Mime::getType()`
- Gérer les erreurs: 404 si fichier introuvable, 403 si pas de permissions
- Retourner HttpResponse avec le contenu du fichier

### Étape 2: Modifier Router

Dans `src/router/Router.cpp`, modifier `Router::handleHttpRequest()`:

```cpp
HttpResponse Router::handleHttpRequest(const HttpRequest& req) {
    // Exemple de routing basique
    std::string filepath;

    if (req.rawTarget == "/") {
        filepath = "./www/pages/index.html";
    }
    else if (req.rawTarget == "/test") {
        filepath = "./www/pages/test.html";
    }
    else if (req.rawTarget == "/upload") {
        filepath = "./www/pages/upload.html";
    }
    else if (req.rawTarget.find("/css/") == 0) {
        filepath = "./www" + req.rawTarget;  // /css/style.css
    }
    else {
        filepath = "./www/pages/404.html";
    }

    return FileHandler::serveFile(filepath, "");
}
```

### Étape 3: Nettoyer Server.cpp

Supprimer de `src/network/Server.cpp`:
1. Les fonctions de génération HTML (lignes 22-501)
2. La logique de routing temporaire dans `handleHttpRequest()` (lignes 815-853)
3. Remplacer par un simple appel au Router:

```cpp
HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
    return Router::handleHttpRequest(req);
}
```

### Étape 4: Tester

```bash
make re
./webserv config/webserv.conf

# Tester dans le navigateur
# http://localhost:8080/          → doit servir index.html
# http://localhost:8080/test      → doit servir test.html
# http://localhost:8080/css/style.css → doit servir le CSS
```

## Bénéfices

1. **Séparation des responsabilités**: Le C++ gère la logique réseau/HTTP, le HTML gère la présentation
2. **Maintenabilité**: Modifier le design ne nécessite pas de recompiler
3. **Performance**: Pas besoin de reconstruire le HTML à chaque requête
4. **Extensibilité**: Facile d'ajouter de nouvelles pages sans toucher au code C++

## Note importante

La page `test.html` affiche actuellement des informations statiques. Une fois le FileHandler implémenté, il faudra:
- Soit servir une page dynamique via CGI (script qui génère le HTML avec les vraies infos de la requête)
- Soit utiliser JavaScript côté client pour afficher les infos (via l'API Fetch)

Pour l'instant, servir le fichier statique est suffisant pour valider que le FileHandler fonctionne.
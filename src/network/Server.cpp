#include "../../include/network/Server.hpp"
#include "../../include/http/Status.hpp"
#include "router/Router.hpp"
#include "colours.hpp"
#include "utils.hpp"
#include <iostream>
#include <cstring>
#include <sstream>

// Helper function pour convertir int en string (C++98)
static std::string intToString(int n) {
    std::ostringstream oss;
    oss << n;
    return oss.str();
}

// ============================================================================
// SECTION CSS COMMUNE - Style minimaliste industriel
// ============================================================================
// Cette fonction génère le CSS partagé entre toutes les pages
// Design: blanc dominant, typographie sans-serif, spacing 8px base
static std::string getCommonCSS() {
    std::string css;
    css = "        * { margin: 0; padding: 0; box-sizing: border-box; }\n";
    css += "        :root {\n";
    css += "            --spacing-xs: 8px;\n";
    css += "            --spacing-sm: 16px;\n";
    css += "            --spacing-md: 32px;\n";
    css += "            --spacing-lg: 48px;\n";
    css += "            --spacing-xl: 80px;\n";
    css += "            --color-primary: #667eea;\n";
    css += "            --color-text: #1A1A1A;\n";
    css += "            --color-bg: #FFFFFF;\n";
    css += "            --color-gray: #F5F5F5;\n";
    css += "            --color-border: #E0E0E0;\n";
    css += "        }\n";
    css += "        body {\n";
    css += "            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', Roboto, sans-serif;\n";
    css += "            background: var(--color-bg);\n";
    css += "            color: var(--color-text);\n";
    css += "            line-height: 1.6;\n";
    css += "            font-size: 16px;\n";
    css += "        }\n";
    // Navbar sticky en haut
    css += "        .navbar {\n";
    css += "            background: var(--color-bg);\n";
    css += "            border-bottom: 1px solid var(--color-border);\n";
    css += "            padding: var(--spacing-md) var(--spacing-xl);\n";
    css += "            position: sticky;\n";
    css += "            top: 0;\n";
    css += "            z-index: 100;\n";
    css += "        }\n";
    css += "        .navbar-content {\n";
    css += "            max-width: 1200px;\n";
    css += "            margin: 0 auto;\n";
    css += "            display: flex;\n";
    css += "            justify-content: space-between;\n";
    css += "            align-items: center;\n";
    css += "        }\n";
    css += "        .navbar-brand {\n";
    css += "            font-size: 24px;\n";
    css += "            font-weight: 700;\n";
    css += "            color: var(--color-primary);\n";
    css += "            text-decoration: none;\n";
    css += "        }\n";
    css += "        .navbar-links {\n";
    css += "            display: flex;\n";
    css += "            gap: var(--spacing-md);\n";
    css += "            list-style: none;\n";
    css += "        }\n";
    css += "        .navbar-links a {\n";
    css += "            color: var(--color-text);\n";
    css += "            text-decoration: none;\n";
    css += "            font-weight: 500;\n";
    css += "            transition: color 0.2s;\n";
    css += "        }\n";
    css += "        .navbar-links a:hover {\n";
    css += "            color: var(--color-primary);\n";
    css += "        }\n";
    css += "        .container {\n";
    css += "            max-width: 1200px;\n";
    css += "            margin: 0 auto;\n";
    css += "            padding: var(--spacing-xl);\n";
    css += "        }\n";
    css += "        .btn {\n";
    css += "            display: inline-block;\n";
    css += "            padding: var(--spacing-sm) var(--spacing-lg);\n";
    css += "            background: var(--color-primary);\n";
    css += "            color: white;\n";
    css += "            border: none;\n";
    css += "            border-radius: 8px;\n";
    css += "            font-weight: 600;\n";
    css += "            font-size: 16px;\n";
    css += "            cursor: pointer;\n";
    css += "            text-decoration: none;\n";
    css += "            transition: all 0.2s;\n";
    css += "        }\n";
    css += "        .btn:hover {\n";
    css += "            transform: translateY(-2px);\n";
    css += "            box-shadow: 0 4px 12px rgba(102, 126, 234, 0.4);\n";
    css += "        }\n";
    return css;
}

// ============================================================================
// PAGE HOME (/) - Menu principal avec cards
// ============================================================================
// TEMPORAIRE: Cette fonction génère du HTML hardcodé
// FUTUR: Le FileHandler lira /var/www/html/index.html depuis le disque
static std::string generateHomePage() {
    std::string html;
    html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>Webserv - Home</title>\n";
    html += "    <style>\n";
    html += getCommonCSS();
    // Hero section: titre centré 64px
    html += "        .hero {\n";
    html += "            text-align: center;\n";
    html += "            padding: var(--spacing-xl) 0;\n";
    html += "            margin-bottom: var(--spacing-xl);\n";
    html += "        }\n";
    html += "        .hero h1 {\n";
    html += "            font-size: 64px;\n";
    html += "            font-weight: 700;\n";
    html += "            margin-bottom: var(--spacing-sm);\n";
    html += "            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);\n";
    html += "            -webkit-background-clip: text;\n";
    html += "            -webkit-text-fill-color: transparent;\n";
    html += "        }\n";
    html += "        .hero p {\n";
    html += "            font-size: 24px;\n";
    html += "            color: #666;\n";
    html += "        }\n";
    // Grid 3 colonnes auto-fit
    html += "        .cards {\n";
    html += "            display: grid;\n";
    html += "            grid-template-columns: repeat(auto-fit, minmax(300px, 1fr));\n";
    html += "            gap: var(--spacing-md);\n";
    html += "            margin-top: var(--spacing-xl);\n";
    html += "        }\n";
    html += "        .card {\n";
    html += "            background: white;\n";
    html += "            border: 1px solid var(--color-border);\n";
    html += "            border-radius: 12px;\n";
    html += "            padding: var(--spacing-lg);\n";
    html += "            transition: all 0.3s;\n";
    html += "        }\n";
    html += "        .card:hover {\n";
    html += "            transform: translateY(-4px);\n";
    html += "            box-shadow: 0 8px 24px rgba(0,0,0,0.1);\n";
    html += "        }\n";
    html += "        .card h2 {\n";
    html += "            font-size: 32px;\n";
    html += "            margin-bottom: var(--spacing-sm);\n";
    html += "        }\n";
    html += "        .card p {\n";
    html += "            color: #666;\n";
    html += "            margin-bottom: var(--spacing-md);\n";
    html += "        }\n";
    html += "    </style>\n</head>\n<body>\n";

    // Navbar commune
    html += "    <nav class=\"navbar\">\n";
    html += "        <div class=\"navbar-content\">\n";
    html += "            <a href=\"/\" class=\"navbar-brand\">Webserv</a>\n";
    html += "            <ul class=\"navbar-links\">\n";
    html += "                <li><a href=\"/\">Home</a></li>\n";
    html += "                <li><a href=\"/test\">Test</a></li>\n";
    html += "                <li><a href=\"/upload\">Upload</a></li>\n";
    html += "            </ul>\n";
    html += "        </div>\n";
    html += "    </nav>\n";

    html += "    <div class=\"container\">\n";
    html += "        <div class=\"hero\">\n";
    html += "            <h1>Webserv</h1>\n";
    html += "            <p>High-Performance C++98 HTTP Server</p>\n";
    html += "        </div>\n";

    // Cards cliquables
    html += "        <div class=\"cards\">\n";
    html += "            <div class=\"card\">\n";
    html += "                <h2>Test Methods</h2>\n";
    html += "                <p>Test HTTP methods (GET, POST, DELETE) with interactive buttons</p>\n";
    html += "                <a href=\"/test\" class=\"btn\">Go to Test</a>\n";
    html += "            </div>\n";
    html += "            <div class=\"card\">\n";
    html += "                <h2>Upload Files</h2>\n";
    html += "                <p>Test file uploads with POST requests (coming soon)</p>\n";
    html += "                <a href=\"/upload\" class=\"btn\">Go to Upload</a>\n";
    html += "            </div>\n";
    html += "            <div class=\"card\">\n";
    html += "                <h2>Documentation</h2>\n";
    html += "                <p>Network layer fully integrated with HTTP parser</p>\n";
    html += "                <a href=\"/test\" class=\"btn\">View Status</a>\n";
    html += "            </div>\n";
    html += "        </div>\n";
    html += "    </div>\n";
    html += "</body>\n</html>";
    return html;
}

// ============================================================================
// PAGE TEST (/test) - Boutons pour tester GET/POST/DELETE
// ============================================================================
// TEMPORAIRE: HTML généré en C++
// FUTUR: FileHandler servira test.html depuis le disque
static std::string generateTestPage(const HttpRequest& req) {
    std::string html;

    // Déterminer méthode et couleur
    std::string methodStr;
    std::string methodColor;
    if (req.method == METHOD_GET) {
        methodStr = "GET";
        methodColor = "#10b981";
    } else if (req.method == METHOD_POST) {
        methodStr = "POST";
        methodColor = "#3b82f6";
    } else if (req.method == METHOD_DELETE) {
        methodStr = "DELETE";
        methodColor = "#ef4444";
    } else {
        methodStr = "UNKNOWN";
        methodColor = "#6b7280";
    }

    html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>Webserv - Test</title>\n";
    html += "    <style>\n";
    html += getCommonCSS();
    html += "        .test-section { margin: var(--spacing-xl) 0; }\n";
    html += "        .test-section h2 { font-size: 32px; margin-bottom: var(--spacing-md); }\n";
    html += "        .request-info {\n";
    html += "            background: var(--color-gray);\n";
    html += "            border: 1px solid var(--color-border);\n";
    html += "            border-radius: 12px;\n";
    html += "            padding: var(--spacing-md);\n";
    html += "            margin-bottom: var(--spacing-lg);\n";
    html += "        }\n";
    html += "        .info-row {\n";
    html += "            display: flex;\n";
    html += "            padding: var(--spacing-sm) 0;\n";
    html += "            border-bottom: 1px solid var(--color-border);\n";
    html += "        }\n";
    html += "        .info-row:last-child { border-bottom: none; }\n";
    html += "        .info-label { font-weight: 600; width: 120px; }\n";
    html += "        .info-value { flex: 1; font-family: 'Courier New', monospace; }\n";
    html += "        .method-badge {\n";
    html += "            display: inline-block;\n";
    html += "            padding: 4px 12px;\n";
    html += "            border-radius: 6px;\n";
    html += "            color: white;\n";
    html += "            font-weight: 700;\n";
    html += "            font-size: 14px;\n";
    html += "        }\n";
    html += "        .button-grid {\n";
    html += "            display: grid;\n";
    html += "            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr));\n";
    html += "            gap: var(--spacing-sm);\n";
    html += "        }\n";
    html += "        .btn-get { background: #10b981; }\n";
    html += "        .btn-post { background: #3b82f6; }\n";
    html += "        .btn-delete { background: #ef4444; }\n";
    html += "    </style>\n</head>\n<body>\n";

    // Navbar
    html += "    <nav class=\"navbar\">\n";
    html += "        <div class=\"navbar-content\">\n";
    html += "            <a href=\"/\" class=\"navbar-brand\">Webserv</a>\n";
    html += "            <ul class=\"navbar-links\">\n";
    html += "                <li><a href=\"/\">Home</a></li>\n";
    html += "                <li><a href=\"/test\">Test</a></li>\n";
    html += "                <li><a href=\"/upload\">Upload</a></li>\n";
    html += "            </ul>\n";
    html += "        </div>\n";
    html += "    </nav>\n";

    html += "    <div class=\"container\">\n";
    html += "        <h1 style=\"font-size: 48px; margin-bottom: var(--spacing-md);\">Test HTTP Methods</h1>\n";
    html += "        <p style=\"color: #666; font-size: 18px; margin-bottom: var(--spacing-xl);\">Click buttons below to test different HTTP methods</p>\n";

    // Info de la requête actuelle
    html += "        <div class=\"request-info\">\n";
    html += "            <h3 style=\"margin-bottom: var(--spacing-md);\">Current Request</h3>\n";
    html += "            <div class=\"info-row\">\n";
    html += "                <div class=\"info-label\">Method</div>\n";
    html += "                <div class=\"info-value\"><span class=\"method-badge\" style=\"background:" + methodColor + ";\">" + methodStr + "</span></div>\n";
    html += "            </div>\n";
    html += "            <div class=\"info-row\">\n";
    html += "                <div class=\"info-label\">Path</div>\n";
    html += "                <div class=\"info-value\">" + req.rawTarget + "</div>\n";
    html += "            </div>\n";
    html += "            <div class=\"info-row\">\n";
    html += "                <div class=\"info-label\">Protocol</div>\n";
    html += "                <div class=\"info-value\">" + req.httpVersion + "</div>\n";
    html += "            </div>\n";

    // Afficher Host
    std::map<std::string, std::string>::const_iterator host_it = req.headers.find("host");
    if (host_it != req.headers.end()) {
        html += "            <div class=\"info-row\">\n";
        html += "                <div class=\"info-label\">Host</div>\n";
        html += "                <div class=\"info-value\">" + host_it->second + "</div>\n";
        html += "            </div>\n";
    }

    // Afficher body size si présent
    if (!req.body.empty()) {
        html += "            <div class=\"info-row\">\n";
        html += "                <div class=\"info-label\">Body Size</div>\n";
        html += "                <div class=\"info-value\">" + intToString(req.body.size()) + " bytes</div>\n";
        html += "            </div>\n";
    }
    html += "        </div>\n";

    // Boutons de test
    html += "        <div class=\"test-section\">\n";
    html += "            <h2>Test Buttons</h2>\n";
    html += "            <div class=\"button-grid\">\n";
    html += "                <button onclick=\"testRequest('GET', '/test')\" class=\"btn btn-get\">GET /test</button>\n";
    html += "                <button onclick=\"testRequest('GET', '/api/users')\" class=\"btn btn-get\">GET /api/users</button>\n";
    html += "                <button onclick=\"testRequest('POST', '/test')\" class=\"btn btn-post\">POST /test</button>\n";
    html += "                <button onclick=\"testRequest('POST', '/test', 'Hello Webserv!')\" class=\"btn btn-post\">POST with body</button>\n";
    html += "                <button onclick=\"testRequest('DELETE', '/file.txt')\" class=\"btn btn-delete\">DELETE /file.txt</button>\n";
    html += "            </div>\n";
    html += "        </div>\n";

    html += "        <div style=\"background: var(--color-gray); padding: var(--spacing-md); border-radius: 8px; margin-top: var(--spacing-xl);\">\n";
    html += "            <p><strong>Note:</strong> Ces boutons testent uniquement le parsing HTTP. Les actions réelles (servir fichiers, upload, delete) seront implémentées par le Router et les Handlers.</p>\n";
    html += "        </div>\n";
    html += "    </div>\n";

    // Script JavaScript
    html += "    <script>\n";
    html += "        function testRequest(method, path, body) {\n";
    html += "            const options = { method: method };\n";
    html += "            if (body) {\n";
    html += "                options.headers = { 'Content-Type': 'text/plain' };\n";
    html += "                options.body = body;\n";
    html += "            }\n";
    html += "            fetch('http://' + window.location.host + path, options)\n";
    html += "                .then(response => response.text())\n";
    html += "                .then(() => window.location.reload())\n";
    html += "                .catch(err => alert('Error: ' + err.message));\n";
    html += "        }\n";
    html += "    </script>\n";
    html += "</body>\n</html>";
    return html;
}

// ============================================================================
// PAGE UPLOAD (/upload) - Formulaire d'upload (simulation)
// ============================================================================
// TEMPORAIRE: HTML généré en C++, upload non fonctionnel
// FUTUR: UploadHandler gérera vraiment l'upload de fichiers
static std::string generateUploadPage(const HttpRequest& req) {
    std::string html;
    html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>Webserv - Upload</title>\n";
    html += "    <style>\n";
    html += getCommonCSS();
    html += "        .upload-container {\n";
    html += "            max-width: 600px;\n";
    html += "            margin: var(--spacing-xl) auto;\n";
    html += "        }\n";
    html += "        .upload-zone {\n";
    html += "            border: 2px dashed var(--color-border);\n";
    html += "            border-radius: 12px;\n";
    html += "            padding: var(--spacing-xl);\n";
    html += "            text-align: center;\n";
    html += "            background: var(--color-gray);\n";
    html += "            margin: var(--spacing-lg) 0;\n";
    html += "        }\n";
    html += "        .upload-zone:hover {\n";
    html += "            border-color: var(--color-primary);\n";
    html += "            background: #f0f4ff;\n";
    html += "        }\n";
    html += "        input[type=\"file\"] {\n";
    html += "            display: none;\n";
    html += "        }\n";
    html += "        .info-box {\n";
    html += "            background: #fff3cd;\n";
    html += "            border: 1px solid #ffc107;\n";
    html += "            border-radius: 8px;\n";
    html += "            padding: var(--spacing-md);\n";
    html += "            margin-top: var(--spacing-lg);\n";
    html += "        }\n";
    html += "    </style>\n</head>\n<body>\n";

    // Navbar
    html += "    <nav class=\"navbar\">\n";
    html += "        <div class=\"navbar-content\">\n";
    html += "            <a href=\"/\" class=\"navbar-brand\">Webserv</a>\n";
    html += "            <ul class=\"navbar-links\">\n";
    html += "                <li><a href=\"/\">Home</a></li>\n";
    html += "                <li><a href=\"/test\">Test</a></li>\n";
    html += "                <li><a href=\"/upload\">Upload</a></li>\n";
    html += "            </ul>\n";
    html += "        </div>\n";
    html += "    </nav>\n";

    html += "    <div class=\"container\">\n";
    html += "        <div class=\"upload-container\">\n";
    html += "            <h1 style=\"font-size: 48px; margin-bottom: var(--spacing-md);\">Upload Files</h1>\n";
    html += "            <p style=\"color: #666; margin-bottom: var(--spacing-lg);\">Test file upload functionality (coming soon)</p>\n";

    // Zone d'upload (non fonctionnelle pour l'instant)
    html += "            <div class=\"upload-zone\" onclick=\"document.getElementById('file-input').click()\">\n";
    html += "                <div style=\"font-size: 48px; margin-bottom: var(--spacing-sm);\">&#x1F4C1;</div>\n";
    html += "                <h3>Click to select file</h3>\n";
    html += "                <p style=\"color: #666; margin-top: var(--spacing-sm);\">or drag and drop</p>\n";
    html += "                <input type=\"file\" id=\"file-input\" onchange=\"handleFileSelect(event)\">\n";
    html += "            </div>\n";

    // Message si POST reçu
    if (req.method == METHOD_POST) {
        html += "            <div style=\"background: #d4edda; border: 1px solid #28a745; border-radius: 8px; padding: var(--spacing-md); margin-top: var(--spacing-lg);\">\n";
        html += "                <strong>POST request received!</strong><br>\n";
        html += "                Body size: " + intToString(req.body.size()) + " bytes<br>\n";
        html += "                (Upload Handler not implemented yet - file not saved)\n";
        html += "            </div>\n";
    }

    html += "            <div class=\"info-box\">\n";
    html += "                <strong>⚠️ Upload Handler Not Implemented Yet</strong><br>\n";
    html += "                The server receives POST requests but doesn't save files yet.<br>\n";
    html += "                This will be implemented by the UploadHandler module.\n";
    html += "            </div>\n";
    html += "        </div>\n";
    html += "    </div>\n";

    html += "    <script>\n";
    html += "        function handleFileSelect(event) {\n";
    html += "            const file = event.target.files[0];\n";
    html += "            if (!file) return;\n";
    html += "            const formData = new FormData();\n";
    html += "            formData.append('file', file);\n";
    html += "            fetch('/upload', { method: 'POST', body: formData })\n";
    html += "                .then(response => response.text())\n";
    html += "                .then(() => window.location.reload())\n";
    html += "                .catch(err => alert('Error: ' + err.message));\n";
    html += "        }\n";
    html += "    </script>\n";
    html += "</body>\n</html>";
    return html;
}

// ============================================================================
// PAGE 404 - Page non trouvée
// ============================================================================
// TEMPORAIRE: 404 généré en C++
// FUTUR: ErrorHandler retournera une vraie page 404.html depuis le disque
static std::string generate404Page(const std::string& path) {
    std::string html;
    html = "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
    html += "    <meta charset=\"UTF-8\">\n";
    html += "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
    html += "    <title>404 - Not Found</title>\n";
    html += "    <style>\n";
    html += getCommonCSS();
    html += "        .error-container {\n";
    html += "            text-align: center;\n";
    html += "            padding: var(--spacing-xl) 0;\n";
    html += "        }\n";
    html += "        .error-code {\n";
    html += "            font-size: 120px;\n";
    html += "            font-weight: 700;\n";
    html += "            color: var(--color-primary);\n";
    html += "            line-height: 1;\n";
    html += "        }\n";
    html += "    </style>\n</head>\n<body>\n";

    html += "    <nav class=\"navbar\">\n";
    html += "        <div class=\"navbar-content\">\n";
    html += "            <a href=\"/\" class=\"navbar-brand\">Webserv</a>\n";
    html += "            <ul class=\"navbar-links\">\n";
    html += "                <li><a href=\"/\">Home</a></li>\n";
    html += "                <li><a href=\"/test\">Test</a></li>\n";
    html += "                <li><a href=\"/upload\">Upload</a></li>\n";
    html += "            </ul>\n";
    html += "        </div>\n";
    html += "    </nav>\n";

    html += "    <div class=\"container\">\n";
    html += "        <div class=\"error-container\">\n";
    html += "            <div class=\"error-code\">404</div>\n";
    html += "            <h1 style=\"font-size: 48px; margin: var(--spacing-md) 0;\">Page Not Found</h1>\n";
    html += "            <p style=\"color: #666; font-size: 18px; margin-bottom: var(--spacing-lg);\">The page <code>" + path + "</code> does not exist.</p>\n";
    html += "            <a href=\"/\" class=\"btn\">Go to Home</a>\n";
    html += "        </div>\n";
    html += "    </div>\n";
    html += "</body>\n</html>";
    return html;
}

Server::ServerException::ServerException(const std::string& message)
    : std::runtime_error(message) {
}

Server::Server(int port, int backlog)
    : socket_manager(), multiplexer(), clients(), server_fd(-1), port(port), running(false) {

    std::cout << BOLD_CYAN << "=== Initializing Server on port ["
		<< port << "] ===" << RES << std::endl;

    // Creer le socket serveur
    server_fd = socket_manager.create_server(port, backlog);

    std::cout << GREEN << "✓ " << RES << "Server socket created: "
		<< RES << "fd=" << server_fd << std::endl;

    // Ajouter le server_fd au multiplexer pour surveiller les nouvelles connexions
    multiplexer.add_fd(server_fd, POLLIN);

    std::cout << GREEN << "✓ " << RES
		<< "Server ready to accept connections" << std::endl;
}

Server::~Server() {
    std::cout << BOLD_ORANGE << "=== Shutting down server ===" << RES << std::endl;

    // Fermer toutes les connexions clients
    for (std::map<int, Connection*>::iterator it = clients.begin();
         it != clients.end(); ++it) {
        delete it->second;
    }
    clients.clear();

    // Liberer tous les parsers HTTP
    for (std::map<int, HttpRequestParser*>::iterator it = parsers.begin();
         it != parsers.end(); ++it) {
        delete it->second;
    }
    parsers.clear();

    // Fermer le socket serveur
    if (server_fd >= 0) {
        SocketManager::close_socket(server_fd);
    }

    std::cout << GREEN << "✓ " << RES << "Server stopped" << std::endl;
}

void Server::run() {
    running = true;

    std::cout << std::endl
    	<< "Multi-client server running on port "
		<< GREEN << port << RES << std::endl
    	<< "Test with: telnet localhost "
		<< GREEN << port << RES << std::endl
    	<< "(Ctrl+C to stop)\n" << std::endl;

    while (running) {
        // Attendre des evenements sur les fds surveilles
        std::vector<int> ready_fds = multiplexer.wait(-1);

        if (ready_fds.empty()) {
            continue;
        }

        // std::cout << "poll() returned " << ready_fds.size() << " ready fd(s)" << std::endl;

        // Traiter chaque fd pret
        for (size_t i = 0; i < ready_fds.size(); i++) {
            int fd = ready_fds[i];

            if (fd == server_fd) {
                // Nouvelle connexion entrante
                acceptNewClient();
            }
            else {
                // Evenement sur un client existant
                short revents = multiplexer.get_revents(fd);
                bool client_disconnected = false;

                if (revents & POLLIN) {
                    handleClientRead(fd);

                    // Verifier si le client a ete supprime pendant la lecture
                    if (clients.find(fd) == clients.end()) {
                        client_disconnected = true;
                    }
                }

                // IMPORTANT: ne traiter POLLOUT que si le client n'a pas ete deconnecte
                if (!client_disconnected && (revents & POLLOUT)) {
                    handleClientWrite(fd);
                }
            }
        }

        std::cout << std::endl;
    }
}

void Server::stop() {
    running = false;
}

void Server::acceptNewClient() {
    try {
        int client_fd = socket_manager.accept_connection(server_fd);
        Connection* conn = new Connection(client_fd);
        clients[client_fd] = conn;

        // Surveiller le client pour les donnees entrantes
        multiplexer.add_fd(client_fd, POLLIN);

        std::cout << GREEN << "✓ " << RES << "New client connected: "
			<< "fd=" << client_fd << " -> (total clients: " << clients.size() << ")"
			<< std::endl;
    }
    catch (const SocketManager::SocketException& e) {
        std::cerr << "✗ Error accepting client: " << e.what() << std::endl;
    }
    catch (const Connection::ConnectionException& e) {
        std::cerr << "✗ Error creating connection: " << e.what() << std::endl;
    }
}

void Server::handleClientRead(int fd) {
    Connection* conn = clients[fd];
    ssize_t n = conn->read_available();

    if (n > 0) {
        // std::cout << "  [fd=" << fd << "] Received " << n << " bytes" << std::endl;
        std::cout << BOLD_YELLOW << "[DEBUG] "
			<< RES << "Received " << n << " bytes" << std::endl;

        // Traiter la requete HTTP avec le parser
        processRequest(conn, fd);

        // Activer POLLOUT si une reponse est prete
        if (!conn->send_buffer.empty()) {
            multiplexer.modify_fd(fd, POLLIN | POLLOUT);
        }
    }
    else if (n == 0) {
        // Client a ferme la connexion
        // std::cout << "  [fd=" << fd << "] Client disconnected" << std::endl;
        std::cout << BOLD_YELLOW << "[DEBUG] " << RES << "Client disconnected" << std::endl;
        removeClient(fd);
    }
    else {
        // Erreur de lecture
        std::cout << "  [fd=" << fd << "] Error reading" << std::endl;
        removeClient(fd);
    }
}

void Server::handleClientWrite(int fd) {
    Connection* conn = clients[fd];

    if (conn->has_pending_data()) {
        ssize_t sent = conn->write_pending();

        if (sent > 0) {
            // std::cout << "  [fd=" << fd << "] Sent " << sent << " bytes" << std::endl;
            std::cout << BOLD_YELLOW << "[DEBUG] "
				<< RES << "Sent " << sent << " bytes" << std::endl;
        }
        else if (sent < 0) {
            std::cout << "  [fd=" << fd << "] Error writing" << std::endl;
            removeClient(fd);
            return;
        }

        // Si tout a ete envoye, desactiver POLLOUT
        if (!conn->has_pending_data()) {
            multiplexer.modify_fd(fd, POLLIN);
        }
    }
    else {
        // Plus rien a envoyer, desactiver POLLOUT
        multiplexer.modify_fd(fd, POLLIN);
    }
}

void Server::removeClient(int fd) {
    std::map<int, Connection*>::iterator it = clients.find(fd);

    if (it != clients.end()) {
        multiplexer.remove_fd(fd);
        delete it->second;
        clients.erase(it);

        // Supprimer aussi le parser HTTP associe
        std::map<int, HttpRequestParser*>::iterator parser_it = parsers.find(fd);
        if (parser_it != parsers.end()) {
            delete parser_it->second;
            parsers.erase(parser_it);
        }

        std::cout << BOLD_YELLOW << "[DEBUG] "
			<< RES << "live clients: " << clients.size() << std::endl;
        // std::cout << "  (remaining clients: " << clients.size() << ")" << std::endl;
    }
}

void Server::processRequest(Connection* conn, int fd) {
    // Creer un parser pour ce client si necessaire
    if (parsers.find(fd) == parsers.end()) {
        parsers[fd] = new HttpRequestParser();
        // std::cout << "  [fd=" << fd << "] Created HTTP parser" << std::endl;
    }

    HttpRequestParser* parser = parsers[fd];

    // Envoyer les donnees au parser
	// std::cout << BOLD_GOLD << conn->recv_buffer << RES << std::endl;
    parser->feed(conn->recv_buffer);
    conn->recv_buffer.clear();

    // Verifier si la requete est complete
    if (parser->isDone()) {
        if (parser->hasError()) {
            // Generer une reponse d'erreur HTTP
            int errorCode = parser->getErrorStatus();
            HttpResponse resp(errorCode, reasonPhrase(errorCode));
            resp.headers["Content-Type"] = "text/html";
            resp.body = "<html><body><h1>Error " + intToString(errorCode)
                      + "</h1><p>" + reasonPhrase(errorCode) + "</p></body></html>";

            std::cout << "  [fd=" << fd << "] HTTP Error: " << errorCode << std::endl;

            // Toujours fermer connexion sur erreur
            conn->send_buffer = ResponseBuilder::build(resp, true);
        }
        else {
            // Traiter la requete HTTP valide
            const HttpRequest& req = parser->getRequest();

            // std::cout << "  [fd=" << fd << "] HTTP Request: "
            //          << (req.method == METHOD_GET ? "GET" :
            //              req.method == METHOD_POST ? "POST" :
            //              req.method == METHOD_DELETE ? "DELETE" : "UNKNOWN")
            //          << " " << req.rawTarget << " " << req.httpVersion << std::endl;
			printHttpRequest(req);

            // Generer la reponse HTTP
            HttpResponse resp = Router::handleHttpRequest(req);

            // Determiner si on doit fermer la connexion (keep-alive)
            bool closeConnection = parser->shouldCloseConnection();
            conn->send_buffer = ResponseBuilder::build(resp, closeConnection);

            // std::cout << "  [fd=" << fd << "] HTTP Response: " << resp.statusCode
            //          << " (Connection: " << (closeConnection ? "close" : "keep-alive") << ")" << std::endl;
			std::cout << BOLD_GREEN << "[HTTP Response] " << RES << resp.statusCode
                     << " (Connection: " << (closeConnection ? "close" : "keep-alive") << ")" << std::endl;

		}

        // Reset parser pour la prochaine requete (keep-alive)
        parser->reset();
    }
}

// ============================================================================
// ROUTING TEMPORAIRE - handleHttpRequest()
// ============================================================================
// CETTE FONCTION EST TEMPORAIRE !
//
// Actuellement: On fait un routing basique en C++ avec des if/else sur le path
// - "/" → Page d'accueil avec menu (cards cliquables)
// - "/test" → Page de test avec boutons GET/POST/DELETE
// - "/upload" → Page d'upload (simulation, pas encore fonctionnel)
// - Autre → Page 404
//
// FUTUR (à implémenter par ton équipe):
// Cette fonction devra être remplacée par :
//
//   HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
//       return router->route(req);  // ← Simple appel au Router
//   }
//
// Et le Router fera:
// 1. Matcher le path avec la config (location blocks)
// 2. Vérifier que la méthode est autorisée
// 3. Dispatcher vers le bon handler:
//    - FileHandler pour GET (servir fichiers HTML/CSS/JS depuis disque)
//    - UploadHandler pour POST /upload (sauvegarder fichiers)
//    - CGIHandler pour *.py, *.php (exécuter scripts)
//    - ErrorHandler pour 404, 403, 500, etc.
//
// Exemple d'architecture finale :
//
//   Router::route(req) {
//       LocationConfig loc = config->matchLocation(req.path);
//
//       if (!loc.isMethodAllowed(req.method))
//           return errorHandler->methodNotAllowed(405);
//
//       if (req.method == GET && isCGI(req.path))
//           return cgiHandler->execute(req, loc);
//       else if (req.method == GET)
//           return fileHandler->serveFile(req.path, loc);
//       else if (req.method == POST && req.path == "/upload")
//           return uploadHandler->handle(req, loc);
//       else if (req.method == DELETE)
//           return fileHandler->deleteFile(req.path, loc);
//
//       return errorHandler->notFound(404);
//   }
//
// ============================================================================
HttpResponse Server::handleHttpRequest(const HttpRequest& req) {
    HttpResponse resp(200, "OK");
    resp.headers["Content-Type"] = "text/html; charset=utf-8";

    // Headers CORS pour permettre les tests depuis le navigateur
    resp.headers["Access-Control-Allow-Origin"] = "*";
    resp.headers["Access-Control-Allow-Methods"] = "GET, POST, DELETE, OPTIONS";
    resp.headers["Access-Control-Allow-Headers"] = "Content-Type";

    // ========================================================================
    // ROUTING TEMPORAIRE (if/else basique sur le path)
    // ========================================================================
    // Route: "/" → Page d'accueil (menu principal)
    if (req.rawTarget == "/") {
        resp.body = generateHomePage();
    }
    // Route: "/test" → Page de test avec boutons interactifs
    else if (req.rawTarget == "/test") {
        resp.body = generateTestPage(req);
    }
    // Route: "/upload" → Page d'upload (simulation)
    else if (req.rawTarget == "/upload") {
        resp.body = generateUploadPage(req);
    }
    // Route: favicon (éviter 404 pour favicon)
    else if (req.rawTarget == "/favicon.ico") {
        resp.statusCode = 204;  // No Content
        resp.reason = "No Content";
        resp.body = "";
    }
    // Route: Tout le reste → 404 Not Found
    else {
        resp.statusCode = 404;
        resp.reason = "Not Found";
        resp.body = generate404Page(req.rawTarget);
    }

    return resp;
}

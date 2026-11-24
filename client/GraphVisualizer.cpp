#include "GraphVisualizer.h"

#include <algorithm>
#include <iostream>
#include <mutex>
#include <sstream>
#include <string>

#ifdef _WIN32
    #ifndef NOMINMAX
    #define NOMINMAX
    #endif
    #include <winsock2.h>
    #include <ws2tcpip.h>
    #pragma comment(lib, "Ws2_32.lib")
    using SocketHandle = SOCKET;
    static constexpr SocketHandle kInvalidSocket = INVALID_SOCKET;
#else
    #include <arpa/inet.h>
    #include <netdb.h>
    #include <sys/socket.h>
    #include <unistd.h>
    using SocketHandle = int;
    static constexpr SocketHandle kInvalidSocket = -1;
#endif

namespace {

std::mutex g_mutex;
SocketHandle g_socket = kInvalidSocket;
std::string g_host = "127.0.0.1";
std::uint16_t g_port = 47653;
bool g_warned = false;

void closeSocketUnlocked() {
    if (g_socket == kInvalidSocket) return;
#ifdef _WIN32
    closesocket(g_socket);
#else
    close(g_socket);
#endif
    g_socket = kInvalidSocket;
}

bool ensureSocketSubsystem() {
#ifdef _WIN32
    static bool initialized = false;
    static bool success = false;
    if (!initialized) {
        WSADATA data{};
        success = (WSAStartup(MAKEWORD(2, 2), &data) == 0);
        initialized = true;
    }
    return success;
#else
    return true;
#endif
}

bool connectUnlocked() {
    if (g_socket != kInvalidSocket) return true;
    if (!ensureSocketSubsystem()) return false;

    addrinfo hints{};
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    addrinfo *results = nullptr;
    const std::string portText = std::to_string(g_port);
    const int lookup = getaddrinfo(g_host.c_str(), portText.c_str(), &hints, &results);
    if (lookup != 0 || results == nullptr) return false;

    for (addrinfo *it = results; it != nullptr; it = it->ai_next) {
        SocketHandle candidate = socket(it->ai_family, it->ai_socktype, it->ai_protocol);
        if (candidate == kInvalidSocket) continue;
        if (connect(candidate, it->ai_addr, static_cast<int>(it->ai_addrlen)) == 0) {
            g_socket = candidate;
            break;
        }
#ifdef _WIN32
        closesocket(candidate);
#else
        close(candidate);
#endif
    }
    freeaddrinfo(results);
    return g_socket != kInvalidSocket;
}

std::string jsonEscape(const std::string &value) {
    std::ostringstream out;
    for (unsigned char ch : value) {
        switch (ch) {
            case '"': out << "\\\""; break;
            case '\\': out << "\\\\"; break;
            case '\b': out << "\\b"; break;
            case '\f': out << "\\f"; break;
            case '\n': out << "\\n"; break;
            case '\r': out << "\\r"; break;
            case '\t': out << "\\t"; break;
            default:
                if (ch < 0x20) {
                    static const char hex[] = "0123456789abcdef";
                    out << "\\u00" << hex[(ch >> 4) & 0x0f] << hex[ch & 0x0f];
                } else {
                    out << static_cast<char>(ch);
                }
        }
    }
    return out.str();
}

bool sendAllUnlocked(const std::string &message) {
    std::size_t sent = 0;
    while (sent < message.size()) {
#ifdef _WIN32
        const int chunk = static_cast<int>(std::min<std::size_t>(message.size() - sent, 1u << 20));
        const int result = send(g_socket, message.data() + sent, chunk, 0);
#else
        const std::size_t chunk = std::min<std::size_t>(message.size() - sent, 1u << 20);
        #ifdef MSG_NOSIGNAL
        const ssize_t result = send(g_socket, message.data() + sent, chunk, MSG_NOSIGNAL);
        #else
        const ssize_t result = send(g_socket, message.data() + sent, chunk, 0);
        #endif
#endif
        if (result <= 0) return false;
        sent += static_cast<std::size_t>(result);
    }
    return true;
}

bool sendEventUnlocked(const std::string &json) {
    const std::string line = json + "\n";
    if (!connectUnlocked() || !sendAllUnlocked(line)) {
        closeSocketUnlocked();
        if (!connectUnlocked() || !sendAllUnlocked(line)) {
            if (!g_warned) {
                std::cerr << "[GraphVisualizer] Cannot connect to " << g_host << ':' << g_port
                          << ". Start GraphVisualizerViewer before calling graph functions.\n";
                g_warned = true;
            }
            return false;
        }
    }
    g_warned = false;
    return true;
}

void sendSimple(const std::string &op, const std::string &fields = {}) {
    std::lock_guard<std::mutex> lock(g_mutex);
    std::string json = "{\"op\":\"" + op + "\"";
    if (!fields.empty()) json += "," + fields;
    json += "}";
    sendEventUnlocked(json);
}

std::string intField(const char *name, int value) {
    return std::string("\"") + name + "\":" + std::to_string(value);
}

} // namespace

bool InitGraph(const std::string &host, std::uint16_t port) {
    std::lock_guard<std::mutex> lock(g_mutex);
    closeSocketUnlocked();
    g_host = host.empty() ? "127.0.0.1" : host;
    g_port = port;
    g_warned = false;
    return connectUnlocked();
}

void ShutdownGraph() {
    std::lock_guard<std::mutex> lock(g_mutex);
    closeSocketUnlocked();
#ifdef _WIN32
    // Winsock is process-wide; leave it initialized so a later InitGraph can reconnect.
#endif
}

bool IsGraphConnected() {
    std::lock_guard<std::mutex> lock(g_mutex);
    return g_socket != kInvalidSocket;
}

void AddNode(int u) {
    sendSimple("addNode", intField("u", u));
}

void AddEdge(int u, int v) {
    const std::string fields = intField("u", u) + "," + intField("v", v) +
        ",\"hasWeight\":false,\"directed\":true";
    sendSimple("addEdge", fields);
}

void AddEdge(int u, int v, const std::string &weight, int directed) {
    const std::string fields = intField("u", u) + "," + intField("v", v) +
        ",\"hasWeight\":true,\"weight\":\"" + jsonEscape(weight) + "\",\"directed\":" +
        (directed != 0 ? "true" : "false");
    sendSimple("addEdge", fields);
}

void HighLight(int u) {
    sendSimple("highlightNode", intField("u", u));
}

void HighLightEdge(int u, int v) {
    sendSimple("highlightEdge", intField("u", u) + "," + intField("v", v));
}

void ClearHighLight() {
    sendSimple("clearHighlight");
}

void RemoveNode(int u) {
    sendSimple("removeNode", intField("u", u));
}

void RemoveEdge(int u, int v) {
    sendSimple("removeEdge", intField("u", u) + "," + intField("v", v));
}

void ClearGraph() {
    sendSimple("clearGraph");
}

void SetNodeLabel(int u, const std::string &label) {
    sendSimple("setNodeLabel", intField("u", u) + ",\"label\":\"" + jsonEscape(label) + "\"");
}

void SetNodeColor(int u, const std::string &color) {
    sendSimple("setNodeColor", intField("u", u) + ",\"color\":\"" + jsonEscape(color) + "\"");
}

void Sleep(int ms) {
    sendSimple("wait", intField("ms", std::max(0, ms)));
}

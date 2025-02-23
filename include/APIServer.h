#pragma once
#include <WebServer.h>
#include <functional>
#include <map>

struct RequestArgs
{
    WebServer *server;
    std::map<String, String> _args;

    RequestArgs(WebServer *srv) : server(srv)
    {
        for (uint8_t i = 0; i < server->args(); i++)
        {
            _args.insert({server->argName(i), server->arg(i)});
        }
    }

    String operator[](const String &key) const
    {
        auto it = _args.find(key);
        if (it != _args.end())
        {
            return it->second;
        }
        return String();
    }

    // Optional: Helper to check if a given key exists
    bool contains(const String &key) const
    {
        return _args.find(key) != _args.end();
    }
};

class APIServer
{
    WebServer server;
    std::map<String, std::function<void(RequestArgs)>> endpoints;

public:
    explicit APIServer(uint16_t port = 80) : server(port) {}

    template <typename Function>
    void addEndpoint(const String &path, HTTPMethod method, Function handler)
    {
        server.on(path.c_str(), method, [this, handler]()
                  {
      RequestArgs args{&server};
      handler(args); });
    }

    void begin() { server.begin(); }
    void handleClient() { server.handleClient(); }
};
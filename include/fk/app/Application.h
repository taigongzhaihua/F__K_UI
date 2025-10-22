#pragma once

#include <string>
#include <unordered_map>

#include "fk/core/Event.h"
#include "fk/ui/Window.h"

namespace fk {

class Application {
public:
    Application();
    virtual ~Application();

    static Application* Current();

    void Run();
    void Shutdown();

    bool IsRunning() const;

    void AddWindow(const WindowPtr& window, const std::string& name);
    void RemoveWindow(const WindowPtr& window);
    void RemoveWindow(const std::string& name);

    core::Event<> Startup;
    core::Event<> Exit;
    core::Event<> Activated;
    core::Event<> Deactivated;

    const std::unordered_map<std::string, WindowPtr>& Windows() const { return windows_; }
    WindowPtr GetWindow(const std::string& name) const;

private:
    static Application* instance_;
    std::unordered_map<std::string, WindowPtr> windows_;
    bool isRunning_;
};

} // namespace fk

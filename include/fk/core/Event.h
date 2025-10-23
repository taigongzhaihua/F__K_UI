#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <utility>
#include <vector>

namespace fk::core {

template<typename... Args>
class Event {
public:
    using Handler = std::function<void(Args...)>;

private:
    struct ListenerEntry {
        std::size_t id;
        int priority;
        bool once;
        Handler handler;
    };

    struct State {
        mutable std::shared_mutex mutex;
        std::vector<ListenerEntry> listeners;
    };

public:
    class Connection {
    public:
        Connection() = default;

        Connection(std::weak_ptr<State> state, std::size_t id) noexcept
            : state_(std::move(state)), id_(id) {}

        Connection(Connection&& other) noexcept
            : state_(std::move(other.state_)), id_(other.id_) {
            other.id_ = 0;
        }

        Connection& operator=(Connection&& other) noexcept {
            if (this != &other) {
                Disconnect();
                state_ = std::move(other.state_);
                id_ = other.id_;
                other.id_ = 0;
            }
            return *this;
        }

        Connection(const Connection&) = delete;
        Connection& operator=(const Connection&) = delete;

        ~Connection() { Disconnect(); }

        void Disconnect() {
            if (id_ == 0) {
                state_.reset();
                return;
            }
            if (auto state = state_.lock()) {
                std::unique_lock lock(state->mutex);
                auto& listeners = state->listeners;
                listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                    [&](const auto& entry) { return entry.id == id_; }), listeners.end());
            }
            state_.reset();
            id_ = 0;
        }

        bool IsConnected() const {
            if (id_ == 0) {
                return false;
            }
            if (auto state = state_.lock()) {
                std::shared_lock lock(state->mutex);
                const auto& listeners = state->listeners;
                return std::any_of(listeners.begin(), listeners.end(),
                    [&](const auto& entry) { return entry.id == id_; });
            }
            return false;
        }

    private:
        std::weak_ptr<State> state_{};
        std::size_t id_{0};
    };

    Event() : state_(std::make_shared<State>()) {}

    void Add(const Handler& handler, int priority = 0) {
        AddInternal(Handler(handler), priority, false);
    }

    void Remove(const Handler& handler) {
        auto state = state_;
        std::unique_lock lock(state->mutex);
        auto& listeners = state->listeners;
        listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
            [&](const auto& entry) { return AreEquivalent(entry.handler, handler); }), listeners.end());
    }

    void RemoveAll() {
        auto state = state_;
        std::unique_lock lock(state->mutex);
        state->listeners.clear();
    }

    void operator+=(const Handler& handler) { Add(handler); }
    void operator-=(const Handler& handler) { Remove(handler); }

    template<typename Callable>
    Connection Connect(Callable&& callable, int priority = 0) {
        Handler handler(std::forward<Callable>(callable));
        const auto id = AddInternal(std::move(handler), priority, false);
        return Connection(state_, id);
    }

    Connection Connect(const Handler& handler, int priority = 0) {
        const auto id = AddInternal(Handler(handler), priority, false);
        return Connection(state_, id);
    }

    template<typename Callable>
    Connection ConnectOnce(Callable&& callable, int priority = 0) {
        Handler handler(std::forward<Callable>(callable));
        const auto id = AddInternal(std::move(handler), priority, true);
        return Connection(state_, id);
    }

    Connection ConnectOnce(const Handler& handler, int priority = 0) {
        const auto id = AddInternal(Handler(handler), priority, true);
        return Connection(state_, id);
    }

    void operator()(Args... args) const {
        std::vector<ListenerEntry> snapshot;
        auto state = state_;
        {
            std::shared_lock lock(state->mutex);
            snapshot = state->listeners;
        }

        std::vector<std::size_t> onceIds;
        onceIds.reserve(snapshot.size());
        for (const auto& entry : snapshot) {
            if (entry.handler) {
                entry.handler(args...);
                if (entry.once) {
                    onceIds.push_back(entry.id);
                }
            }
        }

        if (!onceIds.empty()) {
            std::sort(onceIds.begin(), onceIds.end());
            onceIds.erase(std::unique(onceIds.begin(), onceIds.end()), onceIds.end());

            std::unique_lock lock(state->mutex);
            auto& listeners = state->listeners;
            listeners.erase(std::remove_if(listeners.begin(), listeners.end(),
                [&](const auto& entry) {
                    return std::binary_search(onceIds.begin(), onceIds.end(), entry.id);
                }), listeners.end());
        }
    }

private:
    std::size_t AddInternal(Handler handler, int priority, bool once) {
        if (!handler) {
            return 0;
        }

        const auto id = nextId_.fetch_add(1, std::memory_order_relaxed);
        ListenerEntry entry{id, priority, once, std::move(handler)};

        auto state = state_;
        {
            std::unique_lock lock(state->mutex);
            auto& listeners = state->listeners;
            const auto insertPosition = std::find_if(listeners.begin(), listeners.end(),
                [&](const auto& existing) { return entry.priority > existing.priority; });
            listeners.insert(insertPosition, std::move(entry));
        }

        return id;
    }

    static bool AreEquivalent(const Handler& lhs, const Handler& rhs) {
        if (!lhs || !rhs) {
            return false;
        }
        if (lhs.target_type() != rhs.target_type()) {
            return false;
        }
        if (auto lhsPtr = lhs.template target<void(*)()>(); lhsPtr) {
            if (auto rhsPtr = rhs.template target<void(*)()>(); rhsPtr) {
                return *lhsPtr == *rhsPtr;
            }
        }
        return true;
    }

    std::shared_ptr<State> state_;
    std::atomic<std::size_t> nextId_{1};
};

} // namespace fk::core

namespace fk {
    using core::Event;
}

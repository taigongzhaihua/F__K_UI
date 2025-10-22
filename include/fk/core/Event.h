#pragma once

#include <functional>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>
#include <cstddef>
#include <utility>

namespace fk::core {

template<typename... Args>
class Event {
public:
    using Handler = std::function<void(Args...)>;

    class Connection {
    public:
        Connection() = default;

        Connection(std::weak_ptr<std::vector<std::pair<std::size_t, Handler>>> state,
            std::size_t id) noexcept
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
                return;
            }
            if (auto state = state_.lock()) {
                state->erase(std::remove_if(state->begin(), state->end(),
                    [&](const auto& entry) { return entry.first == id_; }), state->end());
            }
            state_.reset();
            id_ = 0;
        }

        bool IsConnected() const {
            return id_ != 0 && !state_.expired();
        }

    private:
        std::weak_ptr<std::vector<std::pair<std::size_t, Handler>>> state_{};
        std::size_t id_{0};
    };

    Event()
        : listeners_(std::make_shared<std::vector<std::pair<std::size_t, Handler>>>()) {}

    void Add(const Handler& h) { AddInternal(Handler(h)); }

    void Remove(const Handler& h) {
        auto state = listeners_;
        state->erase(std::remove_if(state->begin(), state->end(),
            [&](const auto& entry) { return AreEquivalent(entry.second, h); }), state->end());
    }

    void RemoveAll() { listeners_->clear(); }

    void operator+=(const Handler& h) { Add(h); }
    void operator-=(const Handler& h) { Remove(h); }

    template<typename Callable>
    Connection Connect(Callable&& callable) {
        Handler handler(std::forward<Callable>(callable));
        const auto id = AddInternal(std::move(handler));
        return Connection(listeners_, id);
    }

    Connection Connect(const Handler& handler) {
        const auto id = AddInternal(Handler(handler));
        return Connection(listeners_, id);
    }

    void operator()(Args... args) const {
        std::vector<Handler> snapshot;
        snapshot.reserve(listeners_->size());
        for (const auto& entry : *listeners_) {
            if (entry.second) {
                snapshot.push_back(entry.second);
            }
        }
        for (auto& handler : snapshot) {
            handler(args...);
        }
    }

private:
    std::size_t AddInternal(Handler handler) {
        const auto id = nextId_++;
        listeners_->emplace_back(id, std::move(handler));
        return id;
    }

    static bool AreEquivalent(const Handler& lhs, const Handler& rhs) {
        if (lhs.target_type() != rhs.target_type()) {
            return false;
        }
        return true;
    }

    std::shared_ptr<std::vector<std::pair<std::size_t, Handler>>> listeners_;
    std::size_t nextId_{1};
};

} // namespace fk::core

namespace fk {
    using core::Event;
}

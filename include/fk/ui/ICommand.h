#pragma once

#include "fk/core/Event.h"

#include <any>
#include <functional>
#include <memory>

namespace fk::ui {

class ICommand {
public:
    using Ptr = std::shared_ptr<ICommand>;

    virtual ~ICommand() = default;

    virtual bool CanExecute(const std::any& parameter) const = 0;
    virtual void Execute(const std::any& parameter) = 0;

    virtual core::Event<>& CanExecuteChanged() = 0;
};

class CommandBase : public ICommand {
public:
    ~CommandBase() override = default;

    core::Event<>& CanExecuteChanged() override { return canExecuteChanged_; }

protected:
    void RaiseCanExecuteChanged() { canExecuteChanged_(); }

private:
    core::Event<> canExecuteChanged_{};
};

class RelayCommand : public CommandBase {
public:
    using ExecuteHandler = std::function<void(const std::any&)>;
    using CanExecuteHandler = std::function<bool(const std::any&)>;

    RelayCommand(ExecuteHandler execute, CanExecuteHandler canExecute = CanExecuteHandler{})
        : execute_(std::move(execute))
        , canExecute_(std::move(canExecute)) {}

    bool CanExecute(const std::any& parameter) const override {
        if (!canExecute_) {
            return true;
        }
        return canExecute_(parameter);
    }

    void Execute(const std::any& parameter) override {
        if (!execute_) {
            return;
        }
        execute_(parameter);
    }

    void RaiseCanExecuteChanged() { CommandBase::RaiseCanExecuteChanged(); }

private:
    ExecuteHandler execute_{};
    CanExecuteHandler canExecute_{};
};

inline std::shared_ptr<RelayCommand> relayCommand(
    RelayCommand::ExecuteHandler execute,
    RelayCommand::CanExecuteHandler canExecute = RelayCommand::CanExecuteHandler{}) {
    return std::make_shared<RelayCommand>(std::move(execute), std::move(canExecute));
}

} // namespace fk::ui

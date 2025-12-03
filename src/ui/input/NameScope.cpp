#include "fk/ui/input/NameScope.h"
#include "fk/binding/DependencyObject.h"

namespace fk::ui {

const std::string NameScopeProperty::Key = "__NameScope__";

bool NameScope::RegisterName(const std::string& name, fk::binding::DependencyObject* object) {
    if (name.empty() || !object) {
        return false;
    }
    
    // 检查名称是否已存在
    auto it = names_.find(name);
    if (it != names_.end()) {
        // 名称已存�?
        return false;
    }
    
    // 注册名称
    names_[name] = object;
    return true;
}

void NameScope::UnregisterName(const std::string& name) {
    names_.erase(name);
}

fk::binding::DependencyObject* NameScope::FindName(const std::string& name) const {
    auto it = names_.find(name);
    if (it != names_.end()) {
        return it->second;
    }
    return nullptr;
}

bool NameScope::UpdateName(const std::string& oldName, const std::string& newName, 
                          fk::binding::DependencyObject* object) {
    if (newName.empty() || !object) {
        return false;
    }
    
    // 如果旧名称和新名称相同，不需要更�?
    if (oldName == newName) {
        return true;
    }
    
    // 检查新名称是否已被其他对象占用
    auto it = names_.find(newName);
    if (it != names_.end() && it->second != object) {
        // 新名称已被其他对象使�?
        return false;
    }
    
    // 删除旧名�?
    if (!oldName.empty()) {
        names_.erase(oldName);
    }
    
    // 注册新名�?
    names_[newName] = object;
    return true;
}

bool NameScope::Contains(const std::string& name) const {
    return names_.find(name) != names_.end();
}

void NameScope::Clear() {
    names_.clear();
}

} // namespace fk::ui

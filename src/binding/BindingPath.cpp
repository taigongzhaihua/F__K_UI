#include "fk/binding/BindingPath.h"

#include <charconv>
#include <sstream>

namespace fk::binding {

namespace {

struct RegistryStorage {
    std::unordered_map<std::type_index, std::unordered_map<std::string, PropertyAccessorRegistry::Accessor>> accessors;
    std::mutex mutex;
};

RegistryStorage& GetStorage() {
    static RegistryStorage storage;
    return storage;
}

bool TryParseIndex(std::string_view token, std::size_t& outIndex) {
    const auto begin = token.data();
    const auto end = begin + token.size();
    std::size_t value = 0;
    if (const auto result = std::from_chars(begin, end, value); result.ec == std::errc{}) {
        outIndex = value;
        return true;
    }
    return false;
}

std::vector<std::any>* TryGetMutableVector(std::any& holder) {
    if (auto* vec = std::any_cast<std::vector<std::any>>(&holder)) {
        return vec;
    }
    if (auto* ptr = std::any_cast<std::vector<std::any>*>(&holder)) {
        return ptr ? *ptr : nullptr;
    }
    if (auto* sharedPtr = std::any_cast<std::shared_ptr<std::vector<std::any>>>(&holder)) {
        return sharedPtr ? sharedPtr->get() : nullptr;
    }
    if (auto* ref = std::any_cast<std::reference_wrapper<std::vector<std::any>>>(&holder)) {
        return ref ? &ref->get() : nullptr;
    }
    return nullptr;
}

const std::vector<std::any>* TryGetVector(const std::any& holder) {
    if (const auto* vec = std::any_cast<const std::vector<std::any>>(&holder)) {
        return vec;
    }
    if (const auto* vec = std::any_cast<std::vector<std::any>>(&const_cast<std::any&>(holder))) {
        return vec;
    }
    if (const auto* ptr = std::any_cast<const std::vector<std::any>*>(&holder)) {
        return ptr ? *ptr : nullptr;
    }
    if (const auto* ptr = std::any_cast<std::vector<std::any>*>(&const_cast<std::any&>(holder))) {
        return ptr ? *ptr : nullptr;
    }
    if (const auto* sharedPtr = std::any_cast<std::shared_ptr<const std::vector<std::any>>>(&holder)) {
        return sharedPtr ? sharedPtr->get() : nullptr;
    }
    if (const auto* sharedPtr = std::any_cast<std::shared_ptr<std::vector<std::any>>>(&const_cast<std::any&>(holder))) {
        return sharedPtr ? sharedPtr->get() : nullptr;
    }
    if (const auto* ref = std::any_cast<std::reference_wrapper<const std::vector<std::any>>>(&holder)) {
        return ref ? &ref->get() : nullptr;
    }
    if (const auto* ref = std::any_cast<std::reference_wrapper<std::vector<std::any>>>(&const_cast<std::any&>(holder))) {
        return ref ? &ref->get() : nullptr;
    }
    return nullptr;
}

} // namespace

void PropertyAccessorRegistry::RegisterAccessor(std::type_index ownerType, std::string name, const Accessor& accessor) {
    auto& storage = GetStorage();
    std::lock_guard lock(storage.mutex);
    auto& ownerMap = storage.accessors[ownerType];
    auto& entry = ownerMap[std::move(name)];
    if (accessor.getter) {
        entry.getter = accessor.getter;
    }
    if (accessor.setter) {
        entry.setter = accessor.setter;
    }
}

const PropertyAccessorRegistry::Accessor* PropertyAccessorRegistry::FindAccessor(std::type_index ownerType, std::string_view name) {
    auto& storage = GetStorage();
    std::lock_guard lock(storage.mutex);
    auto ownerIt = storage.accessors.find(ownerType);
    if (ownerIt == storage.accessors.end()) {
        return nullptr;
    }
    auto accessorIt = ownerIt->second.find(std::string{name});
    if (accessorIt == ownerIt->second.end()) {
        return nullptr;
    }
    return &accessorIt->second;
}

BindingPath::BindingPath(std::string path)
    : raw_(std::move(path))
    , segments_(Parse(raw_)) {}

bool BindingPath::Resolve(const std::any& source, std::any& result) const {
    if (segments_.empty()) {
        result = source;
        return true;
    }

    std::any current = source;

    for (const auto& segment : segments_) {
        if (!current.has_value()) {
            return false;
        }

        if (segment.kind == Segment::Kind::Property) {
            const auto type = std::type_index(current.type());
            const auto* accessor = PropertyAccessorRegistry::FindAccessor(type, segment.name);
            if (!accessor || !accessor->getter) {
                return false;
            }

            std::any nextValue;
            if (!accessor->getter(current, nextValue)) {
                return false;
            }

            current = std::move(nextValue);
        } else {
            if (const auto* vectorPtr = std::any_cast<const std::vector<std::any>>(&current)) {
                if (segment.index >= vectorPtr->size()) {
                    return false;
                }
                current = (*vectorPtr)[segment.index];
            } else if (const auto* vectorPtr = std::any_cast<std::vector<std::any>>(&current)) {
                if (segment.index >= vectorPtr->size()) {
                    return false;
                }
                current = (*vectorPtr)[segment.index];
            } else {
                return false;
            }
        }
    }

    result = current;
    return true;
}

bool BindingPath::SetValue(std::any& source, const std::any& value) const {
    if (segments_.empty()) {
        source = value;
        return true;
    }

    std::vector<std::any> nestedValues;
    nestedValues.reserve(segments_.size());

    std::any* current = &source;

    for (std::size_t i = 0; i < segments_.size(); ++i) {
        const auto& segment = segments_[i];
        const bool isLast = (i + 1 == segments_.size());

        if (segment.kind == Segment::Kind::Property) {
            const auto type = std::type_index(current->type());
            const auto* accessor = PropertyAccessorRegistry::FindAccessor(type, segment.name);
            if (!accessor) {
                return false;
            }

            if (isLast) {
                if (!accessor->setter) {
                    return false;
                }
                return accessor->setter(*current, value);
            }

            if (!accessor->getter) {
                return false;
            }

            nestedValues.emplace_back();
            std::any& next = nestedValues.back();
            if (!accessor->getter(*current, next)) {
                return false;
            }

            current = &next;
            continue;
        }

        if (!current->has_value()) {
            return false;
        }

        if (auto* mutableVector = TryGetMutableVector(*current)) {
            if (segment.index >= mutableVector->size()) {
                return false;
            }

            if (isLast) {
                (*mutableVector)[segment.index] = value;
                return true;
            }

            current = &(*mutableVector)[segment.index];
            continue;
        }

        const auto* vectorView = TryGetVector(*current);
        if (!vectorView) {
            return false;
        }
        if (segment.index >= vectorView->size()) {
            return false;
        }
        if (isLast) {
            return false;
        }

        nestedValues.emplace_back((*vectorView)[segment.index]);
        current = &nestedValues.back();
    }

    return false;
}

std::vector<BindingPath::Segment> BindingPath::Parse(const std::string& path) {
    std::vector<Segment> segments;
    if (path.empty()) {
        return segments;
    }

    std::string token;
    for (std::size_t i = 0; i < path.size(); ++i) {
        const char ch = path[i];
        if (ch == '.') {
            if (!token.empty()) {
                segments.push_back(MakePropertySegment(token));
                token.clear();
            }
        } else if (ch == '[') {
            if (!token.empty()) {
                segments.push_back(MakePropertySegment(token));
                token.clear();
            }

            std::size_t indexStart = i + 1;
            const auto closing = path.find(']', indexStart);
            if (closing == std::string::npos) {
                throw std::invalid_argument("Missing closing bracket in binding path");
            }

            const std::string_view indexToken(&path[indexStart], closing - indexStart);
            segments.push_back(MakeIndexSegment(indexToken));
            i = closing;
        } else {
            token.push_back(ch);
        }
    }

    if (!token.empty()) {
        segments.push_back(MakePropertySegment(token));
    }

    return segments;
}

BindingPath::Segment BindingPath::MakePropertySegment(const std::string& token) {
    if (token.empty()) {
        throw std::invalid_argument("Empty property name in binding path");
    }
    return Segment::Property(token);
}

BindingPath::Segment BindingPath::MakeIndexSegment(std::string_view token) {
    if (token.empty()) {
        throw std::invalid_argument("Empty indexer in binding path");
    }

    std::size_t index = 0;
    if (!TryParseIndex(token, index)) {
        throw std::invalid_argument("Invalid numeric index in binding path");
    }

    return Segment::Index(index);
}

} // namespace fk::binding

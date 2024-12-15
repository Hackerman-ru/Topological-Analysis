#include "base.h"

Base::Base(Id id) : id(id) {};

std::string Base::to_string() const {
    return std::to_string(id);
}

Id Base::get_id() const {
    return id;
}

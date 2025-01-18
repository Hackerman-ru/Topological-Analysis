#include "vertices/base.h"

Base::Base(Id id) : m_id(id) {};

std::string Base::to_string() const {
    return std::to_string(m_id);
}

Id Base::get_id() const {
    return m_id;
}

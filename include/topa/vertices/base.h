#ifndef __BASE_H__
#define __BASE_H__

#include "../core/util.h"

#include <compare>
#include <string>

struct Base {
    Id id;

    Base(Id id);

    friend std::strong_ordering operator<=>(const Base& lhs, const Base& rhs) = default;

    Id get_id() const;
    virtual std::string to_string() const;
    virtual ~Base();
};

#endif

#include "either.hpp"

#include <fmt/core.h>

#include <cassert>
#include <memory>

using size_t = std::size_t;
using u_ptr = std::unique_ptr<int>;
using s_ptr = std::shared_ptr<int>;

either<u_ptr, std::string> make_value() {
    return "This is a string, not a pointer";
}

either<s_ptr, std::string> make_ptr() {
    either<s_ptr, std::string> out(std::make_unique<int>(69));
    return out;
}

class point {
  public:
    point(double _x, double _y) : x(_x), y(_y) {};
    double x;
    double y;
};

int main(int argc, char* argv[]) {
    either<int, std::string> v("Boo!");
    either<int, std::string> u(v);
    fmt::print("u holds value: {}\n", u.as<std::string>());

    either<int, point> ep(std::in_place_type<point>, 0.1, 0.2);
    ep.emplace<point>(0.3, 0.4);

    u.emplace<std::string>("Hello world!");
    fmt::print("u holds value: {}\n", u.as<std::string>());
    fmt::
        print("u is is_copy_constructible? {}\n", std::is_copy_constructible_v<decltype(u)>);

    // copy elision
    auto w = make_value();
    fmt::print("w holds value: {}\n", w.as<std::string>());
    w = decltype(w)("Another string");
    fmt::print("w holds value: {}\n", w.as<std::string>());
    // copy elision
    auto p = make_ptr();
    fmt::print("p holds value: {}\n", *p.as<s_ptr>());

    // compilation error: u_ptr can't be copied
    // auto z = w;

    // but moving is fine
    auto z = std::move(w);
    fmt::print("z holds value: {}\n", z.as<std::string>());
    fmt::
        print("z is is_copy_constructible? {}\n", std::is_copy_constructible_v<decltype(z)>);

    // w is now in an undefined state
    try {
        fmt::print("w holds value: {}\n", w.as<std::string>());
    } catch (std::bad_variant_access const& e) {
        fmt::print("Error accessing the value of w: {}\n", e.what());
    }

    either<u_ptr, float> q(3.14f);
    double d = q.as<float>();
    double f = q.as<float, double>();
    fmt::print("q: {}\n", d);
}

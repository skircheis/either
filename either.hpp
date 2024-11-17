#include <fmt/core.h>

#include <type_traits>
#include <variant>

#define _EITHER_X_IN_TS(X, TS) std::disjunction_v<std::is_same<X, TS>...>

#define _EITHER_ENABLE_IF_IN(X, TS) \
    std::enable_if_t<_EITHER_X_IN_TS(X, TS), bool> = true

#define _EITHER_ENABLE_IF_CONVERTIBLE(X, Ts)                   \
    template <                                                 \
        typename X,                                            \
        std::enable_if_t<                                      \
            std::disjunction_v<std::is_convertible<X, Ts>...>, \
            bool> = true>

// adding the dummy template parameter X forces the inference of the second
// template parameter see:
// -
// https://stackoverflow.com/questions/13401716/selecting-a-member-function-using-different-enable-if-conditions
// -
// https://stackoverflow.com/questions/6972368/stdenable-if-to-conditionally-compile-a-member-function
#define _EITHER_ENABLE_IF_PREDICATE_FOR_TS(TS, PREDICATE) \
    template <                                            \
        typename X = std::monostate,                      \
        std::enable_if_t<std::conjunction_v<PREDICATE<TS>..., PREDICATE<X>>>>

template <typename... Ts>
class either {
  public:
    _EITHER_ENABLE_IF_CONVERTIBLE(X, Ts)
    constexpr either(X const& value) : _var(value) {};

    _EITHER_ENABLE_IF_CONVERTIBLE(X, Ts)
    constexpr either(X&& value) : _var(std::forward<X>(value)) {};

    template <typename X, typename... Cs>
    constexpr explicit either(std::in_place_type_t<X> t, Cs&&... args) :
        _var(t, args...){};

    _EITHER_ENABLE_IF_PREDICATE_FOR_TS(Ts, std::is_copy_constructible)
    constexpr either(either const& that) { _var = that._var; };

    _EITHER_ENABLE_IF_PREDICATE_FOR_TS(Ts, std::is_move_constructible)
    constexpr either(either&& that) { std::swap(this->_var, that._var); };

    _EITHER_ENABLE_IF_PREDICATE_FOR_TS(Ts, std::is_copy_assignable)
    constexpr either& operator=(const either& that) {
        _var = that._var;
        return *this;
    };

    _EITHER_ENABLE_IF_PREDICATE_FOR_TS(Ts, std::is_move_assignable)
    constexpr either& operator=(either&& that) {
        std::swap(this->_var, that._var);
        return *this;
    }

    template <typename X, _EITHER_ENABLE_IF_IN(X, Ts)>
    [[nodiscard]] constexpr bool is() const noexcept {
        return std::holds_alternative<X>(_var);
    };

    template <typename X, _EITHER_ENABLE_IF_IN(X, Ts)>
    [[nodiscard]] constexpr bool is(X const& x) const noexcept {
        return is<X>() && as<X>() == x;
    };

    template <typename X, _EITHER_ENABLE_IF_IN(X, Ts)>
    [[nodiscard]] constexpr const X& as() const {
        return std::get<X>(_var);
    };

    template <
        typename X,
        typename Y,
        _EITHER_ENABLE_IF_IN(X, Ts),
        std::enable_if_t<std::is_convertible_v<Y, X>, bool> = true>
    [[nodiscard]] constexpr Y as() const {
        return std::get<X>(_var);
    }

    template <
        typename X,
        typename... Cs,
        _EITHER_ENABLE_IF_IN(X, Ts),
        std::enable_if_t<std::is_constructible_v<X, Cs...>, bool> = true>
    X const& emplace(Cs... args) {
        return _var.template emplace<X>(args...);
    };

  private:
    either();
    std::variant<Ts...> _var;
};

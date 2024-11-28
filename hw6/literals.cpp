#include <type_traits>

template <char... c>
struct TString {
    template <char... other>
    constexpr auto operator+(TString<other...>) const {
        return TString<c..., other...>{};
    }

    template <char... other>
    constexpr bool operator==(TString<other...>) const {
        return std::is_same<TString<c...>, TString<other...>>::value;
    }
};

template <typename CharT, CharT... c>
constexpr TString<c...> operator"" _s() {
    return TString<c...>{};
}

int main() {
    constexpr auto hello = "hello"_s + " world"_s;
    static_assert(hello == "hello world"_s);

    return 0;
}


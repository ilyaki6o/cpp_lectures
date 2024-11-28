#include <exception>
#include <sstream>
#include <string>
#include <gtest/gtest.h>

class TMyException : public std::exception {
private:
    std::string message;

public:
    TMyException() = default;

    const char* what() const noexcept override {
        return message.c_str();
    }

    template <typename T>
    TMyException& operator<<(const T& data) {
        std::ostringstream stream;

        stream << message << data;
        message = stream.str();

        return *this;
    }
};


class TMyExceptionDerived1 : public TMyException {
public:
    TMyExceptionDerived1() = default;
};


class TMyExceptionDerived2 : public TMyException {
public:
    TMyExceptionDerived2() = default;
};


TEST(TMyExceptionTest, ThrowsBaseException) {
    EXPECT_THROW(
        throw TMyException() << "Base exception occurred",
        TMyException
    );
}

TEST(TMyExceptionTest, ThrowsDerived1Exception) {
    EXPECT_THROW(
        throw TMyExceptionDerived1() << "Derived1 exception occurred",
        TMyException
    );
}

TEST(TMyExceptionTest, ThrowsDerived2Exception) {
    EXPECT_THROW(
        throw TMyExceptionDerived2() << "Derived2 exception occurred",
        TMyException
    );
}

// Тест для проверки содержимого сообщения
TEST(TMyExceptionTest, ExceptionMessageContent) {
    try {
        throw TMyException() << "Error: " << 42;
    } catch (const TMyException& ex) {
        EXPECT_STREQ(ex.what(), "Error: 42");
    }
}

int main(int argc, char** argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

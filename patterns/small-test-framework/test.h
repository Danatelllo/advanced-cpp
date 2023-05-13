#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
#include <exception>

class AbstractTest {
public:
    virtual void SetUp() = 0;
    virtual void TearDown() = 0;
    virtual void Run() = 0;
    virtual ~AbstractTest() {
    }
};

class Container {
public:
    Container() = default;
    ~Container() = default;
    virtual std::unique_ptr<AbstractTest> Do() = 0;
};

template <typename T>
class TemplateContainer : public Container {
public:
    TemplateContainer() {
    }
    std::unique_ptr<AbstractTest> Do() override {
        return std::make_unique<T>();
    }
    //    T instance;
    ~TemplateContainer() = default;
};

class Substr {
public:
    bool operator()(std::string_view str) const {
        if (str.find(s) >= str.size()) {
            return false;
        }
        return true;
    }

    Substr(std::string str) {
        s = str;
    }
    std::string_view s;
};

class FullMatch {
public:
    bool operator()(std::string_view str) const {
        return str == s;
    }

    FullMatch(std::string_view str) {
        s = str;
    }
    std::string_view s;
};

class TestRegistry {
public:
    template <class TestClass>
    void RegisterClass(const std::string& class_name) {
        data[class_name] = std::make_unique<TemplateContainer<TestClass>>();
    }

    std::unique_ptr<AbstractTest> CreateTest(const std::string& class_name) {
        std::unique_ptr<AbstractTest> a;
        if (!data.contains(class_name)) {
            throw std::out_of_range("blah");
        }
        try {
            a = data[class_name]->Do();
        } catch (...) {
            throw;
        }
        return a;
    }

    void RunTest(const std::string& test_name) {
        auto a = data[test_name]->Do();
        a->SetUp();
        try {
            a->Run();
        } catch (...) {
            std::exception_ptr b = std::current_exception();
            a->TearDown();
            rethrow_exception(b);
        }
        a->TearDown();
    }

    template <class Predicate>
    std::vector<std::string> ShowTests(Predicate callback) const {
        std::vector<std::string> tests;
        for (auto& s : data) {
            if (callback(s.first)) {
                tests.emplace_back(s.first);
            }
        }
        return tests;
    }

    std::vector<std::string> ShowAllTests() const {
        std::vector<std::string> tests;
        for (auto& s : data) {
            tests.emplace_back(s.first);
        }
        return tests;
    }

    template <class Predicate>
    void RunTests(Predicate callback) {
        for (const auto& ch : data) {
            RunTest(ch.first);
        }
    }

    void Clear() {
        data.clear();
    }

    std::map<std::string, std::unique_ptr<Container>> data;
    static TestRegistry& Instance() {
        static TestRegistry* instance = new TestRegistry();
        return *instance;
    }
};

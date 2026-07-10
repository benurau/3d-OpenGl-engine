#pragma once

#ifndef NDEBUG

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>

#define ASSERT(condition, message) \
    do { \
        if (!(condition)) { \
            std::ostringstream _test_msg; \
            _test_msg << __FILE__ << ":" << __LINE__ << " FAILED: " << message; \
            throw std::runtime_error(_test_msg.str()); \
        } \
    } while(0)

#define ASSERT_NEAR(a, b, eps, message) \
    ASSERT(std::abs((a) - (b)) < (eps), message)

#define ASSERT_TRUE(condition, message) ASSERT(condition, message)
#define ASSERT_FALSE(condition, message) ASSERT(!(condition), message)

class TestRunner {
public:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
    };

    void add(const std::string& name, std::function<bool()> func) {
        tests.push_back({ name, func });
    }

    bool runAll() {
        results.clear();
        passedCount = 0;
        failedCount = 0;

        std::cout << "\n========================================\n";
        std::cout << "  3D OpenGL Engine - Simulation Tests\n";
        std::cout << "========================================\n\n";

        for (size_t i = 0; i < tests.size(); ++i) {
            auto& test = tests[i];
            std::cout << "[" << (i + 1) << "/" << tests.size() << "] " << test.first << "... ";
            std::cout.flush();

            try {
                if (test.second()) {
                    std::cout << "PASS\n";
                    results.push_back({ test.first, true, "" });
                    passedCount++;
                } else {
                    std::cout << "FAIL (returned false)\n";
                    results.push_back({ test.first, false, "Test returned false" });
                    failedCount++;
                }
            } catch (const std::exception& e) {
                std::cout << "FAIL\n";
                std::cout << "       " << e.what() << "\n";
                results.push_back({ test.first, false, e.what() });
                failedCount++;
            }
        }

        std::cout << "\n========================================\n";
        std::cout << "  Results: " << passedCount << " passed, "
                  << failedCount << " failed, "
                  << (passedCount + failedCount) << " total\n";
        std::cout << "========================================\n\n";

        return failedCount == 0;
    }

private:
    std::vector<std::pair<std::string, std::function<bool()>>> tests;
    std::vector<TestResult> results;
    int passedCount = 0;
    int failedCount = 0;
};

#endif // NDEBUG

#include <iostream>
#include <unordered_map>
#include <chrono>
#include <mutex>
#include <memory>
#include <string>
#include<thread>

using namespace std;
using namespace std::chrono;


class RateLimitAlgorithm {
public:
    virtual bool allowRequest() = 0;
    virtual ~RateLimitAlgorithm() = default;
};

class TokenBucket : public RateLimitAlgorithm {
private:
    int capacity;
    int tokens;
    int refillRatePerSec;
    time_point<steady_clock> lastRefillTime;
    mutex mtx;

public:
    TokenBucket(int capacity, int refillRatePerSec)
        : capacity(capacity), tokens(capacity), refillRatePerSec(refillRatePerSec),
          lastRefillTime(steady_clock::now()) {}

    bool allowRequest() override {
        lock_guard<mutex> lock(mtx);
        refillTokens();

        if (tokens > 0) {
            tokens--;
            return true;
        }
        return false;
    }

private:
    void refillTokens() {
        auto now = steady_clock::now();
        auto secondsPassed = duration_cast<seconds>(now - lastRefillTime).count();

        if (secondsPassed > 0) {
            int tokensToAdd = secondsPassed * refillRatePerSec;
            tokens = min(capacity, tokens + tokensToAdd);
            lastRefillTime = now;
        }
    }
};

class UserBucket {
private:
    unique_ptr<RateLimitAlgorithm> algorithm;

public:
    UserBucket(int capacity, int refillRatePerSec) {
        algorithm = make_unique<TokenBucket>(capacity, refillRatePerSec);
    }

    bool allowRequest() {
        return algorithm->allowRequest();
    }
};

class RateLimiter {
private:
    unordered_map<string, shared_ptr<UserBucket>> userBuckets;
    int capacity;
    int refillRatePerSec;
    mutex globalMutex;

public:
    RateLimiter(int capacity, int refillRatePerSec)
        : capacity(capacity), refillRatePerSec(refillRatePerSec) {}

    bool isRequestAllowed(const string& userId) {
        lock_guard<mutex> lock(globalMutex);

        if (userBuckets.find(userId) == userBuckets.end()) {
            userBuckets[userId] = make_shared<UserBucket>(capacity, refillRatePerSec);
        }

        return userBuckets[userId]->allowRequest();
    }
};

int main() {
    RateLimiter limiter(5, 1);
    string user = "user123";

    cout << "Sending 7 rapid requests:\n";
    for (int i = 1; i <= 7; ++i) {
        bool allowed = limiter.isRequestAllowed(user);
        cout << "Request " << i << ": " << (allowed ? "Allowed" : "Blocked") << "\n";
        this_thread::sleep_for(chrono::milliseconds(300));
    }

    cout << "\nWaiting 3 seconds to refill tokens...\n";
    this_thread::sleep_for(chrono::seconds(3));

    cout << "Sending 3 more requests:\n";
    for (int i = 1; i <= 3; ++i) {
        bool allowed = limiter.isRequestAllowed(user);
        cout << "Request " << i << ": " << (allowed ? "Allowed" : "Blocked") << "\n";
    }

    return 0;
}






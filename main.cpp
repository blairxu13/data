#include <iostream>
#include <map>
#include <optional>

class InMemoryDB {
private:
    std::map<std::string, int> mainData;
    std::map<std::string, int> transactionData;
    bool inTransaction = false;

public:
    std::optional<int> get(const std::string& key) {
        if (inTransaction && transactionData.find(key) != transactionData.end()) {
            return transactionData[key];
        } else if (mainData.find(key) != mainData.end()) {
            return mainData[key];
        }
        return {};
    }

    void put(const std::string& key, int val) {
        if (!inTransaction) {
            std::cerr << "Error: No transaction in progress" << std::endl;
            return;
        }
        transactionData[key] = val;
    }

    void begin_transaction() {
        if (inTransaction) {
            std::cerr << "Error: Transaction already in progress" << std::endl;
            return;
        }
        inTransaction = true;
        transactionData.clear();
    }

    void commit() {
        if (!inTransaction) {
            std::cerr << "Error: No transaction in progress" << std::endl;
            return;
        }
        for (const auto& kv : transactionData) {
            mainData[kv.first] = kv.second;
        }
        inTransaction = false;
        transactionData.clear();
    }

    void rollback() {
        if (!inTransaction) {
            std::cerr << "Error: No transaction in progress" << std::endl;
            return;
        }
        inTransaction = false;
        transactionData.clear();
    }
};

int main() {
    InMemoryDB db;

    // Attempt to get a value for key 'A' without prior existence check
    auto value = db.get("A");
    if (value.has_value()) {
        std::cout << "Value of 'A': " << value.value() << std::endl;
    } else {
        std::cout << "Key 'A' does not exist" << std::endl;
    }

    // Attempt to put a value without a transaction
    db.put("A", 5);

    db.begin_transaction();
    db.put("A", 5);

    value = db.get("A");
    if (value.has_value()) {
        std::cout << "Value of 'A' during transaction: " << value.value() << std::endl;
    } else {
        std::cout << "Key 'A' does not exist in transaction yet" << std::endl;
    }

    db.put("A", 6);
    db.commit();

    value = db.get("A");
    if (value.has_value()) {
        std::cout << "Value of 'A' after commit: " << value.value() << std::endl;
    } else {
        std::cout << "Key 'A' does not exist after commit" << std::endl;
    }

    return 0;
}

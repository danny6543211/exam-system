#ifndef DATABASE_H
#define DATABASE_H

#include <functional>
#include <iostream>
#include <string>
#include <map>
#include <memory>
#include <vector>
#include "./sqlite3.h"

const char* DATABASE_FILE = "db.sqlite3";


struct __QuerySet {
    std::vector<std::string> columnName;
    std::vector<std::vector<std::string>> data;
};

class QuerySet {
private:
    std::unique_ptr<__QuerySet> querySet;

public:
    QuerySet(__QuerySet&& querySet) {
        this->querySet = std::make_unique<__QuerySet>(querySet);
    }

    void show() const {
        for (int i = 0; i < querySet->columnName.size(); i++) {
            std::cout << querySet->columnName[i] << "|";
        } 
        std::cout << "\n";
        
        for (auto row : querySet->data) {
            for (auto item : row) {
                std::cout << item << "|";
            }
            std::cout << "\n";
        }
    }

    const std::vector<std::string>& columnName() const {
        return querySet->columnName;
    }

    const std::vector<std::vector<std::string>>& data() const {
        return querySet->data;
    }

    const std::vector<std::string>& operator[](int i) const {
        if (i == 0)
            return querySet->columnName;
        return querySet->data[i-1];
    }
};

class Database {
private:
    sqlite3* db;

public:
    Database() {
        int rc = sqlite3_open(DATABASE_FILE, &db);
        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }

    ~Database() {
        sqlite3_close(db);
    }

    QuerySet executeQuery(const std::string& sql) {
        __QuerySet querySet;

        int rc = sqlite3_exec(
            db, sql.c_str(), [](void* data, int argc, char** argv, char** colName) -> int {
                __QuerySet* querySet = static_cast<__QuerySet*>(data);
                
                // set colName
                if (querySet->columnName.empty()) {
                    for (int i = 0; i < argc; i++) {
                        querySet->columnName.push_back(colName[i]);                        
                    }
                }
                // fill data
                std::vector<std::string> row;
                for (int i = 0; i < argc; i++) {
                    row.push_back(argv[i]);
                }
                querySet->data.push_back(row);

                return 0;
            }, &querySet, nullptr);

        if (rc != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
        
        return QuerySet(std::move(querySet));
    }
};

#endif
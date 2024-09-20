#ifndef DATABASE_H
#define DATABASE_H

#include <vector>
#include <sstream>
#include <iostream>
#include <fstream>

#include "logger.h"
#include "customer.h"
#include "btreemap.h"

using namespace std;

template <typename V>
class Database {

private:
    BTreeMap<string, 
        BTreeMap<string, 
            BTreeMap<string,
                BTreeMap<int, V>  
            >
        >
    >* data;
    
public:
    Database() {
        data = new BTreeMap<string, BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>>();
    };

    ~Database() {
        if (data != nullptr)
            delete data;
    };
    
    void AddOrUpdate(string poolName, string schemeName, string collectionName, int key, V* value) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        auto _scheme = new BTreeMap<string, BTreeMap<int, V>>();
        auto _collection = new BTreeMap<int, V>();
        V _value;
        // Добавление пустых вложенных объектов, если они еще не созданы, но запрашиваются
        if (!data->Get(poolName, *_pool)) {
            data->Add(poolName, new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>());
            data->Get(poolName, *_pool);
        }
        if (!_pool->Get(schemeName, *_scheme)) {
            _pool->Add(schemeName, new BTreeMap<string, BTreeMap<int, V>>());
            _pool->Get(schemeName, *_scheme);
        }
        if (!_scheme->Get(collectionName, *_collection)) {
            _scheme->Add(collectionName, new BTreeMap<int, V>());
            _scheme->Get(collectionName, *_collection);
        }

        if (_collection->Get(key, _value)) {
            _collection->Update(key, value);
        }
        else {
            _collection->Add(key, value);
        }

    };

    bool Get(string poolName, string schemeName, string collectionName, int key, V& value) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        auto _scheme = new BTreeMap<string, BTreeMap<int, V>>();
        auto _collection = new BTreeMap<int, V>();
        V _value;
        if (data->Get(poolName, *_pool) && _pool->Get(schemeName, *_scheme) && _scheme->Get(collectionName, *_collection)) {
            return (_collection->Get(key, value));
        }
        return false;
    };

    bool Get(string poolName, string schemeName, string collectionName, int minbound, int maxbound, vector<V*>& result) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        auto _scheme = new BTreeMap<string, BTreeMap<int, V>>();
        auto _collection = new BTreeMap<int, V>();
        if (data->Get(poolName, *_pool) && _pool->Get(schemeName, *_scheme) && _scheme->Get(collectionName, *_collection)) {
            _collection->Get(minbound, maxbound, result);
        }
        return result.size() > 0;
    };

    void Clear() {
        if (data != nullptr)
            delete data;
        data = new BTreeMap<string, BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>>();
    }

    bool RemovePool(string poolName) {
        return data->Remove(poolName);
    }

    bool RemoveScheme(string poolName, string schemeName) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        if (data->Get(poolName, *_pool))
            return _pool->Remove(schemeName);
        return false;
    }

    bool RemoveCollection(string poolName, string schemeName, string collectionName) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        auto _scheme = new BTreeMap<string, BTreeMap<int, V>>();
        if (data->Get(poolName, *_pool) && _pool->Get(schemeName, *_scheme))
            return _scheme->Remove(collectionName);
        return false;
    }

    bool RemoveValue(string poolName, string schemeName, string collectionName, int key) {
        auto _pool = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
        auto _scheme = new BTreeMap<string, BTreeMap<int, V>>();
        auto _collection = new BTreeMap<int, V>();
        if (data->Get(poolName, *_pool) && _pool->Get(schemeName, *_scheme) && _scheme->Get(collectionName, *_collection))
            return _collection->Remove(key);
        return false;
    }
    
    void LoadFromFile(const string& filename) {
        string myText;
        ifstream MyReadFile(filename);
        Clear();
        int i = -1;
        while (getline (MyReadFile, myText)) {
            vector<string> result;
            stringstream ss(myText);
            string item;
            i++;
            while (getline (ss, item, ',')) {
                result.push_back(item);
            }

            AddOrUpdate(result[0], result[1], result[2], stoi(result[3]),
                        new Customer(stoi(result[4]), result[5], stof(result[6])));
        }
        MyReadFile.close();
    }
    
    void SaveToFile(const string& filename) {
        ofstream MyFile(filename);
        vector<string> poolNames = data->Keys();


        for (string poolName : poolNames) {

            auto _poolSchemas = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
            data->Get(poolName, *_poolSchemas);
            vector<string> schemeNames = _poolSchemas->Keys();


            for (auto schemeName : schemeNames) {
                auto _schemeCollections = new BTreeMap<string, BTreeMap<int, V>>();
                _poolSchemas->Get(schemeName, *_schemeCollections);
                vector<string> collectionNames = _schemeCollections->Keys();


                for (auto collectionName : collectionNames) {

                    auto _collections = new BTreeMap<int, V>();
                    _schemeCollections->Get(collectionName, *_collections);
                    vector<int> collectionKeys = _collections->Keys();

                    for (auto collectionKey : collectionKeys) {
                        V _value;
                        _collections->Get(collectionKey, _value);

                        MyFile
                             << poolName << "," << schemeName << "," << collectionName << ","
                             << collectionKey << ","
                             << _value.getId() << ","
                             << _value.getName() << ","
                             << _value.getBalance() << "," << endl;
                    }
                }
            }
        }
        MyFile.close();
    }
    
    void Print() {
        cout << "Database:" << endl;
        vector<string> poolNames = data->Keys();

        cout << "POOLS size: "  << poolNames.size() << endl;

        for (string poolName : poolNames) {

            auto _poolSchemas = new BTreeMap<string, BTreeMap<string, BTreeMap<int, V>>>();
            data->Get(poolName, *_poolSchemas);
            vector<string> schemeNames = _poolSchemas->Keys();

            cout << "  POOL: " << poolName << " SCHEMAS size: "  << schemeNames.size() << endl;

            for (auto schemeName : schemeNames) {
                auto _schemeCollections = new BTreeMap<string, BTreeMap<int, V>>();
                _poolSchemas->Get(schemeName, *_schemeCollections);
                vector<string> collectionNames = _schemeCollections->Keys();

                cout << "    POOL: " << poolName << " SCHEMA: " << schemeName << " COLLECTIONS size: "  << collectionNames.size() << endl;

                for (auto collectionName : collectionNames) {
                    cout << "      POOL: " << poolName << " SCHEMA: " << schemeName << " COLLECTION: " << collectionName << endl;

                    auto _collections = new BTreeMap<int, V>();
                    _schemeCollections->Get(collectionName, *_collections);
                    vector<int> collectionKeys = _collections->Keys();

                    for (auto collectionKey : collectionKeys) {
                        V _value;
                        _collections->Get(collectionKey, _value);

                        cout << "        "
                               << poolName << "," << schemeName << "," << collectionName << ","
                               << collectionKey << ","
                               << _value.getId() << ","
                               << _value.getName() << ","
                               << _value.getBalance() << "," << endl;
                    }
                }
            }
        }
    }
};

#endif
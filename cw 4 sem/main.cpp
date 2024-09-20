#include<iostream>
#include "logger.h"
#include "database.h"
#include "customer.h"

int g_CommandsCount = 0;  // Кол-во найденных команд в файле

using namespace std;

void Test(Database<Customer>* db) {

    Logger::info("test - empty db");
    db->Print();

    Logger::info("test - db with 6 records");
    auto* c1 = new Customer(1, "Ivan", 123.12);
    auto* c2 = new Customer(2, "Yuri", 100.01);
    auto* c3 = new Customer(3, "Max", 23.12);
    auto* c4 = new Customer(4, "Alex", 55.12);
    auto* c5 = new Customer(5, "Kate", 11.12);
    auto* c6 = new Customer(6, "Max", 34.12);
    db->AddOrUpdate("pool1", "schema1", "collection1", 1, c1);
    db->AddOrUpdate("pool1", "schema2", "collection1", 10, c2);
    db->AddOrUpdate("pool2", "schema1", "collection3", 20, c3);
    db->AddOrUpdate("pool2", "schema1", "collection3", 20, c4);
    db->AddOrUpdate("pool1", "schema1", "collection1", 5, c3);
    db->AddOrUpdate("pool1", "schema1", "collection2", 66, c5);
    db->AddOrUpdate("pool1", "schema1", "collection2", 77, c6);
    db->Print();


    Logger::info("test - db save to file");
    db->SaveToFile("db->txt");

    Logger::info("test - db range search [10; 100]");
    vector<Customer*> result;
    if (db->Get("pool1", "schema1", "collection2", 10, 100, result)) {
        for (Customer* c : result) {
            cout << *c << endl;
        }
    }
    else {
        Logger::info("test - no items with key [10; 100] found");
    }

    Logger::info("test - db remove pool1,schema1,collection2");
    if (db->RemoveCollection("pool1", "schema1", "collection2")) {
        Logger::info("pool1,schema1,collection2 removed");
        db->Print();
    }
    else
        Logger::info("pool1,schema1,collection2 not found");

    Logger::info("test - db remove pool2,schema1");
    if (db->RemoveScheme("pool2", "schema1")) {
        Logger::info("pool2,schema1 removed");
        db->Print();
    }
    else
        Logger::info("pool2,schema1 not found");

    Logger::info("test - db remove pool4"); // Пул несуществует
    if (db->RemovePool("pool4")) {
        Logger::info("pool4 removed");
        db->Print();
    }
    else
        Logger::info("pool4 not found");

    Logger::info("test - db remove pool1"); // Пул существует
    if (db->RemovePool("pool1")) {
        Logger::info("pool1 removed");
        db->Print();
    }
    else
        Logger::error("pool1 not found");

    Logger::info("test - db LoadFromFile");
    db->LoadFromFile("db->txt");
    db->Print();

    Logger::info("test - db Get");
    if (db->Get("pool1", "schema2", "collection1", 10, *c3)) {
        cout << *c3 << endl;
    }
    else
        Logger::error("customer not found");
    if (db->Get("pool1", "schema2", "collection1", 999, *c3)) {
        cout << *c3 << endl;
    }
    else
        Logger::error("customer not found");

    delete c1;
    delete c2;
    delete c3;
    delete c4;
    delete c5;
    delete c6;
}

void ProcessCommand(Database<Customer>* db, const string& command, bool showCommand) {

    vector<string> result;
    stringstream ss(command);
    string item;

    while (getline (ss, item, ' ')) {
        result.push_back(item);
    }
    if (result.empty()) {
        return;
    }
    if (result[0] == "'") {
        return;
    }

    g_CommandsCount++;
    if (showCommand)
        Logger::info("COMMAND: " + command);

    if (result[0] == "PRINT") {
        db->Print();
    }

    else if (result[0] == "ADD") {
        db->AddOrUpdate(result[1], result[2], result[3], stoi(result[4]),
                        new Customer(stoi(result[5]), result[6], stof(result[7])));
    }

    else if (result[0] == "GET") {
        if (result.size() == 6) {
            vector<Customer*> res;
            if (db->Get(result[1], result[2], result[3], stoi(result[4]), stoi(result[5]), res)) {
                for (Customer* c : res) {
                    cout << *c << endl;
                }
            }
            else {
                cout << "No items within range found" << endl;
            }
        }
        else if (result.size() == 5) {
            auto* c = new Customer();
            if (db->Get(result[1], result[2], result[3], stoi(result[4]), *c)) {
                cout << *c << endl;
                delete c;
            }
            else
                cout << "customer not found" << endl;
        }
    }

    else if (result[0] == "DELETE") {
        if (result.size() == 5) {
            bool res = db->RemoveValue(result[1], result[2], result[3], stoi(result[4]));
            cout << "deleted: " + to_string(res) << endl;
        }
        else if (result.size() == 4) {
            bool res = db->RemoveCollection(result[1], result[2], result[3]);
            cout << "deleted: " + to_string(res) << endl;
        }
        else if (result.size() == 3) {
            bool res = db->RemoveScheme(result[1], result[2]);
            cout << "deleted: " + to_string(res) << endl;
        }
        else if (result.size() == 2) {
            bool res = db->RemovePool(result[1]);
            cout << "deleted: " + to_string(res) << endl;
        }
    }

    else if (result[0] == "LOAD") {
        db->LoadFromFile(result[1]);
    }

    else if (result[0] == "SAVE") {
        db->SaveToFile(result[1]);
    }
}

int main(int argc, char* argv[]) {
    Logger::info("Args (" + to_string(argc) + ")");
    if (argc < 2) {
        Logger::error("Provide 2 args:\n- commands filename (or --dialog to type commands manually)\n- db filename (optional)");
        return 0;
    }
    for (int i = 0; i < argc; i++) {
        Logger::info("Arg №" + to_string(i) + ": " + argv[i]);
    }

    string commandsFilename = argv[1];
    string dbFilename = (argc >= 3 ? argv[2] : "");

    auto db = new Database<Customer>();
    if (!dbFilename.empty()) {
        ofstream file(dbFilename);
        file.close();
    }
    string command;
    Logger::info("== START ==");
    if (commandsFilename != "--dialog") {
        g_CommandsCount = 0;
        ifstream MyReadFile(commandsFilename);
        while (getline (MyReadFile, command)) {

            if (!dbFilename.empty())
                db->LoadFromFile(dbFilename);

            ProcessCommand(db, command, true);

            if (!dbFilename.empty())
                db->SaveToFile(dbFilename);
        };
        MyReadFile.close();
    }
    else {
        cout << "COMMAND: ";
        while (getline(cin, command))
        {
            if (command == "EXIT")
                break;
            if (!dbFilename.empty())
                db->LoadFromFile(dbFilename);
            ProcessCommand(db, command, false);
            if (!dbFilename.empty())
                db->SaveToFile(dbFilename);
            cout << "COMMAND: ";
        }
    }

    Logger::info("COMMANDS FOUND: " + to_string(g_CommandsCount));
    Logger::info("== END ==");

    delete db;
    return 0;
}

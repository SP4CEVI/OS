#ifndef CUSTOMER_H
#define CUSTOMER_H

#include <iostream>


using namespace std;


class Customer {
private:
    int id;
    string name;
    float balance;
    
public:
    Customer() {}
    
    Customer(int id, string name, float balance) {
        this->id = id;
        this->name = name;
        this->balance = balance;
    }
    
    int getId() {
        return this->id;
    }
    string getName() {
        return this->name;
    }
    float getBalance() {
        return this->balance;
    }
    
    friend ostream& operator<<(ostream& os, const Customer& customer);
};

ostream& operator<<(ostream& os, const Customer& customer){
    os << customer.id << ' ' << customer.name << ' ' << customer.balance;
    return os;
}

#endif
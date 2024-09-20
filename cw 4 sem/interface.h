#ifndef V_INTERFACE_H
#define V_INTERFACE_H

using namespace std;
#include <vector>

template <typename T, typename V>
class ICollection {

public:
    virtual void Add(T key, V* value) = 0;
    virtual bool Get(T key, V&) = 0; 
    virtual void Get(T minbound, T maxbound, vector<V*>& result) = 0;
    virtual bool Update(T key, V* value) = 0; 
    virtual bool Remove(T key) = 0;
};

#endif //V_INTERFACE_H

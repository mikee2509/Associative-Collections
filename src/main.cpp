#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>

#include "TreeMap.h"
namespace
{
    using std::cout;
    using std::endl;
    template <typename K, typename V>
    using Map = aisdi::TreeMap<K, V>;
//
//    void perfomTest()
//    {
//        Map<int, std::string> map;
//        map[1] = "TODO";
//    }

} // namespace

int main()
{
    cout << "Hello World!" << endl;
//    perfomTest();
    return 0;
}

#include <cstddef>
#include <cstdlib>
#include <string>
#include <iostream>
#include <ctime>
#include <vector>
#include <chrono>

#include "TreeMap.h"
#include "HashMap.h"
namespace
{
    using std::cout;
    using std::endl;
    using std::vector;
    using std::pair;
    template <typename K, typename V>
    using TreeMap = aisdi::TreeMap<K, V>;

    template <typename K, typename V>
    using HashMap = aisdi::HashMap<K, V>;

} // namespace

int main(int argc, char* argv[])
{
    srand(time(0));
    long long NUM;
    if(argc > 1) NUM = std::atoll(argv[1]);
    else
    {
        cout << "Usage: aisdiMaps <number_of_elements>" << endl;
        return 1;
    }
    vector <pair<long long, long long>> testSet;
    for(long long i = 0; i < NUM; ++i)
        testSet.push_back(std::make_pair((rand() % NUM), (rand() % NUM)));

    cout << "Testing TreeMap" << endl;
    auto start_time = std::chrono::high_resolution_clock::now();

    TreeMap<long long, long long> tree;

    for(long long i = 0; i < NUM; ++i)
        tree[testSet[i].first] = testSet[i].second;
    cout << "...finished adding" << endl;

    for(auto it = tree.begin(); it!=tree.end(); ++it)
        if(it->first % 10000 == 0) cout << it->first << " ";
    cout << endl << "...finished iteration" << endl;

    for(std::pair <long long, long long> val : testSet)
        tree.find(val.first);
    cout << "...finished finding" << endl << endl;

    auto current_time = std::chrono::high_resolution_clock::now();
    cout << "Test run for: " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count()
        << " milliseconds" << endl;

    cout << endl << endl << "Testing HashMap" << endl;
    start_time = std::chrono::high_resolution_clock::now();
    HashMap<long long, long long> hash;


    for(long long i = 0; i < NUM; ++i)
        hash[testSet[i].first] = testSet[i].second;
    cout << "...finished adding" << endl;

    for(auto it = hash.begin(); it!=hash.end(); ++it)
        if(it->first % 10000 == 0) cout << it->first << " ";
    cout << endl << "...finished iteration" << endl;

    for(std::pair <long long, long long> val : testSet)
        hash.find(val.first);
    cout << "...finished finding" << endl << endl;

    current_time = std::chrono::high_resolution_clock::now();
    cout << "Test run for: " << std::chrono::duration_cast<std::chrono::milliseconds>(current_time - start_time).count()
        << " milliseconds" << endl;

    return 0;
}

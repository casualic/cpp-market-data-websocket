#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

// The task we want to accomplish: load a vector, populate it to size 1000, and then reference it.
// printing and pushing and  pointers 

std::unique_ptr<std::vector<int>> loadDataWithPointer(){
    auto dataPtr = std::make_unique<std::vector<int>>();

    dataPtr->reserve(1000);
    for (int i = 0; i <= 1000; i++) {
        dataPtr->push_back(i);
    }

    return dataPtr;

}

// int main() {

//     auto x {loadDataWithPointer()};
//     std::cout << x->front() << '\n';
//     std::cout << x->back() << '\n';
//     std::cout << x->at(2) << '\n';



//     return 0;
// }

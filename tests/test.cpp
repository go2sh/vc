#include <boost/filesystem.hpp>
#include <iostream>

int main(int argc, char **argv) {
    boost::filesystem::path path("C:\\asd/../asddf/asd.hpp");

    for (auto element : path) {
        std::cout << "asd: " << element << std::endl;
    }
}
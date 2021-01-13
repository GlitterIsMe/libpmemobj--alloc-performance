#include <unistd.h>
#include <iostream>
#include <string>
#include <chrono>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_array.hpp>
#include <libpmemobj++/make_persistent_atomic.hpp>
#include <libpmemobj++/make_persistent_array_atomic.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#define ALLOCATION_UNIT 64
#define RUN_TIMES 100

const std::string PMEM = "/pmem0/test/alloc_test";
const std::string LAYOUT = "test";
using namespace pmem::obj;

struct Root {
    persistent_ptr<char> p;
};

static inline int file_exists(char const *file) {
    return access(file, F_OK);
}

int main(int argc, char** argv) {
    pool<Root> p;
    if (!file_exists(PMEM.c_str())) {
        p = pool<Root>::open(PMEM, LAYOUT);
    } else {
        p = pool<Root>::create(PMEM, LAYOUT, 10UL * 1024 * 1024* 1024);
    }
    if (argc != 2) {
        std::cout << "Mismatching parameter number " << argc << std::endl;
        exit(-1);
    }
    int allocation_unit = atoi(argv[1]);
    std::cout << "Allocation unit: " << allocation_unit << "bytes\n";

    {
        // Transaction Allocation
        auto start = std::chrono::high_resolution_clock::now();
        persistent_ptr<char[]> blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
            transaction::run(p, [&] {
                blocks[i] = make_persistent<char[]>(allocation_unit);
            });
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> f = end - start;
        std::cout << "[Transaction] alloc ops: " << RUN_TIMES / f.count() << std::endl;
    }

    {
        // Atomic Allocation
        auto start = std::chrono::high_resolution_clock::now();
        persistent_ptr<char[]> blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
           make_persistent_atomic<char[]>(p, blocks[i], allocation_unit);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> f = end - start;
        std::cout << "[Atomic] alloc ops: " << RUN_TIMES / f.count() << std::endl;
    }

    {
        // Memory Allocation
        auto start = std::chrono::high_resolution_clock::now();
        char* blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
            blocks[i] = new char[allocation_unit];
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<float> f = end - start;
        std::cout << "[Memory] alloc ops: " << RUN_TIMES / f.count() << std::endl;
    }
    return 0;
}

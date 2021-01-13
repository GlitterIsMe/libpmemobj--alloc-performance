#include <unistd.h>
#include <iostream>
#include <string>
#include <chrono>
#include <libpmemobj++/make_persistent.hpp>
#include <libpmemobj++/make_persistent_atomic.hpp>
#include <libpmemobj++/pool.hpp>
#include <libpmemobj++/transaction.hpp>

#define ALLOCATION_UNIT 64
#define RUN_TIMES 100

const std::string PMEM = "mnt/pmem0/test";
const std::string LAYOUT = "test";
using namespace pmem::obj;

struct Block {
    char block[ALLOCATION_UNIT];
};

struct Root {
    persistent_ptr<Block> p;
};

static inline int file_exists(char const *file) {
    return access(file, F_OK);
}

int main() {
    pool<Root> p;
    if (file_exists(PMEM.c_str())) {
        p = pool<Root>::open(PMEM, LAYOUT);
    } else {
        p = pool<Root>::create(PMEM, LAYOUT, 1UL * 1024 * 1024* 1024);
    }

    {
        // Transaction Allocation
        auto start = std::chrono::high_resolution_clock::now();
        persistent_ptr<Block> blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
            transaction::run(p, [&] {
                blocks[i] = make_persistent<Block>();
            });
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "[Transaction] alloc ops: " << RUN_TIMES / std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
    }

    {
        // Atomic Allocation
        auto start = std::chrono::high_resolution_clock::now();
        persistent_ptr<Block> blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
           make_persistent_atomic<Block>(p, blocks[i]);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "[Atomic] alloc ops: " << RUN_TIMES / std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
    }

    {
        // Memory Allocation
        auto start = std::chrono::high_resolution_clock::now();
        Block* blocks[RUN_TIMES];
        for (int i = 0; i < RUN_TIMES; i++) {
            blocks[i] = new Block();
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "[Memory] alloc ops: " << RUN_TIMES / std::chrono::duration_cast<std::chrono::seconds>(end - start).count() << std::endl;
    }
    return 0;
}

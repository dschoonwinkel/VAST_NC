#include <iostream>
#include "../ConcurrentQueue.h"
#include <assert.h>

int main() {

    ConcurrentQueue<int> q;
    std::thread producer([&]() {
        for (int i = 0; i != 100; ++i) {
            std::cout << "Enqueuing " << i << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(i % 10));
            q.push(i);
        }
    });
    std::thread consumer([&]() {
        for (int i = 0; i != 100; ++i) {
            int item;
            while (q.size() == 0){}
            q.pop(item);
            std::cout << "Dequeueing " << item << std::endl;
            assert(item == i);

        }
    });
    producer.join();
    consumer.join();

    assert(q.size() == 0);

	return 0;
}

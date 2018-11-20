#include <iostream>
#include <iomanip>
#include <thread>
#include <mutex>
#include <chrono>
#include <stdexcept>
#include <cstdlib>
#include <ctime>

using std::chrono::system_clock;

std::mutex mtx;

void print_even(int x) {

    std::time_t tt = system_clock::to_time_t(system_clock::now());

    struct std::tm* ptm = std::localtime(&tt);
    std::cout << "Current time: " << std::put_time(ptm, "%X") << '\n';

    std::cout << "Waiting for the next minute to begin...\n";
    ++ptm->tm_min; ptm->tm_sec = 0;
    std::this_thread::sleep_until(system_clock::from_time_t(mktime(ptm)));

    std::cout << std::put_time(ptm, "%X") << "reached!\n";

    std::this_thread::sleep_for(std::chrono::seconds(3));
    if (x % 2 == 0) std::cout << x << " is even\n";
    else throw (std::logic_error("not even"));
}

void print_thread_id(int id) {
    try {
        std::lock_guard<std::mutex> lck(mtx);
        print_even(id);
    }
    catch (std::logic_error&) {
        std::cout << "[exception caught\n";
    }
}

int main(int argc, char* argv[]) {
    std::thread threads[10];
    for (int i = 0; i < 10; ++i) {
        threads[i] = std::thread(print_thread_id, i + 1);
    }

    for (auto& th : threads) th.join();

    return EXIT_SUCCESS;
}
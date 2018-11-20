#include <boost/thread/thread.hpp>
#include <boost/lockfree/stack.hpp>
#include <iostream>

#include <boost/atomic.hpp>

#include "..\Project1\MLogger.h"

boost::atomic_int producer_count(0);
boost::atomic_int consumer_count(0);

boost::lockfree::stack<int> stack(128);

const int iterations = 10000000;
const int producer_thread_count = 1;
const int consumer_thread_count = 1;

void producer(void)
{
    for (int i = 0; i != iterations; ++i) {
        int value = ++producer_count;
        while (!stack.push(value))
            ;
    }
}

boost::atomic<bool> isAllThreadStarted(false);

void consumer(void)
{
    int value;
    while (!isAllThreadStarted) {
        while (stack.pop(value))
            ++consumer_count;
    }

    while (stack.pop(value))
        ++consumer_count;
}

int main(int argc, char* argv[])
{
    using namespace std;
    char ts[24];
    cout << nowTime(ts) << " : boost::lockfree::stack is ";
    if (!stack.is_lock_free())
        cout << "not ";
    cout << "lockfree" << endl;

    boost::thread_group producer_threads, consumer_threads;

    for (int i = 0; i != producer_thread_count; ++i)
        producer_threads.create_thread(producer);

    for (int i = 0; i != consumer_thread_count; ++i)
        consumer_threads.create_thread(consumer);

    producer_threads.join_all();
    isAllThreadStarted
        = true;

    consumer_threads.join_all();

    cout << nowTime(ts) << " : produced " << producer_count << " objects." << endl;
    cout << nowTime(ts) << " : consumed " << consumer_count << " objects." << endl;
}
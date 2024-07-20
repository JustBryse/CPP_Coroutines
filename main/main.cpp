// Description: This project exists to experiment with coroutines

#include "task.h"
#include <iostream>
#include <unistd.h>

using std::cout;
using namespace CoroutinePractice;

suspend_always MySleep(int seconds){
    cout << "MySleep(): Start" << "\n";
    for(int i = 0; i < seconds; i++)
    {
        sleep(1);
        cout << "MySleep(): " << i+1 << "\n";
    }
    cout << "MySleep(): End" << "\n";
    return {};
}

Task ProcessSleeping(){
    while(true) {
        cout << "Receive(): Before MySleep()" << "\n";
        // co_await returns control back up to main()
        co_await MySleep(3);
        cout << "Receive(): After MySleep()" << "\n";
        co_return;
    }
}

int main(){

    // note that Task's promise never suspends initially
    Task c1 = ProcessSleeping();
    // this resume will happen after co_await in ProcessSleeping() returns control back to main()
    c1.Resume();
    cout << "Main(): After c1 created" << "\n";

    return 0;
}
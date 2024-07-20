// Description: This file is meant to demonstrate a very basic coroutine task schedule

#include "task.h"
#include <iostream>
#include <unistd.h>

const int REQUESTED_SUPPLY = 10;
const int PRODUCTION_RATE = 1;
const int CONSUMPTION_RATE = 2;
const int PRODUCTION_DELAY_US = 2000; // microseconds
const int CONSUMPTION_DELAY_US = 1000; // microseconds

using namespace CoroutinePractice;
using std::cout;

int supply;
int consumed;

Task Producer(){
    while(true)
    {
        cout << "Producer() -> Producing..." << "\n"; 
        usleep(PRODUCTION_DELAY_US);
        supply += PRODUCTION_RATE;
        cout << "Producer() -> Supply (+) = " << supply << "\n";
        co_yield 0; // yield back up to calling function  
    }
}

Task Consumer(){
    while(true)
    {
        cout << "Consumer() -> Attempting Consuming..." << "\n";
        usleep(CONSUMPTION_DELAY_US);

        if(supply >= CONSUMPTION_RATE){
            consumed += CONSUMPTION_RATE;
            supply -= CONSUMPTION_RATE;
            cout << "Consumer() -> Supply (-) = " << supply << "\n";
        }

        co_yield 0; 
    }
}

int main(){
    supply = 0;
    consumed = 0;

    Task p = Producer();
    Task c = Consumer();

    while(true){

        if(consumed >= REQUESTED_SUPPLY){
            p.Destroy();
            c.Destroy();
            return 0;
        }

        p.Resume();
        c.Resume();
    }

    return 0;
}

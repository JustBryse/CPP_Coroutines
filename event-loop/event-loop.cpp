#include "task.h"
#include <iostream>
#include <unistd.h>
#include <deque>
#include <time.h>
#include <string>

using namespace CoroutinePractice;
using std::cout;
using std::deque;

// execute commands 1 second before their expiry time
const int LEAD_TIME = 1;
// the window length is 5 seconds of commands
const int WINDOW_LENGTH = 5;
// holds the number of commands that will be generated into the backlog
const int BACKLOG_SIZE = 10;
// holds the number of seconds into the future that command expiry time's will be offset by
const int COMMAND_EXPIRY_OFFSET = 10;

struct Command {
    string message;
    time_t expiry;
};

// stores the list of commands that will be gradually added to the window. This is populated with data on initialization
deque<Command> backlog;
// represents a moving window of commands that will expire. Is gradually loaded from the backlog
deque<Command> window;

// add new commands
Task Add()
{
    while(true)
    {   
        time_t now = 0;
        time(&now);

        // add() is done when the backlog is empty
        if(backlog.empty()){
            cout << now << " - (^) Add() -> Yielding" << "\n";
            co_yield 1; // status code 1 indicates that the coroutine has nothing to do right now
            time(&now); // update the time when resumed by the event loop
        }

        Command c = backlog.back();

        time_t delta = c.expiry - now; // get how much time remains until the command should be executed

        if(delta <= WINDOW_LENGTH){
            window.push_front(c);
            backlog.pop_back();
            cout << now << " - (+) Add() -> Load: " << c.expiry << " - " << c.message << "\n";
        }
        else{
            cout << now << " - (-) Add() -> Pass: " << c.expiry << " - " << c.message << ". ETA = " << delta - WINDOW_LENGTH << "\n";
        }

        co_yield 0; // status code 0 indicates that the coroutine has done something (one thing) and is now relinquishing control
    }
}

// execute commands if it is the right time
Task Execute()
{
    while(true)
    {   
        time_t now = 0;
        time(&now);

        // if there are no commands right now in the window, then yield back up to the event loop and let it decide what to do
        if(window.empty()){
            cout << now << " - (^) Execute() -> Yielding" << "\n";
            co_yield 1; // status code 1 indicates that the coroutine has nothing to do right now
            time(&now); // update the time when resumed by the event loop
        }

        Command c = window.back();

        time_t delta = c.expiry - now;
        //cout << "Execute() -> Delta: " << delta << "\n";

        if(delta <= LEAD_TIME){
            cout << now << " - (+) Execute() -> Run: " << c.expiry << " - " << c.message << "\n";
            window.pop_back();
            //cout << "Execute() -> window size: " << window.size() << "\n";
        }
        else{
            cout << now << " - (-) Execute() -> Pass: " << c.expiry << " - " << c.message << ". ETA = " << delta << "\n";
        }

        co_yield 0; // status code 0 indicates that the coroutine has done something (one thing) and is now relinquishing control
    }
}

void FillBacklog()
{
    time_t now = 0;
    time(&now);

    cout << now << " - Filling Backlog..." << "\n\n";

    for(int i = 0; i < BACKLOG_SIZE; ++i)
    {
        Command c;
        c.expiry = now + (COMMAND_EXPIRY_OFFSET+i);
        c.message = "command " + std::to_string(i+1);
        backlog.push_front(c);
    }
}

void DisplayBacklog()
{
    time_t now = 0;
    time(&now);

    cout << now << " - Showing Backlog..." << "\n\n";

    for(int i = 0; i < backlog.size(); ++i){
        cout << backlog[i].expiry << " - " << backlog[i].message << "\n";
    }
    cout << "\n";
}

// The event loop contains the logic for deciding when to resume the coroutines in the event loop
int EventLoop(Task* add, Task* execute)
{
    // start in state 01.
    int addYieldValue = 0;
    int execYieldValue = 1;
    while(true)
    {   
        // this happens when add() is able to load window from the backlog but execute() doesn't yet have anything in window to pop
        // backlog is not empty and window is empty
        if(addYieldValue == 0 && execYieldValue == 1){
            usleep(100000);
            add->Resume();
            addYieldValue = add->GetYieldValue();

            /* After add() is resumed, it may pass or load a command into window (depends on the next command's expiry time).
             * Check if add() actuall loaded a command into window before resuming execute(). */
            if(window.empty() == false){
                usleep(100000);
                execute->Resume();
                execYieldValue = execute->GetYieldValue();
            }
        }
        // this happens when add() has emptied the backlog and execute() is still able to pop from window
        // backlog is empty and window is not empty
        else if(addYieldValue == 1 && execYieldValue == 0){
            usleep(100000);
            execute->Resume();
            execYieldValue = execute->GetYieldValue();
        }
        // this happens when add() is able to load window and execute() is able to pop window
        // backlog is not empty and window is not empty
        else if(addYieldValue == 0 && execYieldValue == 0){
            usleep(100000);
            add->Resume();
            addYieldValue = add->GetYieldValue();

            usleep(100000);
            execute->Resume();
            execYieldValue = execute->GetYieldValue();
        }
        // this can happen if add() is waiting to load window and execute() is waiting for window to be loaded, such as during initialization
        // backlog is empty and window is empty OR backlog is not empty and window is empty (these two scenarios can occur at program end and start respectively)
        else if(addYieldValue == 1 && execYieldValue == 1){
            // if the backlog is empty and both add() and execute() have yielded as being done, then the program is complete
            if(backlog.empty()){
                return 0;
            }
        }
    }

    return 1;
}

int main() {

    backlog = deque<Command>();
    window = deque<Command>();

    // initialize backlog
    FillBacklog();
    DisplayBacklog();

    // note that these coroutines should be configured to suspend always on initialization
    Task a = Add();
    Task e = Execute();

    int result = EventLoop(&a,&e);
    cout << "main() -> EventLoop() result = " << result << "\n";

    return 0;
}
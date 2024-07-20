#include "task.h"

#include <unistd.h>

using CoroutinePractice::Task;

suspend_always Task::promise_type::initial_suspend(){
    return {};
}

Task Task::promise_type::get_return_object(){
    //std::cout << "Task::promise_type::get_return_object()" << "\n";
    return Task(coroutine_handle<promise_type>::from_promise(*this));
}

suspend_never Task::promise_type::final_suspend() noexcept {
    return {};
}

suspend_always Task::promise_type::yield_value(int arg){
    yieldValue = arg;
    return {};
}

void Task::promise_type::return_void(){
    return;
}

void Task::promise_type::unhandled_exception(){
    exit(1);
}

Task::Task(coroutine_handle<promise_type> ch){
    coroutine = ch;
}

void Task::Resume(){
    coroutine.resume();
}

void Task::Destroy(){
    coroutine.destroy();
}

int Task::GetYieldValue(){
    return coroutine.promise().yieldValue;
}

#ifndef TASK_H
#define TASK_H

#include <coroutine>
#include <iostream>
#include <string>

using std::string;
using std::coroutine_handle;
// using awaitable types
using std::suspend_always;
using std::suspend_never;

namespace CoroutinePractice
{
    class Task{

        public:

            struct promise_type {
                friend Task;
                // call at the start of co_await
                suspend_always initial_suspend();
                // returns a handle to this tasks
                Task get_return_object();
                // called at the end of co_await
                suspend_never final_suspend() noexcept;
                // called on co_yield. Argument is passed via co_yield
                suspend_always yield_value(int arg);
                // does nothing
                void return_void();
                // called if an exception occurs
                void unhandled_exception();

                int yieldValue = 0;
            };

        protected:
            coroutine_handle<promise_type> coroutine;

        public:
            Task(coroutine_handle<promise_type> ch);
            void Resume();
            void Destroy();
            int GetYieldValue();
    };
} // namespace CoroutinePractice

#endif
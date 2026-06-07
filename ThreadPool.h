#pragma once

#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <atomic>
#include <condition_variable>
#include <future>
#include <functional>
#include <stop_token>

class ThreadPool {
public:

	explicit ThreadPool(size_t numThreads = std::thread::hardware_concurrency());
	
	ThreadPool(const ThreadPool&) = delete;
	ThreadPool& operator=(const ThreadPool&) = delete;

	~ThreadPool();

	template<typename F, typename... Args>
	auto enqueue(F&& f, Args&& ... args) -> std::future<decltype(f(args...))>;

	void shutdown();
	size_t pendingTasks() const;
	bool isStopped() const;

private:

	void worker(std::stop_token st);

	std::vector<std::jthread> workers;
	std::queue<std::function<void()>> tasks;
	mutable std::mutex queueMutex;
	std::condition_variable cv;
	std::stop_source stopSource;
	std::atomic<bool> stopFlag = false;

};
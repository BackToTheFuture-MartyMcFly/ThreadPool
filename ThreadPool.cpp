//ThreadPool.cpp
#include "ThreadPool.h"

ThreadPool::ThreadPool(size_t numThreads) {
	for (auto i = 0; i < numThreads; ++i) {
		workers.emplace_back([this](std::stop_token st) { worker(st); });
	}
}

ThreadPool::~ThreadPool() {
	shutdown();
	for (auto it = workers.begin(); it != workers.end(); ++it)
		it->join();
}

void ThreadPool::shutdown() {
	stopFlag = true;
	stopSource.request_stop();
	cv.notify_all();
}

size_t ThreadPool::pendingTasks() const {
	std::lock_guard<std::mutex> lock(queueMutex);
	return tasks.size();
}

bool ThreadPool::isStopped() const {
	return stopFlag.load();
}

template<typename F, typename... Args>
auto ThreadPool::enqueue(F&& f, Args&& ... args) -> std::future<decltype(f(args...))> {
	using ReturnType = decltype(f(args...));

	if (isStopped())
		throw std::runtime_error("Thread pool is stopped");

	auto task = std::make_shared<std::packaged_task<ReturnType>>(std::bind(std::forward<F>(f), std::forward<Args>(args)...));
	std::future<ReturnType> future = task->get_future();

	{
		std::lock_guard<std::mutex> lock(queueMutex);
		tasks.emplace([task]() { (*task)(); });
	}
	cv.notify_one();

	return future;
}

void ThreadPool::worker(std::stop_token st) {
	while (!st.stop_requested()) {
		std::unique_lock lock(queueMutex);
		cv.wait(lock, [this, &st] {
			return !tasks.empty() || st.stop_requested();
			});

		if (st.stop_requested() && tasks.empty())
			break;

		auto task = std::move(tasks.front());
		tasks.pop();
		lock.unlock();

		task();
	}
}
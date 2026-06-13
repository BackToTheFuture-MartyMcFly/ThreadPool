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

		if (--activeTasks == 0)
			allDoneCv.notify_one();
		
	}
}

void ThreadPool::waitForAll() {
	std::unique_lock lock(queueMutex);
	allDoneCv.wait(lock, [this] {
		return activeTasks == 0 && tasks.empty();
		});
}
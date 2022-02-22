#pragma once
#include <atomic>
#include <mutex>
#include <thread>
#include <queue>

class SimpleThreadTask
{
public:
	virtual void run() = 0;
};

class SimpleThreadPool
{
public:
	SimpleThreadPool() : max_workers(std::max(1U, std::thread::hardware_concurrency())) {}

	using Task = std::unique_ptr<SimpleThreadTask>;
	using TaskQueue = std::queue<Task>;

	static SimpleThreadPool& get()
	{
		static SimpleThreadPool instance;
		return instance;
	}

	Task get_task()
	{
		std::scoped_lock lock(mutex);
		if (exiting || task_queue.empty()) return nullptr;

		Task task;
		task.swap(task_queue.front());
		task_queue.pop();
		return task;
	}

	void add_task(Task task)
	{
		if (exiting) return;

		std::scoped_lock lock(mutex);
		task_queue.emplace(std::move(task));

		if (workers.size() < max_workers)
		{
			add_worker();
		}
	}

	void add_worker()
	{
		auto t = std::thread([]()
			{
				uint64_t last_tick = GetTickCount64();

				while (!SimpleThreadPool::get().exiting && GetTickCount64() - last_tick < 10000)
				{
					auto task = SimpleThreadPool::get().get_task();
					if (task)
					{
						task->run();
						last_tick = GetTickCount64();
					}
					else
					{
						std::this_thread::sleep_for(std::chrono::milliseconds(1000));
					}
				}
				SimpleThreadPool::get().remove_worker();
			});

		workers.insert(t.get_id());
		t.detach();
	}

	void exit()
	{
		exiting = true;
		std::scoped_lock lock(mutex);
		task_queue = TaskQueue();
	}

	void remove_worker()
	{
		std::scoped_lock lock(mutex);
		workers.erase(std::this_thread::get_id());
	}

	std::atomic_bool exiting = false;

private:
	TaskQueue task_queue;
	std::mutex mutex;
	std::set<std::thread::id> workers;
	uint32_t max_workers = 0;

	PFC_CLASS_NOT_COPYABLE_EX(SimpleThreadPool)
};

namespace
{
	class InitQuitTP : public initquit
	{
	public:
		void on_quit() override
		{
			SimpleThreadPool::get().exit();
		}
	};

	FB2K_SERVICE_FACTORY(InitQuitTP);
}

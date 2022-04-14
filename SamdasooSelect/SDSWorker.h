#pragma once

#include "ISDSWorkUnit.h"

class SDSWorker
{
private:
	std::atomic_bool loop = false;
	std::thread workerThread;
	std::map<SOCKET, std::shared_ptr<ISDSWorkUnit>> units;
	::fd_set fds;
	std::recursive_mutex lock;

	static void Run(SDSWorker* $);

public:
	SDSWorker() = default;
	~SDSWorker() = default;
	SDSWorker(const SDSWorker&) = delete;
	SDSWorker(SDSWorker&&) = delete;
	SDSWorker& operator=(const SDSWorker&) = delete;
	SDSWorker& operator=(SDSWorker&&) = delete;

	void Init();
	void BeginThread();
	void JoinThread();
	void EndThread();

	std::shared_ptr<ISDSWorkUnit> GetUnit(SOCKET key);
	bool InsertUnit(std::shared_ptr<ISDSWorkUnit> unit);
	void RemoveUnit(std::shared_ptr<ISDSWorkUnit> unit);
};

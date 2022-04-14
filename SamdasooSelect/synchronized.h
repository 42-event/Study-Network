#pragma once

#define synchronized(_x) for (auto _ul = std::unique_lock<std::recursive_mutex>(_x); _ul; _ul.unlock())

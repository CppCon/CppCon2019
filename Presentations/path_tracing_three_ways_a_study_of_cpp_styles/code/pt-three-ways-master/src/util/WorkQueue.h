#pragma once

#include "Progressifier.h"

#include <mutex>
#include <optional>
#include <vector>

template <typename WorkItem>
class WorkQueue {
  std::mutex mutex_;
  std::vector<WorkItem> todo_;
  Progressifier progress_;

public:
  explicit WorkQueue(std::vector<WorkItem> todo) noexcept
      : todo_(std::move(todo)), progress_(todo_.size()) {}

  template <typename InLock>
  std::optional<WorkItem> pop(InLock &&inLock) noexcept {
    std::unique_lock lock(mutex_);
    inLock();
    progress_.numLeft(todo_.size());
    if (todo_.empty())
      return {};
    auto tile = todo_.back();
    todo_.pop_back();
    return tile;
  }
};
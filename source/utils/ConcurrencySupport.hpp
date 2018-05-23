/*
 * Copyright 2018 Frangou Lab
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LIBGENE_UTILS_CONCURRENCYSUPPORT_HPP
#define LIBGENE_UTILS_CONCURRENCYSUPPORT_HPP

#include <mutex>
#include <condition_variable>

namespace gene {

class ThreadBarrier {
 public:
    constexpr ThreadBarrier() noexcept = default;
    constexpr explicit ThreadBarrier(int thread_count) noexcept
    : threshold_(thread_count)
    , count_(thread_count)
    {
    }

    void SetThreshold(int thread_count) noexcept
    {
        threshold_ = thread_count;
        count_ = thread_count;
    }

    void Wait()
    {
        std::unique_lock<std::mutex> lock(m_);
        int gen = generation_;
        if (!--count_) {
            generation_++;
            count_ = threshold_;
            cond_.notify_all();
        } else
            cond_.wait(lock, [this, gen] { return gen != generation_; });
    }

 private:
    std::mutex m_;
    std::condition_variable cond_;
    int threshold_;
    int count_;
    int generation_{0};
};

}  // namespace gene

#endif  // LIBGENE_UTILS_CONCURRENCYSUPPORT_HPP

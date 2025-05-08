#include "MemoryManager.hpp"
#include "Allocator.hpp"
#include "alloca/Arena.hpp"
#include "alloca/Region.hpp"
#include "utils/defines.h"

#include <Runtime.hpp>
#include <array>
#include <oneapi/tbb/concurrent_queue.h>
#include <system/System.hpp>
#include <unordered_map>


namespace MemoryManager {
  std::recursive_mutex mutex_;

  auto partion_of_pages() -> std::vector<size_t> {
    auto counts      = std::vector<size_t>(48);
    auto goal_counts = std::vector<size_t>(48, 2);

    int count_pages = max_heap_size >> 13;
    // int count_regions = sqrt(max_heap_size >> 1) - 1;
    //
    // int page_for_reg = count_pages / count_regions;
    //
    // for (auto i = 0; i < count_regions; ++i) {
    //   auto count_page_in_arena = ((1 << i) + 1);
    //   counts[i]                = page_for_reg / count_page_in_arena;
    //   count_pages -= counts[i] * count_page_in_arena;
    // }
    //
    // assert(count_pages >= 0);

    for (int i = 0; count_pages > 1 << i; ++i) {
      ++counts[i];
      count_pages -= (1 << i) + 1;
      if (counts[i] == goal_counts[i]) {
        ++ ++goal_counts[i];
      } else {
        i = -1;
      }
    }
    for (auto& el : counts)
      el <<= 1;
    count_pages >>= 1;
    for (int i = 0; count_pages > 0; ++i) {
      if (count_pages < (1 << (i + 1)) + 1) {
        ++counts[i];
        count_pages -= (1 << i) + 1;
        i = 0;
      }
    }
    return counts;
  }

  size_t COUNT_OF_TIERS = 48;

  ref heap_start = 0;

  size_t max_heap_size = 256ULL * 1024 * 1024 * 1024;

  std::atomic_size_t memory = 0;

  auto regions() -> std::vector<Region<Arena>*>& {
    guard(mutex_);
    static auto regs = [] {
      std::vector<Region<Arena>*> result;

      auto&& counts = partion_of_pages();

      size_t a_size = 2_page;
      size_t i      = 0;

      auto cur = heap_start;
      for (const size_t c : counts) {
        if (!c)
          break;
        result.push_back(new Region<Arena>{cur, c, a_size, i++});
        cur += c * a_size;
        a_size = (a_size << 1) - 1_page;
      }

      return result;
    }();

    return regs;
  }

  auto ref_in_heap(ref ptr) -> bool {
    ptr -= 1_ref;
    if (heap_start <= ptr && ptr < heap_start + max_heap_size) {
      auto abp = arena_by_ptr(ptr);
      return abp->start != abp->cur;
    }
    return false;
  }

  auto tier_by_size(size_t size) -> size_t {
    for (size_t tier = 0;; tier++)
      if (((1 << tier) + 1) * 1_page > size)
        return tier;
  }


  auto arena_by_ptr(ref ptr) -> Arena* {
    int    index;
    auto&& regs = regions();
    for (index = 0; index < regs.size() && ptr >= regs[index]->start; ++index)
      ;
    --index;
    auto offset  = ptr - regs[index]->start;
    auto a_index = offset / regs[index]->t_size;


    return (*regs[index])[a_index];
  }

  auto free_arena(Arena* a) -> void {
    guard(mutex_);
    a->cur = a->start;
    a->kill();
    sys::free(a);
  }
}; // namespace MemoryManager

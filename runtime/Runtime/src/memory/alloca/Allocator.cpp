#include "Allocator.hpp"

Allocator::Allocator(const size_t start_heap, const size_t size_heap)
    : start(start_heap), size(size_heap) {
  /*
   * variant of regions "double-double"
   * each egion is +- equal in size
   */
  int count;
  for (count = 1; count * (1 << count) < size << 12; ++count)
    ;

  std::vector<size_t> counts;

  for (int i = count; i; counts.push_back(1 << i--))
    ;

  size_t lost_pages = size << 12 - count * (1 << count);

  for (int i = 0; lost_pages; ++i) {
    if (lost_pages & 1 << i) {
      lost_pages -= 1 << i;
      counts[i]++;
    }
  }

  size_t cur    = start_heap;
  size_t a_size = 1 << 12;
  for (const size_t c : counts) {
    regions.push_back(Region<Arena>(cur, c, a_size));

    cur += c * a_size;
    a_size <<= 1;
  }
}
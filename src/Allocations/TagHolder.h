// Copyright (c) 2019 VMware, Inc. All Rights Reserved.
// SPDX-License-Identifier: GPL-2.0

#pragma once
#include "Finder.h"

namespace chap {
namespace Allocations {
template <typename Offset>
class TagHolder {
 public:
  typedef typename Finder<Offset>::AllocationIndex AllocationIndex;
  typedef size_t TagIndex;
  TagHolder(const AllocationIndex numAllocations)
      : _numAllocations(numAllocations), notTagged(0) {
    _tags.reserve(numAllocations);
    _tags.resize(numAllocations, 0);
    _indexToName.push_back("");
  }

  TagIndex RegisterTag(const char *name) {
    TagIndex newIndex = _indexToName.size();
    if (_indexToName.size() == 0x255) {
      std::cerr
          << "255 tags reached - change the implementation of TagHolder\n";
      abort();
    }
    _indexToName.push_back(name);
    return newIndex;
  }

  bool TagAllocation(AllocationIndex allocationIndex, TagIndex tagIndex) {
    if (tagIndex >= _indexToName.size()) {
      std::cerr << "Invalid allocation tag index " << tagIndex << "\n";
      abort();
    }
    if (allocationIndex >= _numAllocations) {
      std::cerr << "Invalid allocation index " << allocationIndex << "\n";
      abort();
    }
    if (_tags[allocationIndex] == 0) {
      _tags[allocationIndex] = tagIndex;
      return true;
    }
    return false;
  }

  TagIndex GetTagIndex(AllocationIndex allocationIndex) const {
    if (allocationIndex >= _numAllocations) {
      std::cerr << "Invalid allocation index " << allocationIndex << "\n";
      abort();
    }
    return _tags[allocationIndex];
  }

 private:
  const AllocationIndex _numAllocations;
  const int notTagged;
  std::vector<unsigned char> _tags;
  std::vector<const char *> _indexToName;
};
}  // namespace Allocations
}  // namespace chap

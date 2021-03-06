// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdlib.h>

#include <utility>

#include <minfs/allocator-promise.h>

#include "allocator.h"

namespace minfs {

AllocatorPromise::~AllocatorPromise() { Cancel(); }

zx_status_t AllocatorPromise::Initialize(PendingWork* transaction, size_t reserved,
                                         Allocator* allocator) {
  if (allocator_ != nullptr) {
    return ZX_ERR_BAD_STATE;
  }

  if (allocator == nullptr) {
    return ZX_ERR_INVALID_ARGS;
  }

  ZX_DEBUG_ASSERT(reserved_ == 0);

  zx_status_t status = allocator->Reserve({}, transaction, reserved, this);
  if (status == ZX_OK) {
    allocator_ = allocator;
    reserved_ = reserved;
  }
  return status;
}

size_t AllocatorPromise::Allocate(PendingWork* transaction) {
  ZX_DEBUG_ASSERT(allocator_ != nullptr);
  ZX_DEBUG_ASSERT(reserved_ > 0);
  reserved_--;
  return allocator_->Allocate({}, transaction);
}

#ifdef __Fuchsia__
size_t AllocatorPromise::Swap(size_t old_index) {
  ZX_DEBUG_ASSERT(allocator_ != nullptr);
  ZX_DEBUG_ASSERT(reserved_ > 0);
  reserved_--;
  return allocator_->Swap({}, old_index);
}

void AllocatorPromise::SwapCommit(PendingWork* transaction) {
  ZX_DEBUG_ASSERT(allocator_ != nullptr);
  allocator_->SwapCommit({}, transaction);
}

void AllocatorPromise::GiveBlocks(size_t requested, AllocatorPromise* other_promise) {
  ZX_DEBUG_ASSERT(requested <= reserved_);
  ZX_DEBUG_ASSERT(other_promise != nullptr);

  if (other_promise->IsInitialized()) {
    ZX_DEBUG_ASSERT(other_promise->allocator_ == allocator_);
  } else {
    other_promise->allocator_ = allocator_;
  }

  reserved_ -= requested;
  other_promise->reserved_ += requested;
}

#endif

void AllocatorPromise::Cancel() {
  if (IsInitialized() && reserved_ > 0) {
    allocator_->Unreserve({}, reserved_);
    reserved_ = 0;
  }

  ZX_DEBUG_ASSERT(reserved_ == 0);
}

}  // namespace minfs

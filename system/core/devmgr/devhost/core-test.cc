// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <fuchsia/device/manager/llcpp/fidl.h>
#include <lib/async-loop/cpp/loop.h>
#include <lib/async-loop/default.h>
#include <lib/fidl-async/cpp/bind.h>

#include <ddk/driver.h>
#include <zxtest/zxtest.h>

#include "zx-device.h"

namespace {

class FakeCoordinator : public ::llcpp::fuchsia::device::manager::Coordinator::Interface {
 public:
  zx_status_t Connect(async_dispatcher_t* dispatcher, zx::channel request) {
    return fidl::Bind(dispatcher, std::move(request), this);
  }

  void AddDevice(::zx::channel rpc, ::fidl::VectorView<uint64_t> props, ::fidl::StringView name,
                 uint32_t protocol_id, ::fidl::StringView driver_path, ::fidl::StringView args,
                 llcpp::fuchsia::device::manager::AddDeviceConfig device_add_config,
                 ::zx::channel client_remote, AddDeviceCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_AddDevice_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void UnbindDone(UnbindDoneCompleter::Sync completer) override {}
  void RemoveDone(RemoveDoneCompleter::Sync completer) override {}
  void ScheduleRemove(bool unbind_self, ScheduleRemoveCompleter::Sync completer) override {}
  void AddCompositeDevice(
      ::fidl::StringView name, ::fidl::VectorView<uint64_t> props,
      ::fidl::VectorView<llcpp::fuchsia::device::manager::DeviceComponent> components,
      uint32_t coresident_device_index, AddCompositeDeviceCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_AddCompositeDevice_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void PublishMetadata(::fidl::StringView device_path, uint32_t key,
                       ::fidl::VectorView<uint8_t> data,
                       PublishMetadataCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_PublishMetadata_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void AddDeviceInvisible(::zx::channel rpc, ::fidl::VectorView<uint64_t> props,
                          ::fidl::StringView name, uint32_t protocol_id,
                          ::fidl::StringView driver_path, ::fidl::StringView args,
                          ::zx::channel client_remote,
                          AddDeviceInvisibleCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_AddDeviceInvisible_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void RemoveDevice(RemoveDeviceCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_RemoveDevice_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void MakeVisible(MakeVisibleCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_MakeVisible_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void BindDevice(::fidl::StringView driver_path, BindDeviceCompleter::Sync completer) override {
    bind_count_++;
    llcpp::fuchsia::device::manager::Coordinator_BindDevice_Result response;
    response.set_err(ZX_OK);
    completer.Reply(std::move(response));
  }
  void GetTopologicalPath(GetTopologicalPathCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_GetTopologicalPath_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void LoadFirmware(::fidl::StringView fw_path, LoadFirmwareCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_LoadFirmware_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void GetMetadata(uint32_t key, GetMetadataCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_GetMetadata_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void GetMetadataSize(uint32_t key, GetMetadataSizeCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_GetMetadataSize_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void AddMetadata(uint32_t key, ::fidl::VectorView<uint8_t> data,
                   AddMetadataCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_AddMetadata_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void ScheduleUnbindChildren(ScheduleUnbindChildrenCompleter::Sync completer) override {}
  void RunCompatibilityTests(int64_t hook_wait_time,
                             RunCompatibilityTestsCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_RunCompatibilityTests_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }
  void DirectoryWatch(uint32_t mask, uint32_t options, ::zx::channel watcher,
                      DirectoryWatchCompleter::Sync completer) override {
    llcpp::fuchsia::device::manager::Coordinator_DirectoryWatch_Result response;
    response.set_err(ZX_ERR_NOT_SUPPORTED);
    completer.Reply(std::move(response));
  }

  uint32_t bind_count_ = 0;
};

class CoreTest : public zxtest::Test {
 protected:
  CoreTest() : loop_(&kAsyncLoopConfigNoAttachToCurrentThread) {
    loop_.StartThread("devhost-test-loop");
  }

  void Connect(zx::channel* out) {
    zx::channel local, remote;
    ASSERT_OK(zx::channel::create(0, &local, &remote));
    ASSERT_OK(coordinator_.Connect(loop_.dispatcher(), std::move(remote)));
    *out = std::move(local);
  }

  async::Loop loop_;
  FakeCoordinator coordinator_;
};

TEST_F(CoreTest, RebindNoChildren) {
  fbl::RefPtr<zx_device> dev;
  ASSERT_OK(zx_device::Create(&dev));

  zx_protocol_device_t ops = {};
  dev->ops = &ops;

  zx::channel rpc;
  ASSERT_NO_FATAL_FAILURES(Connect(&rpc));
  dev->rpc = zx::unowned(rpc);

  EXPECT_EQ(device_rebind(dev.get()), ZX_OK);
  EXPECT_EQ(coordinator_.bind_count_, 1);

  dev->flags |= DEV_FLAG_VERY_DEAD;
}

TEST_F(CoreTest, RebindHasOneChild) {
  {
    uint32_t unbind_count = 0;
    fbl::RefPtr<zx_device> parent;

    zx::channel rpc;
    ASSERT_NO_FATAL_FAILURES(Connect(&rpc));

    zx_protocol_device_t ops = {};
    ops.unbind = [](void* ctx) { *static_cast<uint32_t*>(ctx) += 1; };

    ASSERT_OK(zx_device::Create(&parent));
    parent->ops = &ops;
    parent->ctx = &unbind_count;
    parent->rpc = zx::unowned(rpc);
    {
      fbl::RefPtr<zx_device> child;
      ASSERT_OK(zx_device::Create(&child));
      child->ops = &ops;
      child->ctx = &unbind_count;
      child->rpc = zx::unowned(rpc);
      parent->children.push_back(child.get());
      child->parent = parent;

      EXPECT_EQ(device_rebind(parent.get()), ZX_OK);
      EXPECT_EQ(unbind_count, 1);

      child->flags |= DEV_FLAG_VERY_DEAD;
    }
    parent->flags |= DEV_FLAG_VERY_DEAD;
  }
  EXPECT_EQ(coordinator_.bind_count_, 1);
}

TEST_F(CoreTest, RebindHasMultipleChildren) {
  {
    uint32_t unbind_count = 0;
    fbl::RefPtr<zx_device> parent;

    zx::channel rpc;
    ASSERT_NO_FATAL_FAILURES(Connect(&rpc));

    zx_protocol_device_t ops = {};
    ops.unbind = [](void* ctx) { *static_cast<uint32_t*>(ctx) += 1; };

    ASSERT_OK(zx_device::Create(&parent));
    parent->ops = &ops;
    parent->ctx = &unbind_count;
    parent->rpc = zx::unowned(rpc);
    {
      std::array<fbl::RefPtr<zx_device>, 5> children;
      for (auto& child : children) {
        ASSERT_OK(zx_device::Create(&child));
        child->ops = &ops;
        child->ctx = &unbind_count;
        child->rpc = zx::unowned(rpc);
        parent->children.push_back(child.get());
        child->parent = parent;
      }

      EXPECT_EQ(device_rebind(parent.get()), ZX_OK);
      EXPECT_EQ(unbind_count, children.size());

      for (auto& child : children) {
        child->flags |= DEV_FLAG_VERY_DEAD;
      }
    }
    parent->flags |= DEV_FLAG_VERY_DEAD;
  }
  EXPECT_EQ(coordinator_.bind_count_, 1);
}

}  // namespace

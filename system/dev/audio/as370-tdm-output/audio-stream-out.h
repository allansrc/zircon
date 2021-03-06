// Copyright 2019 The Fuchsia Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef ZIRCON_SYSTEM_DEV_AUDIO_AS370_TDM_OUTPUT_AUDIO_STREAM_OUT_H_
#define ZIRCON_SYSTEM_DEV_AUDIO_AS370_TDM_OUTPUT_AUDIO_STREAM_OUT_H_

#include <lib/device-protocol/i2c-channel.h>
#include <lib/device-protocol/pdev.h>
#include <lib/fzl/pinned-vmo.h>
#include <lib/simple-audio-stream/simple-audio-stream.h>
#include <lib/sync/completion.h>
#include <lib/zircon-internal/thread_annotations.h>

#include <memory>
#include <optional>

#include <audio-proto/audio-proto.h>
#include <ddk/io-buffer.h>
#include <ddk/protocol/platform/device.h>
#include <ddktl/device-internal.h>
#include <ddktl/device.h>
#include <ddktl/protocol/clock.h>
#include <ddktl/protocol/codec.h>
#include <ddktl/protocol/platform/device.h>
#include <dispatcher-pool/dispatcher-timer.h>
#include <fbl/mutex.h>
#include <soc/as370/syn-audio-out.h>

#include "codec.h"

namespace audio {
namespace as370 {

class As370AudioStreamOut : public SimpleAudioStream {
 protected:
  zx_status_t Init() TA_REQ(domain_->token()) override;
  zx_status_t ChangeFormat(const audio_proto::StreamSetFmtReq& req)
      TA_REQ(domain_->token()) override;
  zx_status_t GetBuffer(const audio_proto::RingBufGetBufferReq& req, uint32_t* out_num_rb_frames,
                        zx::vmo* out_buffer) TA_REQ(domain_->token()) override;
  zx_status_t Start(uint64_t* out_start_time) TA_REQ(domain_->token()) override;
  zx_status_t Stop() TA_REQ(domain_->token()) override;
  zx_status_t SetGain(const audio_proto::SetGainReq& req) TA_REQ(domain_->token()) override;
  void ShutdownHook() TA_REQ(domain_->token()) override;
  zx_status_t InitPost() TA_REQ(domain_->token()) override;

 private:
  enum {
    kAvpll0Clk,
    kAvpll1Clk,
    kClockCount,
  };

  friend class SimpleAudioStream;
  friend class fbl::RefPtr<As370AudioStreamOut>;

  As370AudioStreamOut(zx_device_t* parent);
  ~As370AudioStreamOut() {}

  zx_status_t AddFormats() TA_REQ(domain_->token());
  zx_status_t InitBuffer(size_t size) TA_REQ(domain_->token());
  zx_status_t InitPdev() TA_REQ(domain_->token());
  zx_status_t ProcessRingNotification();

  uint32_t us_per_notification_ = 0;
  fbl::RefPtr<dispatcher::Timer> notify_timer_;
  ddk::PDev pdev_ TA_GUARDED(domain_->token());
  zx::vmo ring_buffer_vmo_ TA_GUARDED(domain_->token());
  std::unique_ptr<SynAudioOutDevice> lib_;
  ddk::ClockProtocolClient clks_[kClockCount] TA_GUARDED(domain_->token());
  Codec codec_ TA_GUARDED(domain_->token());
};

}  // namespace as370
}  // namespace audio

#endif  // ZIRCON_SYSTEM_DEV_AUDIO_AS370_TDM_OUTPUT_AUDIO_STREAM_OUT_H_

// WARNING: This file is machine generated by fidlgen.

#include <fidl_llcpp_controlflow.h>
#include <memory>

namespace llcpp {

namespace fidl {
namespace test {
namespace llcpp {
namespace controlflow {

namespace {

[[maybe_unused]]
constexpr uint64_t kControlFlow_Shutdown_Ordinal = 0x7d0a313700000000lu;
[[maybe_unused]]
constexpr uint64_t kControlFlow_NoReplyMustSendAccessDeniedEpitaph_Ordinal = 0x2141b1d000000000lu;
[[maybe_unused]]
constexpr uint64_t kControlFlow_MustSendAccessDeniedEpitaph_Ordinal = 0x531dcb0700000000lu;
extern "C" const fidl_type_t fidl_test_llcpp_controlflow_ControlFlowMustSendAccessDeniedEpitaphResponseTable;

}  // namespace

ControlFlow::ResultOf::Shutdown_Impl::Shutdown_Impl(zx::unowned_channel _client_end) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<ShutdownRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, ShutdownRequest::PrimarySize);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(ShutdownRequest));
  ::fidl::DecodedMessage<ShutdownRequest> _decoded_request(std::move(_request_bytes));
  Super::operator=(
      ControlFlow::InPlace::Shutdown(std::move(_client_end)));
}

ControlFlow::ResultOf::Shutdown ControlFlow::SyncClient::Shutdown() {
  return ResultOf::Shutdown(zx::unowned_channel(this->channel_));
}

ControlFlow::ResultOf::Shutdown ControlFlow::Call::Shutdown(zx::unowned_channel _client_end) {
  return ResultOf::Shutdown(std::move(_client_end));
}

::fidl::internal::StatusAndError ControlFlow::InPlace::Shutdown(zx::unowned_channel _client_end) {
  constexpr uint32_t _write_num_bytes = sizeof(ShutdownRequest);
  ::fidl::internal::AlignedBuffer<_write_num_bytes> _write_bytes;
  ::fidl::BytePart _request_buffer = _write_bytes.view();
  _request_buffer.set_actual(_write_num_bytes);
  ::fidl::DecodedMessage<ShutdownRequest> params(std::move(_request_buffer));
  params.message()->_hdr = {};
  params.message()->_hdr.ordinal = kControlFlow_Shutdown_Ordinal;
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::internal::StatusAndError::FromFailure(
        std::move(_encode_request_result));
  }
  zx_status_t _write_status =
      ::fidl::Write(std::move(_client_end), std::move(_encode_request_result.message));
  if (_write_status != ZX_OK) {
    return ::fidl::internal::StatusAndError(_write_status, ::fidl::internal::kErrorWriteFailed);
  } else {
    return ::fidl::internal::StatusAndError(ZX_OK, nullptr);
  }
}


ControlFlow::ResultOf::NoReplyMustSendAccessDeniedEpitaph_Impl::NoReplyMustSendAccessDeniedEpitaph_Impl(zx::unowned_channel _client_end) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<NoReplyMustSendAccessDeniedEpitaphRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, NoReplyMustSendAccessDeniedEpitaphRequest::PrimarySize);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(NoReplyMustSendAccessDeniedEpitaphRequest));
  ::fidl::DecodedMessage<NoReplyMustSendAccessDeniedEpitaphRequest> _decoded_request(std::move(_request_bytes));
  Super::operator=(
      ControlFlow::InPlace::NoReplyMustSendAccessDeniedEpitaph(std::move(_client_end)));
}

ControlFlow::ResultOf::NoReplyMustSendAccessDeniedEpitaph ControlFlow::SyncClient::NoReplyMustSendAccessDeniedEpitaph() {
  return ResultOf::NoReplyMustSendAccessDeniedEpitaph(zx::unowned_channel(this->channel_));
}

ControlFlow::ResultOf::NoReplyMustSendAccessDeniedEpitaph ControlFlow::Call::NoReplyMustSendAccessDeniedEpitaph(zx::unowned_channel _client_end) {
  return ResultOf::NoReplyMustSendAccessDeniedEpitaph(std::move(_client_end));
}

::fidl::internal::StatusAndError ControlFlow::InPlace::NoReplyMustSendAccessDeniedEpitaph(zx::unowned_channel _client_end) {
  constexpr uint32_t _write_num_bytes = sizeof(NoReplyMustSendAccessDeniedEpitaphRequest);
  ::fidl::internal::AlignedBuffer<_write_num_bytes> _write_bytes;
  ::fidl::BytePart _request_buffer = _write_bytes.view();
  _request_buffer.set_actual(_write_num_bytes);
  ::fidl::DecodedMessage<NoReplyMustSendAccessDeniedEpitaphRequest> params(std::move(_request_buffer));
  params.message()->_hdr = {};
  params.message()->_hdr.ordinal = kControlFlow_NoReplyMustSendAccessDeniedEpitaph_Ordinal;
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::internal::StatusAndError::FromFailure(
        std::move(_encode_request_result));
  }
  zx_status_t _write_status =
      ::fidl::Write(std::move(_client_end), std::move(_encode_request_result.message));
  if (_write_status != ZX_OK) {
    return ::fidl::internal::StatusAndError(_write_status, ::fidl::internal::kErrorWriteFailed);
  } else {
    return ::fidl::internal::StatusAndError(ZX_OK, nullptr);
  }
}

template <>
ControlFlow::ResultOf::MustSendAccessDeniedEpitaph_Impl<ControlFlow::MustSendAccessDeniedEpitaphResponse>::MustSendAccessDeniedEpitaph_Impl(zx::unowned_channel _client_end) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<MustSendAccessDeniedEpitaphRequest, ::fidl::MessageDirection::kSending>();
  ::fidl::internal::AlignedBuffer<_kWriteAllocSize> _write_bytes_inlined;
  auto& _write_bytes_array = _write_bytes_inlined;
  uint8_t* _write_bytes = _write_bytes_array.view().data();
  memset(_write_bytes, 0, MustSendAccessDeniedEpitaphRequest::PrimarySize);
  ::fidl::BytePart _request_bytes(_write_bytes, _kWriteAllocSize, sizeof(MustSendAccessDeniedEpitaphRequest));
  ::fidl::DecodedMessage<MustSendAccessDeniedEpitaphRequest> _decoded_request(std::move(_request_bytes));
  Super::SetResult(
      ControlFlow::InPlace::MustSendAccessDeniedEpitaph(std::move(_client_end), Super::response_buffer()));
}

ControlFlow::ResultOf::MustSendAccessDeniedEpitaph ControlFlow::SyncClient::MustSendAccessDeniedEpitaph() {
  return ResultOf::MustSendAccessDeniedEpitaph(zx::unowned_channel(this->channel_));
}

ControlFlow::ResultOf::MustSendAccessDeniedEpitaph ControlFlow::Call::MustSendAccessDeniedEpitaph(zx::unowned_channel _client_end) {
  return ResultOf::MustSendAccessDeniedEpitaph(std::move(_client_end));
}

template <>
ControlFlow::UnownedResultOf::MustSendAccessDeniedEpitaph_Impl<ControlFlow::MustSendAccessDeniedEpitaphResponse>::MustSendAccessDeniedEpitaph_Impl(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer) {
  FIDL_ALIGNDECL uint8_t _write_bytes[sizeof(MustSendAccessDeniedEpitaphRequest)] = {};
  ::fidl::BytePart _request_buffer(_write_bytes, sizeof(_write_bytes));
  memset(_request_buffer.data(), 0, MustSendAccessDeniedEpitaphRequest::PrimarySize);
  _request_buffer.set_actual(sizeof(MustSendAccessDeniedEpitaphRequest));
  ::fidl::DecodedMessage<MustSendAccessDeniedEpitaphRequest> _decoded_request(std::move(_request_buffer));
  Super::SetResult(
      ControlFlow::InPlace::MustSendAccessDeniedEpitaph(std::move(_client_end), std::move(_response_buffer)));
}

ControlFlow::UnownedResultOf::MustSendAccessDeniedEpitaph ControlFlow::SyncClient::MustSendAccessDeniedEpitaph(::fidl::BytePart _response_buffer) {
  return UnownedResultOf::MustSendAccessDeniedEpitaph(zx::unowned_channel(this->channel_), std::move(_response_buffer));
}

ControlFlow::UnownedResultOf::MustSendAccessDeniedEpitaph ControlFlow::Call::MustSendAccessDeniedEpitaph(zx::unowned_channel _client_end, ::fidl::BytePart _response_buffer) {
  return UnownedResultOf::MustSendAccessDeniedEpitaph(std::move(_client_end), std::move(_response_buffer));
}

::fidl::DecodeResult<ControlFlow::MustSendAccessDeniedEpitaphResponse> ControlFlow::InPlace::MustSendAccessDeniedEpitaph(zx::unowned_channel _client_end, ::fidl::BytePart response_buffer) {
  constexpr uint32_t _write_num_bytes = sizeof(MustSendAccessDeniedEpitaphRequest);
  ::fidl::internal::AlignedBuffer<_write_num_bytes> _write_bytes;
  ::fidl::BytePart _request_buffer = _write_bytes.view();
  _request_buffer.set_actual(_write_num_bytes);
  ::fidl::DecodedMessage<MustSendAccessDeniedEpitaphRequest> params(std::move(_request_buffer));
  params.message()->_hdr = {};
  params.message()->_hdr.ordinal = kControlFlow_MustSendAccessDeniedEpitaph_Ordinal;
  auto _encode_request_result = ::fidl::Encode(std::move(params));
  if (_encode_request_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ControlFlow::MustSendAccessDeniedEpitaphResponse>::FromFailure(
        std::move(_encode_request_result));
  }
  auto _call_result = ::fidl::Call<MustSendAccessDeniedEpitaphRequest, MustSendAccessDeniedEpitaphResponse>(
    std::move(_client_end), std::move(_encode_request_result.message), std::move(response_buffer));
  if (_call_result.status != ZX_OK) {
    return ::fidl::DecodeResult<ControlFlow::MustSendAccessDeniedEpitaphResponse>::FromFailure(
        std::move(_call_result));
  }
  return ::fidl::Decode(std::move(_call_result.message));
}


bool ControlFlow::TryDispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  if (msg->num_bytes < sizeof(fidl_message_header_t)) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_INVALID_ARGS);
    return true;
  }
  fidl_message_header_t* hdr = reinterpret_cast<fidl_message_header_t*>(msg->bytes);
  switch (hdr->ordinal) {
    case kControlFlow_Shutdown_Ordinal:
    {
      auto result = ::fidl::DecodeAs<ShutdownRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      impl->Shutdown(
        Interface::ShutdownCompleter::Sync(txn));
      return true;
    }
    case kControlFlow_NoReplyMustSendAccessDeniedEpitaph_Ordinal:
    {
      auto result = ::fidl::DecodeAs<NoReplyMustSendAccessDeniedEpitaphRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      impl->NoReplyMustSendAccessDeniedEpitaph(
        Interface::NoReplyMustSendAccessDeniedEpitaphCompleter::Sync(txn));
      return true;
    }
    case kControlFlow_MustSendAccessDeniedEpitaph_Ordinal:
    {
      auto result = ::fidl::DecodeAs<MustSendAccessDeniedEpitaphRequest>(msg);
      if (result.status != ZX_OK) {
        txn->Close(ZX_ERR_INVALID_ARGS);
        return true;
      }
      impl->MustSendAccessDeniedEpitaph(
        Interface::MustSendAccessDeniedEpitaphCompleter::Sync(txn));
      return true;
    }
    default: {
      return false;
    }
  }
}

bool ControlFlow::Dispatch(Interface* impl, fidl_msg_t* msg, ::fidl::Transaction* txn) {
  bool found = TryDispatch(impl, msg, txn);
  if (!found) {
    zx_handle_close_many(msg->handles, msg->num_handles);
    txn->Close(ZX_ERR_NOT_SUPPORTED);
  }
  return found;
}


void ControlFlow::Interface::MustSendAccessDeniedEpitaphCompleterBase::Reply(int32_t reply) {
  constexpr uint32_t _kWriteAllocSize = ::fidl::internal::ClampedMessageSize<MustSendAccessDeniedEpitaphResponse, ::fidl::MessageDirection::kSending>();
  FIDL_ALIGNDECL uint8_t _write_bytes[_kWriteAllocSize] = {};
  auto& _response = *reinterpret_cast<MustSendAccessDeniedEpitaphResponse*>(_write_bytes);
  _response._hdr.ordinal = kControlFlow_MustSendAccessDeniedEpitaph_Ordinal;
  _response.reply = std::move(reply);
  ::fidl::BytePart _response_bytes(_write_bytes, _kWriteAllocSize, sizeof(MustSendAccessDeniedEpitaphResponse));
  CompleterBase::SendReply(::fidl::DecodedMessage<MustSendAccessDeniedEpitaphResponse>(std::move(_response_bytes)));
}

void ControlFlow::Interface::MustSendAccessDeniedEpitaphCompleterBase::Reply(::fidl::BytePart _buffer, int32_t reply) {
  if (_buffer.capacity() < MustSendAccessDeniedEpitaphResponse::PrimarySize) {
    CompleterBase::Close(ZX_ERR_INTERNAL);
    return;
  }
  auto& _response = *reinterpret_cast<MustSendAccessDeniedEpitaphResponse*>(_buffer.data());
  _response._hdr.ordinal = kControlFlow_MustSendAccessDeniedEpitaph_Ordinal;
  _response.reply = std::move(reply);
  _buffer.set_actual(sizeof(MustSendAccessDeniedEpitaphResponse));
  CompleterBase::SendReply(::fidl::DecodedMessage<MustSendAccessDeniedEpitaphResponse>(std::move(_buffer)));
}

void ControlFlow::Interface::MustSendAccessDeniedEpitaphCompleterBase::Reply(::fidl::DecodedMessage<MustSendAccessDeniedEpitaphResponse> params) {
  params.message()->_hdr = {};
  params.message()->_hdr.ordinal = kControlFlow_MustSendAccessDeniedEpitaph_Ordinal;
  CompleterBase::SendReply(std::move(params));
}


}  // namespace controlflow
}  // namespace llcpp
}  // namespace test
}  // namespace fidl
}  // namespace llcpp
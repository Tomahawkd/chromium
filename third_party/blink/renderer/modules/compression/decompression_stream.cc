// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "third_party/blink/renderer/modules/compression/decompression_stream.h"

#include "base/metrics/histogram_macros.h"
#include "third_party/blink/renderer/modules/compression/compression_format.h"
#include "third_party/blink/renderer/modules/compression/inflate_transformer.h"
#include "third_party/blink/renderer/platform/bindings/exception_state.h"

namespace blink {

DecompressionStream* DecompressionStream::Create(
    ScriptState* script_state,
    const AtomicString& format,
    ExceptionState& exception_state) {
  return MakeGarbageCollected<DecompressionStream>(script_state, format,
                                                   exception_state);
}

ReadableStream* DecompressionStream::readable() const {
  return transform_->Readable();
}

WritableStream* DecompressionStream::writable() const {
  return transform_->Writable();
}

void DecompressionStream::Trace(Visitor* visitor) {
  visitor->Trace(transform_);
  ScriptWrappable::Trace(visitor);
}

DecompressionStream::DecompressionStream(ScriptState* script_state,
                                         const AtomicString& format,
                                         ExceptionState& exception_state)
    : transform_(MakeGarbageCollected<TransformStream>()) {
  CompressionFormat inflate_format =
      LookupCompressionFormat(format, exception_state);
  if (exception_state.HadException())
    return;

  UMA_HISTOGRAM_ENUMERATION("Blink.Compression.DecompressionStream.Format",
                            inflate_format);

  transform_->Init(
      MakeGarbageCollected<InflateTransformer>(script_state, inflate_format),
      script_state, exception_state);
}

}  // namespace blink

// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROMECAST_MEDIA_AUDIO_MIXER_SERVICE_MIXER_CONTROL_H_
#define CHROMECAST_MEDIA_AUDIO_MIXER_SERVICE_MIXER_CONTROL_H_

#include <string>

#include "base/macros.h"
#include "base/threading/sequence_bound.h"

namespace chromecast {
namespace media {
namespace mixer_service {
class ControlConnection;

// Threadsafe process-wide mixer control.
class MixerControl {
 public:
  // Returns the mixer control instance for this process, or nullptr if the
  // mixer is not present on this system.
  static MixerControl* Get();

  // Sends arbitrary config data to a specific postprocessor.
  void ConfigurePostprocessor(std::string postprocessor_name,
                              std::string config);

  // Instructs the mixer to reload postprocessors based on the config file.
  void ReloadPostprocessors();

  // Sets the desired number of output channels used by the mixer. This will
  // cause an audio interruption on any currently active streams. The actual
  // output channel count is determined by the output implementation and may not
  // match |num_channels|.
  void SetNumOutputChannels(int num_channels);

 private:
  friend class base::NoDestructor<MixerControl>;
  MixerControl();
  ~MixerControl();

  base::SequenceBound<ControlConnection> control_;

  DISALLOW_COPY_AND_ASSIGN(MixerControl);
};

}  // namespace mixer_service
}  // namespace media
}  // namespace chromecast

#endif  // CHROMECAST_MEDIA_AUDIO_MIXER_SERVICE_MIXER_CONTROL_H_

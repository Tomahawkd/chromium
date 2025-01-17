// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_WEBUI_FAVICON_SOURCE_H_
#define CHROME_BROWSER_UI_WEBUI_FAVICON_SOURCE_H_

#include <map>
#include <string>

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/task/cancelable_task_tracker.h"
#include "components/favicon/core/favicon_service.h"
#include "content/public/browser/url_data_source.h"
#include "ui/gfx/favicon_size.h"

class Profile;

namespace base {
class RefCountedMemory;
}

namespace chrome {
enum class FaviconUrlFormat;
}

namespace ui {
class NativeTheme;
}

// FaviconSource is the gateway between network-level chrome:
// requests for favicons and the history backend that serves these.
// Two possible formats are allowed: chrome://favicon, kept only for backwards
// compatibility for extensions, and chrome://favicon2. Formats are described in
// favicon_url_parser.h.
class FaviconSource : public content::URLDataSource {
 public:
  // |type| is the type of icon this FaviconSource will provide.
  explicit FaviconSource(Profile* profile, chrome::FaviconUrlFormat format);

  ~FaviconSource() override;

  // content::URLDataSource implementation.
  std::string GetSource() override;
  void StartDataRequest(
      const GURL& url,
      const content::WebContents::Getter& wc_getter,
      const content::URLDataSource::GotDataCallback& callback) override;
  std::string GetMimeType(const std::string&) override;
  bool AllowCaching() override;
  bool ShouldReplaceExistingSource() override;
  bool ShouldServiceRequest(const GURL& url,
                            content::ResourceContext* resource_context,
                            int render_process_id) override;

 protected:
  // Exposed for testing.
  virtual ui::NativeTheme* GetNativeTheme();
  virtual base::RefCountedMemory* LoadIconBytes(float scale_factor,
                                                int resource_id);

  Profile* profile_;

 private:
  // Defines the allowed pixel sizes for requested favicons.
  enum IconSize {
    SIZE_16,
    SIZE_32,
    SIZE_64,
    NUM_SIZES
  };

  // Called when favicon data is available from the history backend. If
  // |bitmap_result| is valid, returns it to caller using |callback|. Otherwise
  // will send appropriate default icon for |size_in_dip| and |scale_factor|.
  void OnFaviconDataAvailable(
      const content::URLDataSource::GotDataCallback& callback,
      int size_in_dip,
      float scale_factor,
      const favicon_base::FaviconRawBitmapResult& bitmap_result);

  // Sends the 16x16 DIP 1x default favicon.
  void SendDefaultResponse(
      const content::URLDataSource::GotDataCallback& callback);

  // Sends the default favicon.
  void SendDefaultResponse(
      const content::URLDataSource::GotDataCallback& callback,
      int size_in_dip,
      float scale_factor);

  chrome::FaviconUrlFormat url_format_;

  base::CancelableTaskTracker cancelable_task_tracker_;

  DISALLOW_COPY_AND_ASSIGN(FaviconSource);
};

#endif  // CHROME_BROWSER_UI_WEBUI_FAVICON_SOURCE_H_

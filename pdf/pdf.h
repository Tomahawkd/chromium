// Copyright (c) 2010 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef PDF_PDF_H_
#define PDF_PDF_H_

#include <vector>

#include "base/containers/span.h"
#include "build/build_config.h"

#if defined(OS_WIN)
#include <windows.h>
#endif

#if defined(OS_WIN)
typedef void (*PDFEnsureTypefaceCharactersAccessible)(const LOGFONT* font,
                                                      const wchar_t* text,
                                                      size_t text_length);
#endif

namespace gfx {
class Rect;
class Size;
}

namespace chrome_pdf {

#if defined(OS_CHROMEOS)
// Create a flattened PDF document from an existing PDF document.
// |input_buffer| is the buffer that contains the entire PDF document to be
// flattened.
std::vector<uint8_t> CreateFlattenedPdf(base::span<const uint8_t> input_buffer);
#endif  // defined(OS_CHROMEOS)

#if defined(OS_WIN)
// Printing modes - type to convert PDF to for printing
enum PrintingMode {
  kEmf = 0,
  kTextOnly = 1,
  kPostScript2 = 2,
  kPostScript3 = 3,
};

// |pdf_buffer| is the buffer that contains the entire PDF document to be
//     rendered.
// |page_number| is the 0-based index of the page to be rendered.
// |dc| is the device context to render into.
// |dpi_x| and |dpi_y| is the resolution.
// |bounds_origin_x|, |bounds_origin_y|, |bounds_width| and |bounds_height|
//     specify a bounds rectangle within the DC in which to render the PDF
//     page.
// |fit_to_bounds| specifies whether the output should be shrunk to fit the
//     supplied bounds if the page size is larger than the bounds in any
//     dimension. If this is false, parts of the PDF page that lie outside
//     the bounds will be clipped.
// |stretch_to_bounds| specifies whether the output should be stretched to fit
//     the supplied bounds if the page size is smaller than the bounds in any
//     dimension.
// If both |fit_to_bounds| and |stretch_to_bounds| are true, then
//     |fit_to_bounds| is honored first.
// |keep_aspect_ratio| If any scaling is to be done is true, this flag
//     specifies whether the original aspect ratio of the page should be
//     preserved while scaling.
// |center_in_bounds| specifies whether the final image (after any scaling is
//     done) should be centered within the given bounds.
// |autorotate| specifies whether the final image should be rotated to match
//     the output bound.
// |use_color| specifies color or grayscale.
// Returns false if the document or the page number are not valid.
bool RenderPDFPageToDC(base::span<const uint8_t> pdf_buffer,
                       int page_number,
                       HDC dc,
                       int dpi_x,
                       int dpi_y,
                       int bounds_origin_x,
                       int bounds_origin_y,
                       int bounds_width,
                       int bounds_height,
                       bool fit_to_bounds,
                       bool stretch_to_bounds,
                       bool keep_aspect_ratio,
                       bool center_in_bounds,
                       bool autorotate,
                       bool use_color);

void SetPDFEnsureTypefaceCharactersAccessible(
    PDFEnsureTypefaceCharactersAccessible func);

void SetPDFUseGDIPrinting(bool enable);

void SetPDFUsePrintMode(int mode);
#endif  // defined(OS_WIN)

// |page_count| and |max_page_width| are optional and can be NULL.
// Returns false if the document is not valid.
bool GetPDFDocInfo(base::span<const uint8_t> pdf_buffer,
                   int* page_count,
                   double* max_page_width);

// Gets the dimensions of a specific page in a document.
// |pdf_buffer| is the buffer that contains the entire PDF document to be
//     rendered.
// |page_number| is the page number that the function will get the dimensions
//     of.
// |width| is the output for the width of the page in points.
// |height| is the output for the height of the page in points.
// Returns false if the document or the page number are not valid.
bool GetPDFPageSizeByIndex(base::span<const uint8_t> pdf_buffer,
                           int page_number,
                           double* width,
                           double* height);

// Renders PDF page into 4-byte per pixel BGRA color bitmap.
// |pdf_buffer| is the buffer that contains the entire PDF document to be
//     rendered.
// |page_number| is the 0-based index of the page to be rendered.
// |bitmap_buffer| is the output buffer for bitmap.
// |bitmap_width| is the width of the output bitmap.
// |bitmap_height| is the height of the output bitmap.
// |dpi_x| and |dpi_y| is the resolution.
// |autorotate| specifies whether the final image should be rotated to match
//     the output bound.
// |use_color| specifies color or grayscale.
// Returns false if the document or the page number are not valid.
bool RenderPDFPageToBitmap(base::span<const uint8_t> pdf_buffer,
                           int page_number,
                           void* bitmap_buffer,
                           int bitmap_width,
                           int bitmap_height,
                           int dpi_x,
                           int dpi_y,
                           bool autorotate,
                           bool use_color);

// Convert multiple PDF pages into a N-up PDF.
// |input_buffers| is the vector of buffers with each buffer contains a PDF.
//     If any of the PDFs contains multiple pages, only the first page of the
//     document is used.
// |pages_per_sheet| is the number of pages to put on one sheet.
// |page_size| is the output page size, measured in PDF "user space" units.
// |printable_area| is the output page printable area, measured in PDF
//     "user space" units.  Should be smaller than |page_size|.
//
// |page_size| is the print media size.  The page size of the output N-up PDF is
// determined by the |pages_per_sheet|, the orientation of the PDF pages
// contained in the |input_buffers|, and the media page size |page_size|. For
// example, when |page_size| = 512x792, |pages_per_sheet| = 2, and the
// orientation of |input_buffers| = portrait, the output N-up PDF will be
// 792x512.
// See printing::NupParameters for more details on how the output page
// orientation is determined, to understand why |page_size| may be swapped in
// some cases.
std::vector<uint8_t> ConvertPdfPagesToNupPdf(
    std::vector<base::span<const uint8_t>> input_buffers,
    size_t pages_per_sheet,
    const gfx::Size& page_size,
    const gfx::Rect& printable_area);

// Convert a PDF document to a N-up PDF document.
// |input_buffer| is the buffer that contains the entire PDF document to be
//     converted to a N-up PDF document.
// |pages_per_sheet| is the number of pages to put on one sheet.
// |page_size| is the output page size, measured in PDF "user space" units.
// |printable_area| is the output page printable area, measured in PDF
//     "user space" units.  Should be smaller than |page_size|.
//
// Refer to the description of ConvertPdfPagesToNupPdf to understand how the
// output page size will be calculated.
// The algorithm used to determine the output page size is the same.
std::vector<uint8_t> ConvertPdfDocumentToNupPdf(
    base::span<const uint8_t> input_buffer,
    size_t pages_per_sheet,
    const gfx::Size& page_size,
    const gfx::Rect& printable_area);

}  // namespace chrome_pdf

#endif  // PDF_PDF_H_

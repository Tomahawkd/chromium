/*
 * Copyright (C) 2004, 2005 Nikolas Zimmermann <zimmermann@kde.org>
 * Copyright (C) 2004, 2005, 2006 Rob Buis <buis@kde.org>
 * Copyright (C) 2009 Google, Inc.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301, USA.
 */

#include "third_party/blink/renderer/core/layout/svg/layout_svg_transformable_container.h"

#include "third_party/blink/renderer/core/layout/svg/transform_helper.h"
#include "third_party/blink/renderer/core/svg/svg_g_element.h"
#include "third_party/blink/renderer/core/svg/svg_graphics_element.h"
#include "third_party/blink/renderer/core/svg/svg_use_element.h"

namespace blink {

LayoutSVGTransformableContainer::LayoutSVGTransformableContainer(
    SVGGraphicsElement* node)
    : LayoutSVGContainer(node), needs_transform_update_(true) {}

static bool HasValidPredecessor(const Node* node) {
  DCHECK(node);
  for (node = node->previousSibling(); node; node = node->previousSibling()) {
    auto* svg_element = DynamicTo<SVGElement>(node);
    if (svg_element && svg_element->IsValid())
      return true;
  }
  return false;
}

bool LayoutSVGTransformableContainer::IsChildAllowed(
    LayoutObject* child,
    const ComputedStyle& style) const {
  DCHECK(GetElement());
  Node* child_node = child->GetNode();
  if (IsSVGSwitchElement(*GetElement())) {
    // Reject non-SVG/non-valid elements.
    auto* svg_element = DynamicTo<SVGElement>(child_node);
    if (!svg_element || !svg_element->IsValid()) {
      return false;
    }
    // Reject this child if it isn't the first valid node.
    if (HasValidPredecessor(child_node))
      return false;
  } else if (IsSVGAElement(*GetElement())) {
    // http://www.w3.org/2003/01/REC-SVG11-20030114-errata#linking-text-environment
    // The 'a' element may contain any element that its parent may contain,
    // except itself.
    if (child_node && IsSVGAElement(*child_node))
      return false;
    if (Parent() && Parent()->IsSVG())
      return Parent()->IsChildAllowed(child, style);
  }
  return LayoutSVGContainer::IsChildAllowed(child, style);
}

void LayoutSVGTransformableContainer::SetNeedsTransformUpdate() {
  // The transform paint property relies on the SVG transform being up-to-date
  // (see: PaintPropertyTreeBuilder::updateTransformForNonRootSVG).
  SetNeedsPaintPropertyUpdate();
  needs_transform_update_ = true;
}

bool LayoutSVGTransformableContainer::IsUseElement() const {
  const SVGElement& element = *GetElement();
  if (IsSVGUseElement(element))
    return true;
  // Nested <use> are replaced by <g> during shadow tree expansion.
  if (IsA<SVGGElement>(element) && To<SVGGElement>(element).InUseShadowTree())
    return IsSVGUseElement(element.CorrespondingElement());
  return false;
}

SVGTransformChange LayoutSVGTransformableContainer::CalculateLocalTransform() {
  SVGElement* element = GetElement();
  DCHECK(element);

  // If we're either the LayoutObject for a <use> element, or for any <g>
  // element inside the shadow tree, that was created during the use/symbol/svg
  // expansion in SVGUseElement. These containers need to respect the
  // translations induced by their corresponding use elements x/y attributes.
  if (IsUseElement()) {
    const ComputedStyle& style = StyleRef();
    const SVGComputedStyle& svg_style = style.SvgStyle();
    SVGLengthContext length_context(element);
    FloatSize translation(ToFloatSize(
        length_context.ResolveLengthPair(svg_style.X(), svg_style.Y(), style)));
    // TODO(fs): Signal this on style update instead.
    if (translation != additional_translation_)
      SetNeedsTransformUpdate();
    additional_translation_ = translation;
  }

  if (!needs_transform_update_)
    return SVGTransformChange::kNone;

  SVGTransformChangeDetector change_detector(local_transform_);
  local_transform_ =
      element->CalculateTransform(SVGElement::kIncludeMotionTransform);
  local_transform_.Translate(additional_translation_.Width(),
                             additional_translation_.Height());
  needs_transform_update_ = false;
  return change_detector.ComputeChange(local_transform_);
}

}  // namespace blink

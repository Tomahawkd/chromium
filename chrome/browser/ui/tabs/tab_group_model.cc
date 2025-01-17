// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "chrome/browser/ui/tabs/tab_group_model.h"

#include <map>
#include <memory>
#include <utility>
#include <vector>

#include "base/optional.h"
#include "chrome/browser/ui/tabs/tab_group.h"
#include "chrome/browser/ui/tabs/tab_group_id.h"
#include "chrome/browser/ui/tabs/tab_group_visual_data.h"
#include "chrome/browser/ui/tabs/tab_strip_model.h"

TabGroupModel::TabGroupModel(TabStripModel* model) : model_(model) {}
TabGroupModel::~TabGroupModel() {}

TabGroup* TabGroupModel::AddTabGroup(
    TabGroupId id,
    base::Optional<TabGroupVisualData> visual_data) {
  auto tab_group = std::make_unique<TabGroup>(
      model_, id, visual_data.value_or(TabGroupVisualData()));
  groups_[id] = std::move(tab_group);
  return groups_[id].get();
}

bool TabGroupModel::ContainsTabGroup(TabGroupId id) const {
  return base::Contains(groups_, id);
}

// TODO(connily): This should DCHECK(ContainsTabGroup(id)) instead of returning
// nullptr. Other places should be checking for ContainsTabGroup().
TabGroup* TabGroupModel::GetTabGroup(TabGroupId id) const {
  return ContainsTabGroup(id) ? groups_.find(id)->second.get() : nullptr;
}

void TabGroupModel::RemoveTabGroup(TabGroupId id) {
  DCHECK(ContainsTabGroup(id));
  groups_.erase(id);
}

std::vector<TabGroupId> TabGroupModel::ListTabGroups() const {
  std::vector<TabGroupId> group_ids;
  group_ids.reserve(groups_.size());
  for (const auto& id_group_pair : groups_)
    group_ids.push_back(id_group_pair.first);
  return group_ids;
}

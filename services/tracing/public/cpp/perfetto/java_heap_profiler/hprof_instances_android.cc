// Copyright 2019 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include "services/tracing/public/cpp/perfetto/java_heap_profiler/hprof_instances_android.h"

#include "services/tracing/public/cpp/perfetto/java_heap_profiler/hprof_data_type_android.h"

namespace tracing {

Instance::Instance(uint64_t object_id, std::string type_name)
    : type_name(type_name), object_id(object_id) {}

Instance::Instance(uint64_t object_id, uint32_t size)
    : object_id(object_id), size(size) {}

Instance::Instance(uint64_t object_id, uint32_t size, std::string type_name)
    : type_name(type_name), object_id(object_id), size(size) {}

Instance::~Instance() {}

Instance::Instance(const Instance& other) = default;

ClassInstance::ClassInstance(uint64_t object_id,
                             uint64_t class_id,
                             uint32_t temp_data_position,
                             uint32_t size)
    : base_instance(object_id, size),
      class_id(class_id),
      temp_data_position(temp_data_position) {}

Field::Field(std::string name, DataType type, uint64_t object_id)
    : name(name), type(type), object_id(object_id) {}

ClassObject::~ClassObject() {}

ClassObject::ClassObject(uint64_t object_id, std::string type_name)
    : base_instance(object_id, type_name) {}

ObjectArrayInstance::ObjectArrayInstance(uint64_t object_id,
                                         uint64_t class_id,
                                         uint32_t temp_data_position,
                                         uint32_t temp_data_length,
                                         uint32_t size)
    : base_instance(object_id, size),
      class_id(class_id),
      temp_data_position(temp_data_position),
      temp_data_length(temp_data_length) {}

PrimitiveArrayInstance::PrimitiveArrayInstance(uint64_t object_id,
                                               DataType type,
                                               std::string type_name,
                                               uint32_t size)
    : base_instance(object_id, size, type_name), type(type) {}

}  // namespace tracing

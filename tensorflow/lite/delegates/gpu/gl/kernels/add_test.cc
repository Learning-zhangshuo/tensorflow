/* Copyright 2019 The TensorFlow Authors. All Rights Reserved.

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
==============================================================================*/

#include "tensorflow/lite/delegates/gpu/gl/kernels/add.h"

#include <vector>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "tensorflow/lite/delegates/gpu/common/operations.h"
#include "tensorflow/lite/delegates/gpu/gl/kernels/test_util.h"

using ::testing::FloatNear;
using ::testing::Pointwise;

namespace tflite {
namespace gpu {
namespace gl {
namespace {

TEST(AddTest, AddsTwoInputTensors) {
  TensorRefFloat32 augend, addend, output;
  augend.type = DataType::FLOAT32;
  augend.ref = 0;
  augend.shape = BHWC(1, 2, 2, 1);

  addend.type = DataType::FLOAT32;
  addend.ref = 1;
  addend.shape = BHWC(1, 2, 2, 1);

  output.type = DataType::FLOAT32;
  output.ref = 2;
  output.shape = BHWC(1, 2, 2, 1);

  AddAttributes attr;
  SingleOpModel model({ToString(OperationType::ADD), std::move(attr)},
                      {augend, addend}, {output});
  ASSERT_TRUE(model.PopulateTensor(0, {-2.0, 0.2, 0.7, 0.8}));
  ASSERT_TRUE(model.PopulateTensor(1, {0.1, 0.2, 0.3, 0.5}));
  ASSERT_TRUE(model.Invoke(*NewAddNodeShader()));
  EXPECT_THAT(model.GetOutput(0),
              Pointwise(FloatNear(1e-6), {-1.9, 0.4, 1.0, 1.3}));
}

TEST(AddTest, AddsOneInputTensorWithBroadcast) {
  AddAttributes attr;
  attr.param = 0.1f;
  TensorRefFloat32 input, output;
  input.type = DataType::FLOAT32;
  input.ref = 0;
  input.shape = BHWC(1, 3, 1, 2);

  output.type = DataType::FLOAT32;
  output.ref = 1;
  output.shape = BHWC(1, 3, 1, 2);

  SingleOpModel model({ToString(OperationType::ADD), std::move(attr)}, {input},
                      {output});
  ASSERT_TRUE(model.PopulateTensor(0, {-2.0, 0.2, 0.7, 0.8, 1.1, 2.0}));
  ASSERT_TRUE(model.Invoke(*NewAddNodeShader()));
  EXPECT_THAT(model.GetOutput(0),
              Pointwise(FloatNear(1e-6), {-1.9, 0.3, 0.8, 0.9, 1.2, 2.1}));
}

}  // namespace
}  // namespace gl
}  // namespace gpu
}  // namespace tflite

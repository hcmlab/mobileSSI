/* Copyright 2016 The TensorFlow Authors. All Rights Reserved.

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

#ifndef TENSORFLOW_CORE_COMMON_RUNTIME_GRAPH_RUNNER_H_
#define TENSORFLOW_CORE_COMMON_RUNTIME_GRAPH_RUNNER_H_

#include <memory>
#include <string>
#include <vector>

#include "tensorflow/core/framework/function.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/graph/graph.h"
#include "tensorflow/core/lib/core/errors.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/macros.h"
#include "tensorflow/core/platform/mutex.h"
#include "tensorflow/core/platform/thread_annotations.h"
#include "tensorflow/core/platform/types.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {

// GraphRunner takes a Graph, some inputs to feed, and some outputs
// to fetch and executes the graph required to feed and fetch the
// inputs and outputs.
//
// This class is only meant for internal use where one needs to
// partially evaluate inexpensive nodes in a graph, such as for shape
// inference or for constant folding.  Because of its limited, simple
// use-cases, it executes all computation on the CPU and is not meant
// to be particularly lightweight, fast, or efficient.
class GraphRunner {
 public:
  // Function semantics for `inputs`, `output_names` and `outputs`
  // matches those from Session::Run().
  //
  // REQUIRES: `graph`, `env`, and `outputs` are not nullptr.
  // `function_library` may be nullptr.
  typedef std::vector<std::pair<string, Tensor>> NamedTensorList;
  static Status Run(Graph* graph, FunctionLibraryRuntime* function_library,
                    Env* env, const NamedTensorList& inputs,
                    const std::vector<string>& output_names,
                    std::vector<Tensor>* outputs);
};

}  // namespace tensorflow

#endif  // TENSORFLOW_CORE_COMMON_RUNTIME_GRAPH_RUNNER_H_

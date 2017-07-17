/* Copyright 2017 The TensorFlow Authors. All Rights Reserved.

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

#ifndef THIRD_PARTY_TENSORFLOW_CORE_KERNELS_REMOTE_FUSED_GRAPH_EXECUTE_UTILS_H_
#define THIRD_PARTY_TENSORFLOW_CORE_KERNELS_REMOTE_FUSED_GRAPH_EXECUTE_UTILS_H_

#include <unordered_map>

#include "tensorflow/core/framework/graph.pb.h"
#include "tensorflow/core/framework/remote_fused_graph_execute_info.pb.h"
#include "tensorflow/core/kernels/i_remote_fused_graph_executor.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/platform/macros.h"

namespace tensorflow {

// RemoteFusedGraphExecuteUtils provides APIs to register and get builder
// functions for IRemoteFusedGraphExecutor.
class RemoteFusedGraphExecuteUtils {
 public:
  using ExecutorBuildFunc = std::function<Status(
      std::unique_ptr<IRemoteFusedGraphExecutor>* executor)>;
  // Registrar class for IRemoteFusedGraphExecutor.
  class ExecutorBuildRegistrar {
   public:
    ExecutorBuildRegistrar(const string& name, ExecutorBuildFunc func);

   private:
    TF_DISALLOW_COPY_AND_ASSIGN(ExecutorBuildRegistrar);
  };
  using ExecutorBuildRegistry = std::map<string, ExecutorBuildFunc>;

  using TensorShapeMap =
      std::unordered_map<string, std::pair<DataType, TensorShape>>;

  // Return registered ExecutorBuildFunc for given name.
  static const ExecutorBuildFunc* GetExecutorBuildFunc(const string& name);

  // To determine shapes of output tensors of all nodes, dryrun the graph.
  // This function supplies memory allocation information when loading
  // the graph. This function is used to verify shape inference and actual
  // output shape.
  static Status DryRunInference(
      const GraphDef& graph_def,
      const std::vector<std::pair<string, Tensor>>& input_node_info_list,
      const std::vector<string>& output_node_names,
      const bool initialize_by_zero,
      std::vector<tensorflow::Tensor>* output_tensors);

  // Dry run inference to obtain shapes for all nodes.
  // CAVEAT: Do not add or modify output_tensors in output_tensor_info
  // otherwise, address map may be broken by re-allocation inside
  // std::vector.
  static Status DryRunInferenceForAllNode(
      const GraphDef& graph_def,
      const std::vector<std::pair<string, Tensor>>& input_node_info_list,
      const bool initialize_by_zero, TensorShapeMap* tensor_shape_map);

  static bool IsInputNode(
      const std::vector<std::pair<string, Tensor>>& input_node_info_list,
      const string& node_name);

  static void ConvertToTensorShapeMap(
      const std::vector<std::pair<string, Tensor>>& input_node_info_list,
      const std::vector<string>& output_node_names,
      const std::vector<tensorflow::Tensor>& output_tensors,
      TensorShapeMap* tensor_shape_map);

 private:
  static ExecutorBuildRegistry* GetExecutorBuildRegistry();

  TF_DISALLOW_COPY_AND_ASSIGN(RemoteFusedGraphExecuteUtils);
};
}  // namespace tensorflow

#endif  // THIRD_PARTY_TENSORFLOW_CORE_KERNELS_REMOTE_FUSED_GRAPH_EXECUTE_UTILS_H_

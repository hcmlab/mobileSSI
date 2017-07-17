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

#ifndef TENSORFLOW_GRAPPLER_CLUSTERS_CLUSTER_H_
#define TENSORFLOW_GRAPPLER_CLUSTERS_CLUSTER_H_

#include <string>
#include <utility>
#include <vector>

#include "tensorflow/core/framework/device_attributes.pb.h"
#include "tensorflow/core/framework/tensor.h"
#include "tensorflow/core/grappler/grappler_item.h"
#include "tensorflow/core/lib/core/status.h"
#include "tensorflow/core/public/session.h"

namespace tensorflow {
namespace grappler {

// A cluster represents of collection of hardware resources available to run
// the TensorFlow model.
// A process can only create a single cluster at a time.
class Cluster {
 public:
  explicit Cluster(int timeout_s);
  virtual ~Cluster();

  // Provision the hardware resources needed to run TensorFlow and start a
  // TensorFlow session that can take advantage of these resources.
  // The actual resources that are leveraged depend on the type of cluster
  // instantiated.
  // Returns OK iff all the requested resources could be reserved and a
  // TensorFlow session successfully created. Returns an error otherwise.
  // There is no graceful degradation to handle the case where only a subset
  // of the requested resources are available.
  virtual Status Provision() = 0;

  // Set the number of steps required to warmup TensorFlow. Must be called
  // before Provision().
  void SetNumWarmupSteps(int num_steps);

  // Return the list of TensorFlow devices that are available to execute a
  // graph. This is empty until provision() is called.
  const std::vector<DeviceAttributes>& GetDevices() const { return devices_; }

  // Convenience method that returns the set of device names.
  const std::vector<string> GetDeviceNames() const {
    std::vector<string> device_names;
    device_names.reserve(devices_.size());
    for (const auto& device : devices_) {
      device_names.push_back(device.name());
    }
    return device_names;
  }

  // Prepare the session to run the specified grappler item. This include
  // initializing all the model variables.
  virtual Status Initialize(const GrapplerItem& item) = 0;

  // Run the specified graph_def and return the corresponding metadata.
  virtual Status Run(const GraphDef& graph_def,
                     const std::vector<std::pair<string, Tensor>>& feed,
                     const std::vector<string>& fetch,
                     RunMetadata* metadata) = 0;

 protected:
  std::vector<DeviceAttributes> devices_;
  const int timeout_s_;
  SessionOptions options_;
  RunOptions run_options_;
};

}  // end namespace grappler
}  // end namespace tensorflow

#endif  // TENSORFLOW_GRAPPLER_CLUSTERS_CLUSTER_H_

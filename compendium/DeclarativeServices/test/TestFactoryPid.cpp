/*=============================================================================

  Library: CppMicroServices

  Copyright (c) The CppMicroServices developers. See the COPYRIGHT
  file at the top-level directory of this distribution and at
  https://github.com/CppMicroServices/CppMicroServices/COPYRIGHT .

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.

=============================================================================*/

#include "TestFixture.hpp"
#include "gtest/gtest.h"

#include "TestInterfaces/Interfaces.hpp"
#include <cppmicroservices/ServiceTracker.h>

namespace test {
/**
   * Verify a component with default name(=implementation class name) is loaded properly
   */

TEST_F(tServiceComponent, testFactoryPidConstruction)
{
  
 // Start the test bundle containing the factory component name.
  std::string factoryComponentName = "sample::ServiceComponentCA20";
  cppmicroservices::Bundle testBundle = StartTestBundle("TestBundleDSCA20");

  // Use DS runtime service to validate the component description and 
  // Verify that DS is finished creating the component data structures. 
  scr::dto::ComponentDescriptionDTO compDescDTO;
  auto compConfigs = GetComponentConfigs(testBundle, factoryComponentName, compDescDTO);
  EXPECT_EQ(compConfigs.size(), 1ul) << "One default config expected";
  EXPECT_EQ(compConfigs.at(0).state, scr::dto::ComponentState::UNSATISFIED_REFERENCE) 
      << "factory component state should be UNSATISIFIED_REFERENCE";
  
 // Get a service reference to ConfigAdmin to create the factory component instance.
  auto configAdminService =
    GetInstance<cppmicroservices::service::cm::ConfigurationAdmin>();
   ASSERT_TRUE(configAdminService) << "GetService failed for ConfigurationAdmin";
 
  //Create factory configuration object
  auto factoryConfig =
    configAdminService->CreateFactoryConfiguration(factoryComponentName);
  auto factoryInstance = factoryConfig->GetPid();

  // CreateFactoryConfiguration created the configuration object on
  // which the component is configured but it created it with no
  // properties. Update the properties before instantiating the factory
  // instance.
  cppmicroservices::AnyMap props(
    cppmicroservices::AnyMap::UNORDERED_MAP_CASEINSENSITIVE_KEYS);
  const std::string instanceId{ "instance1" };
  props["uniqueProp"] = instanceId;
  factoryConfig->Update(props);

  // The Update properties sends an asynchronous request to DS to
  // update the properties. Must wait until DS is finished before we can continue. 
  // Use DS runtime service to validate the component properties. May take more
  // than one try. 
     
   auto result = RepeatTaskUntilOrTimeout(
    [&compDescDTO, &compConfigs, this, &testBundle, &factoryInstance]() { 
       compDescDTO =
       dsRuntimeService->GetComponentDescriptionDTO(testBundle, factoryInstance);
       if (compDescDTO.name != "") {
          compConfigs =
          this->dsRuntimeService->GetComponentConfigurationDTOs(compDescDTO); 
       }
      },
     [&compConfigs, &instanceId]() -> bool { 
      if (compConfigs.size() == 1) {
          auto properties = compConfigs.at(0).properties;
          auto id = properties.find("uniqueProp");
          if (id != properties.end()) {
            return (id->second == instanceId);
          }
      }
      return false;
    });

  ASSERT_TRUE(result) << "Timed out waiting for Update Configuration"
                         "to complete.";
   EXPECT_EQ(compConfigs.size(), 1ul) << "One default config expected";
   EXPECT_EQ(compConfigs.at(0).state, scr::dto::ComponentState::SATISFIED) 
       << "Factory instance state should be SATISFIED";


    //Request a service reference to the new component instance. This will
   //cause DS to construct the instance with the updated properties.
   auto instance = GetInstance<test::CAInterface>();
   ASSERT_TRUE(instance) << "GetService failed for CAInterface";

  //Confirm factory instance was created with the correct properties

  auto instanceProps = instance->GetProperties();
  auto uniqueProp = instanceProps.find("uniqueProp");
 
  ASSERT_TRUE(uniqueProp != instanceProps.end())
    << "uniqueProp not found in constructed instance";
  EXPECT_EQ(uniqueProp->second, instanceId);
  

}
}
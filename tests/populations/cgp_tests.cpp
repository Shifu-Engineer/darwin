// Copyright 2019 The Darwin Neuroevolution Framework Authors.
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "dummy_domain.h"

#include <core/darwin.h>
#include <core/utils.h>
#include <populations/cgp/cgp.h>
#include <populations/cgp/genotype.h>
#include <populations/cgp/population.h>

#include <third_party/json/json.h>
using nlohmann::json;

#include <tests/testcase_output.h>
#include <third_party/gtest/gtest.h>

#include <vector>
using namespace std;

namespace cgp_tests {

struct CgpTest : public testing::Test {
  static constexpr int kInputs = 5;
  static constexpr int kOutputs = 4;

  CgpTest() {
    auto factory = darwin::registry()->populations.find("cgp");
    CHECK(factory != nullptr);

    domain = make_unique<DummyDomain>(kInputs, kOutputs);
    
    config = make_unique<cgp::Config>();
    config->rows = 7;
    config->columns = 8;
    config->levels_back = 4;
    config->fn_basic_constants = true;
    config->fn_basic_arithmetic = true;

    population = factory->create(*config, *domain);
    CHECK(population);
  }

  unique_ptr<cgp::Config> config;
  unique_ptr<DummyDomain> domain;
  unique_ptr<darwin::Population> population;
};

TEST_F(CgpTest, FunctionGene_LoadSave) {
  core_test::TestCaseOutput output;
  
  cgp::FunctionGene gene;
  gene.function = cgp::FunctionId::Multiply;
  for (cgp::IndexType i = 0; i < gene.connections.size(); ++i) {
    gene.connections[i] = i;
  }

  json json_obj = gene;
  fprintf(output, "%s", json_obj.dump(2).c_str());
  
  cgp::FunctionGene gene_clone = json_obj;
  EXPECT_EQ(gene_clone.function, gene.function);
  EXPECT_EQ(gene_clone.connections, gene.connections);
}

TEST_F(CgpTest, OutputGene_LoadSave) {
  core_test::TestCaseOutput output;
  
  cgp::OutputGene gene;
  gene.connection = 5;

  json json_obj = gene;
  fprintf(output, "%s", json_obj.dump(2).c_str());
  
  cgp::OutputGene gene_clone = json_obj;
  EXPECT_EQ(gene_clone.connection, gene.connection);
}

TEST_F(CgpTest, Genotype) {
  const auto cgp_population = dynamic_cast<const cgp::Population*>(population.get());
  ASSERT_NE(cgp_population, nullptr);

  cgp::Genotype genotype(cgp_population);
  genotype.createPrimordialSeed();

  constexpr int kTestMutationCount = 1000;
  for (int i = 0; i < kTestMutationCount; ++i) {
    genotype.mutate(1.0f, 1.0f);
  }

  json json_obj = genotype.save();
  cgp::Genotype loaded_genotype(cgp_population);
  loaded_genotype.load(json_obj);
  EXPECT_EQ(loaded_genotype, genotype);
}

}  // namespace cgp_tests
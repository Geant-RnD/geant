#include "CommonBenchmark.h"
#include "KleinNishinaTestCommon.h"

void PrepareKNScalarPrims(std::vector<LightTrack> &out, int N)
{
  CreateParticles(kKNminEn, kKNmaxEn, true, TestParticleType::Em, out, N);
}

void PrepareKNVectorPrims(LightTrack_v &out, int N)
{
  CreateParticles(kKNminEn, kKNmaxEn, true, TestParticleType::Em, out, N);
}

int main(int argc, char **argv)
{
  PrepareWorld();

  auto td = PrepareTaskData();

  std::unique_ptr<EMModel> knRej        = InitEMModel(new KleinNishinaComptonModel, kKNminEn, kKNmaxEn, false);
  std::unique_ptr<EMModel> knAliasTable = InitEMModel(new KleinNishinaComptonModel, kKNminEn, kKNmaxEn, true);

  EMMODEL_REGISTER_SCALAR_BENCHMARK("KleinNishinaAliasScal", knAliasTable.get(), PrepareKNScalarPrims, td.get(),
                                    kBasketSize);
  EMMODEL_REGISTER_VECTOR_BENCHMARK("KleinNishinaAliasVec", knAliasTable.get(), PrepareKNVectorPrims, td.get(),
                                    kBasketSize);
  EMMODEL_REGISTER_SCALAR_BENCHMARK("KleinNishinaRejScal", knRej.get(), PrepareKNScalarPrims, td.get(), kBasketSize);
  EMMODEL_REGISTER_VECTOR_BENCHMARK("KleinNishinaRejVec", knRej.get(), PrepareKNVectorPrims, td.get(), kBasketSize);

  ::benchmark::Initialize(&argc, argv);
  if (::benchmark::ReportUnrecognizedArguments(argc, argv)) return 1;
  ::benchmark::RunSpecifiedBenchmarks();

  CleanTaskData(td.get());
}

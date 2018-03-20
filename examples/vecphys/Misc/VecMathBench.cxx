#include <benchmark/benchmark.h>

#include "Geant/VecRngWrapper.h"

const int kN = 256;

static void ScalarExp(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;
  std::vector<double> rnd;
  for (int i = 0; i < state.range(0); ++i) {
    rnd.push_back(wrapper.uniform());
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); ++i) {
      sum += std::exp(rnd[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(ScalarExp)->Arg(kN);

static void VectorExp(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;

  double *rnd = (double *)vecCore::AlignedAlloc(kPhysDVAlign, state.range(0) * sizeof(double));
  for (int i = 0; i < state.range(0); ++i) {
    rnd[i] = wrapper.uniform();
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); i += kPhysDVWidth) {
      PhysDV tmp;
      vecCore::Load(tmp, &rnd[i]);
      tmp = vecCore::math::Exp(tmp);
      sum += vecCore::ReduceAdd(tmp);
    }
    benchmark::DoNotOptimize(sum);
  }
  vecCore::AlignedFree(rnd);
}
BENCHMARK(VectorExp)->Arg(kN);

static void ScalarLog(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;
  std::vector<double> rnd;
  for (int i = 0; i < state.range(0); ++i) {
    rnd.push_back(wrapper.uniform());
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); ++i) {
      sum += std::log(rnd[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(ScalarLog)->Arg(kN);

static void VectorLog(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;

  double *rnd = (double *)vecCore::AlignedAlloc(kPhysDVAlign, state.range(0) * sizeof(double));
  for (int i = 0; i < state.range(0); ++i) {
    rnd[i] = wrapper.uniform();
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); i += kPhysDVWidth) {
      PhysDV tmp;
      vecCore::Load(tmp, &rnd[i]);
      tmp = vecCore::math::Log(tmp);
      sum += vecCore::ReduceAdd(tmp);
    }
    benchmark::DoNotOptimize(sum);
  }
  vecCore::AlignedFree(rnd);
}
BENCHMARK(VectorLog)->Arg(kN);

static void ScalarSqrt(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;
  std::vector<double> rnd;
  for (int i = 0; i < state.range(0); ++i) {
    rnd.push_back(wrapper.uniform());
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); ++i) {
      sum += std::sqrt(rnd[i]);
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(ScalarSqrt)->Arg(kN);

static void VectorSqrt(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;

  double *rnd = (double *)vecCore::AlignedAlloc(kPhysDVAlign, state.range(0) * sizeof(double));
  for (int i = 0; i < state.range(0); ++i) {
    rnd[i] = wrapper.uniform();
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); i += kPhysDVWidth) {
      PhysDV tmp;
      vecCore::Load(tmp, &rnd[i]);
      tmp = vecCore::math::Sqrt(tmp);
      sum += vecCore::ReduceAdd(tmp);
    }
    benchmark::DoNotOptimize(sum);
  }
  vecCore::AlignedFree(rnd);
}
BENCHMARK(VectorSqrt)->Arg(kN);

static void ScalarDiv(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;
  std::vector<double> rnd;
  for (int i = 0; i < state.range(0); ++i) {
    rnd.push_back(wrapper.uniform());
  }
  for (auto _ : state) {
    double sum = 0.0;
    for (int i = 0; i < state.range(0); ++i) {
      double tmp = rnd[i];
      tmp        = 1.0 / tmp;
      tmp        = 1.0 / tmp;
      tmp        = 1.0 / tmp;
      tmp        = 1.0 / tmp;
      sum += tmp;
    }
    benchmark::DoNotOptimize(sum);
  }
}
BENCHMARK(ScalarDiv)->Arg(kN);

static void VectorDiv(benchmark::State &state)
{
  geant::VecRngWrapper wrapper;

  double *rnd = (double *)vecCore::AlignedAlloc(kPhysDVAlign, state.range(0) * sizeof(double));
  for (int i = 0; i < state.range(0); ++i) {
    rnd[i] = wrapper.uniform();
  }
  for (auto _ : state) {
    PhysDV sum = 0.0;
    for (int i = 0; i < state.range(0); i += kPhysDVWidth) {
      PhysDV tmp;
      vecCore::Load(tmp, &rnd[i]);
      tmp = 1.0 / tmp;
      tmp = 1.0 / tmp;
      tmp = 1.0 / tmp;
      tmp = 1.0 / tmp;
      sum += tmp;
    }
    benchmark::DoNotOptimize(sum);
  }
  vecCore::AlignedFree(rnd);
}
BENCHMARK(VectorDiv)->Arg(kN);

BENCHMARK_MAIN();

#include "adl/Pipelines.h"

#include "adl/Analysis/ISA/Passes.h"
#include "adl/Transforms/ISA/Passes.h"
#include "adl/Transforms/Proc/Passes.h"

#include "mlir/Pass/PassManager.h"
#include "mlir/Pass/PassRegistry.h"

using namespace mlir;

namespace adl {
namespace {

auto buildProcDecoderPipeline(OpPassManager &pm) -> void {
  pm.addPass(adl::isa::createVerifyDecoderTablePass());
  pm.addPass(adl::isa::createMaterializeDecoderTablePass());
  pm.addPass(adl::proc::createLowerISADecoderToProcPass());
  pm.addPass(adl::proc::createExpandProcDecoderPass());
}

} // namespace

auto registerAllPipelines() -> void {
  static PassPipelineRegistration<> buildProcDecoderPipelineRegistration(
      "adl-build-proc-decoder",
      "Verify ISA encodings, materialize decoder metadata, lower it to proc, "
      "and expand it into executable proc decoder IR.",
      [](OpPassManager &pm) -> void { buildProcDecoderPipeline(pm); });
}

} // namespace adl

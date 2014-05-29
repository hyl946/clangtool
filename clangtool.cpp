#include "llvm/Pass.h"
#include "llvm/IR/BasicBlock.h"
#include "llvm/PassRegistry.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
// For output
#include "llvm/Support/raw_ostream.h"

using namespace llvm;

// Example pass: just dumps the insns for every block.
namespace {
  struct ClangTool : public BasicBlockPass {
    static char ID;
    ClangTool() : BasicBlockPass(ID) {}
    virtual bool runOnBasicBlock(BasicBlock &BB) {
      BasicBlock::iterator i;
      errs() << "Basic Block\n";
      for( i=BB.begin(); i!=BB.end(); i++ ) {
        errs() << "  " << i->getOpcodeName() << "\n";
      }
      return false;
    }
  };
}

// Pass info
char ClangTool::ID = 0; // LLVM ignores the actual value
static RegisterPass<ClangTool> X("clangtool", "Example pass", false, false);

// Pass loading stuff
// Run: clang -Xclang -load -Xclang pass.so ...

// This function is of type PassManagerBuilder::ExtensionFn
static void loadPass(const PassManagerBuilder &Builder, PassManagerBase &PM) {
  PM.add(new ClangTool());
}
// Declares a static instance of the RegisterStandardPasses class, which gets
// recognized by Clang, running loadPass automatically.
static RegisterStandardPasses clangtoolLoader(PassManagerBuilder::EP_OptimizerLast, loadPass);

// Note: The location EP_OptimizerLast places this pass at the end of the list
// of *optimizations*. That means on -O0, it does not get run.
//
// If you want to run on -O0, you can use EP_EnabledOnOptLevel0, but be aware
// that under -O1, 2, and 3, the pass will be run much earlier (and llvm will
// optimize the code after your pass runs).
// 
// Check the header file PassManagerBuilder.h for details.
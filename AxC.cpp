// AxC.cpp : Defines the entry point for the application.
//

#include "AxC.h"

#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <llvm/IRReader/IRReader.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/raw_ostream.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Target/TargetOptions.h>
#include <llvm/Bitcode/BitcodeReader.h>
#include <llvm/TargetParser/Host.h>
#include <llvm/IR/LegacyPassManager.h>
#include <algorithm>
#include <cassert>
#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <map>
#include <memory>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

using namespace llvm;
using namespace std;
using namespace llvm::sys;

int main(int argc, char** argv) {
    if (argc < 3) {
        errs() << "Usage: " << argv[0] << " <input.ll> <output.o>\n";
        return 1;
    }

    // Initialize LLVM Targets
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    LLVMContext Context;
    SMDiagnostic Err;

    // Parse the LLVM IR file
    std::unique_ptr<Module> Mod = parseIRFile(argv[1], Err, Context);
    if (!Mod) {
        cout << "Error: Failed to parse IR file.\n";
        Err.print(argv[0], errs());
        return 1;
    }

    // Get target triple for the host system
    auto TargetTriple = sys::getDefaultTargetTriple();
    Mod->setTargetTriple(TargetTriple);

    // Look up the target
    std::string Error;
    const Target* Target = TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target) {
        errs() << Error;
        return 1;
    }

    // Create target machine
    TargetOptions opt;
    auto TargetMachine = Target->createTargetMachine(TargetTriple, "generic", "", opt, std::nullopt);
    if (!TargetMachine) {
        cout << "Error: Unable to create target machine.\n";
        return 1;
    }

    // Generate object file
    std::error_code EC;
    raw_fd_ostream Dest(argv[2], EC, sys::fs::OF_None);
    if (EC) {
        llvm::errs() << "Error opening file: " << EC.message() << "\n";
        return 1;
    }

    // Pass manager to generate object file
    legacy::PassManager PM;
    if (TargetMachine->addPassesToEmitFile(PM, Dest, nullptr, CodeGenFileType::ObjectFile)) {
        llvm::errs() << "Error: Target machine cannot emit object file.\n";
        return 1;
    }

    PM.run(*Mod);
    Dest.flush();

    llvm::outs() << "Object file written to " << argv[2] << "\n";
    string command = "clang " + string(argv[2]) + " -o example";
    system(command.c_str());
    llvm::outs() << "Generated executable";
    return 0;
}
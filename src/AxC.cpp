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
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "AST.h"
#include "Lexer.h"
#include "Parser.h"
#include "TreeWalker.h"

using namespace llvm;
using namespace std;
using namespace llvm::sys;

string readFileContents(const std::string& filePath) {
    std::ifstream file(filePath);
    
    if (!file) {
        throw std::runtime_error("Could not open file: " + filePath);
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char** argv) {


    if (argc < 3) {
        errs() << "Usage: " << argv[0] << " <program.ax> <exename>\n";
        return 1;
    }

    Parser p(readFileContents(argv[1]));
    TreeWalker treeWalker(p.parseProgram());

    // Initialize LLVM Targets
    InitializeNativeTarget();
    InitializeNativeTargetAsmPrinter();

    LLVMContext Context;
    SMDiagnostic Err;

    // Parse the LLVM IR file
    auto mod = treeWalker.generateIR();
    if (!mod) {
        cout << "Error: Failed to compile program.\n";
        Err.print(argv[0], errs());
        return 1;
    }

    // File path to save IR
    std::string filename = "output.ll";
    std::error_code eC;

    // Create a file output stream
    llvm::raw_fd_ostream file(filename, eC, llvm::sys::fs::OF_Text);

    // Get target triple for the host system
    auto targetTriple = sys::getDefaultTargetTriple();
    mod->setTargetTriple(targetTriple);
    mod->print(file, nullptr);

    // Look up the target
    std::string early_inc_iterator_impl;
    string error;
    const Target* target = TargetRegistry::lookupTarget(targetTriple, error);
    if (!target) {
        errs() << error;
        return 1;
    }

    // Create target machine
    TargetOptions opt;
    auto TargetMachine = target->createTargetMachine(targetTriple, "generic", "", opt, std::nullopt);
    if (!TargetMachine) {
        cout << "Error: Unable to create target machine.\n";
        return 1;
    }

    // Generate object file
    std::error_code EC;
    raw_fd_ostream Dest(string(argv[2]) +".o", EC, sys::fs::OF_None);
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

    if (llvm::verifyModule(*mod, &llvm::errs())) {
        std::cerr << "Module verification failed!\n";
        return -1;
    }

    PM.run(*mod);
    Dest.flush();

    llvm::outs() << "Object file written to " << argv[2] << ".o \n";
    string command = "clang -no-pie " + string(argv[2]) + ".o -o example";
    system(command.c_str());
    llvm::outs() << "Generated executable";
    return 0;
}
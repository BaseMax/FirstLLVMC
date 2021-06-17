// 17 Jun, 2021
// https://github.com/BaseMax/FirstLLVMC

// c headers
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

// llvm headers
#include <llvm-c/Core.h>
#include <llvm-c/TargetMachine.h>

// headers
void llvm_create_program();

LLVMValueRef llvm_create_function_strlen(LLVMModuleRef *Module);

void llvm_module_print();

void llvm_create_object();

void llvm_module_free();
void llvm_builder_free(LLVMBuilderRef Builder);

// global variables
LLVMModuleRef Module;

// functions define
void llvm_create_program() {
	// create main module
	Module = LLVMModuleCreateWithName("module-c");

	// create builder/function
	LLVMBuilderRef Builder = LLVMCreateBuilder();

	// create strlen
	LLVMValueRef FunctionStrlen = llvm_create_function_strlen(&Module);

	// extern printf function
	LLVMTypeRef FunctionPrintfDefineArgs[] = { LLVMPointerType(LLVMInt8Type(), 0) };
	// ref: https://llvm.org/doxygen/group__LLVMCCoreTypeFunction.html#ga8b0c32e7322e5c6c1bf7eb95b0961707
	LLVMTypeRef FunctionPrintfType = LLVMFunctionType(
		LLVMInt32Type(), // return data type
		FunctionPrintfDefineArgs, // type of arguments
		0, // number of arguments
		true // is var arguments!
	);
	LLVMValueRef FunctionPrintf = LLVMAddFunction(Module, "printf", FunctionPrintfType);

	// create main
	LLVMTypeRef FunctionMainType = LLVMFunctionType(
		LLVMVoidType(), // return data type
		NULL, // without arguments
		0, // number of arguments!
		false // not var arguments!
	);

	// add main to module
	LLVMValueRef FunctionMain = LLVMAddFunction(Module, "main", FunctionMainType);

	// add a label at first of the `main` function
	LLVMBasicBlockRef BasicBlock = LLVMAppendBasicBlock(FunctionMain, "entrypoint");
	LLVMPositionBuilderAtEnd(Builder, BasicBlock);

	LLVMValueRef Format = LLVMBuildGlobalStringPtr(
		Builder, // builder
		"Hello, %s.\n", // value
		"format" // name
	);
	LLVMValueRef Name = LLVMBuildGlobalStringPtr(
		Builder, // builder
		"Max Base", // value
		"name" // name
	);

	// create argument list (for printf)
	LLVMValueRef FunctionPrintfArgs[] = { Format, Name };
	// call a function
	LLVMBuildCall(
		Builder,
		FunctionPrintf,
		FunctionPrintfArgs,
		2,
		"printf"
	);

	LLVMValueRef Format2 = LLVMBuildGlobalStringPtr(
		Builder, // builder
		"own_len(Name) = %d\n", // value
		"format2" // name
	);

	// set first argument of printf
	FunctionPrintfArgs[0] = Format2;

	// init the second argument of the printf
	LLVMValueRef FunctionStrlenArgs[] = { Name };
	// set second argument of printf
	FunctionPrintfArgs[1] = LLVMBuildCall(
		Builder,
		FunctionStrlen,
		FunctionStrlenArgs,
		1,
		""
	);

	// call printf
	LLVMBuildCall(
		Builder,
		FunctionPrintf,
		FunctionPrintfArgs,
		2,
		""
	);

	// return void
	LLVMBuildRetVoid(Builder);
}

LLVMValueRef llvm_create_function_strlen(LLVMModuleRef *Module) {
	// create builder of the function
	LLVMBuilderRef Builder = LLVMCreateBuilder();

	// create number 0 with i8 type
	LLVMValueRef Zero8 = LLVMConstInt(LLVMInt8Type(), 0, false); // Do you know? i8 is equal to char
	// create number 0 with i32 type
	LLVMValueRef Zero32 = LLVMConstInt(LLVMInt32Type(), 0, false);
	// create number 1 with i32 type
	LLVMValueRef One32 = LLVMConstInt(LLVMInt32Type(), 1, false); // 1 with int 32bit type

	LLVMTypeRef FunctionStrlenDefineArgs[] = { LLVMPointerType(LLVMInt8Type(), 0) }; // char = int8
	LLVMTypeRef FunctionStrlenType = LLVMFunctionType(
		LLVMInt32Type(), // return type is i32
		FunctionStrlenDefineArgs,
		1, // number of the arguments
		false // not variable arguments!
	);

	LLVMValueRef FunctionStrlen = LLVMAddFunction(*Module, "strlen", FunctionStrlenType);
	LLVMValueRef s = LLVMGetParam(FunctionStrlen, 0);
	LLVMSetValueName(s, "s");

	LLVMBasicBlockRef InitBasicBlock = LLVMAppendBasicBlock(FunctionStrlen, "init");
	LLVMBasicBlockRef CheckBasicBlock = LLVMAppendBasicBlock(FunctionStrlen, "check");
	LLVMBasicBlockRef BodyBasicBlock = LLVMAppendBasicBlock(FunctionStrlen, "body");
	LLVMBasicBlockRef EndBasicBlock = LLVMAppendBasicBlock(FunctionStrlen, "end");

	// start InitBasicBlock label
	LLVMPositionBuilderAtEnd(Builder, InitBasicBlock); // ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html#l03068

	// create variable `i`
	LLVMValueRef i = LLVMBuildAlloca(Builder, LLVMInt32Type(), "i");
	// set default value of `i` as number 0
	LLVMBuildStore(Builder, Zero32, i); // ref: https://llvm.org/doxygen/group__LLVMCCoreInstructionBuilder.html#ga9a320c8b85497624cffd657178fbb08b

	// go to CheckBasicBlock
	LLVMBuildBr(Builder, CheckBasicBlock); // ref: https://llvm.org/doxygen/group__LLVMCCoreInstructionBuilder.html#ga61def0c0fc591008bc9ec04ffc601093

	// start CheckBasicBlock label
	LLVMPositionBuilderAtEnd(Builder, CheckBasicBlock); // ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html#l03068

	// if it's EOF
	LLVMValueRef id_if[] = { LLVMBuildLoad(Builder, i, "") };
	LLVMValueRef If = LLVMBuildICmp( // ref: https://llvm.org/doxygen/group__LLVMCCoreInstructionBuilder.html#ga73559fb71fcb2caee54375378f49d174
		Builder,
		LLVMIntNE,
		Zero8,
		LLVMBuildLoad( // ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html#l03542
			Builder,
			LLVMBuildGEP(Builder, s, id_if, 1, ""), // ref: https://llvm.org/doxygen/group__LLVMCCoreInstructionBuilder.html#gaf392c575dd594d83894d3e71a520ffa4
			""
		),
		""
	);

	// go to EndBasicBlock if it's EOF.
	LLVMBuildCondBr(Builder, If, BodyBasicBlock, EndBasicBlock);

	// start BodyBasicBlock label
	LLVMPositionBuilderAtEnd(Builder, BodyBasicBlock);

	// increase the value of `i`
	LLVMBuildStore( // ref: https://llvm.org/doxygen/group__LLVMCCoreInstructionBuilder.html#ga9a320c8b85497624cffd657178fbb08b
		Builder,
		LLVMBuildAdd( // +
			Builder,
			LLVMBuildLoad( // get value of the `i` variable
				Builder,
				i, // want `i` variable
				""
			),
			One32, // 1
			""
		),
		i // store the new value to the `i`
	);

	// go to CheckBasicBlock
	LLVMBuildBr(Builder, CheckBasicBlock);

	// set EndBasicBlock point, so it's where we return the `i` value
	LLVMPositionBuilderAtEnd(Builder, EndBasicBlock);

	// return i
	LLVMBuildRet(Builder, LLVMBuildLoad(Builder, i, ""));

	return FunctionStrlen;
}

void llvm_module_print() {
	// print IR of whole of the module, not only this builder!
	LLVMDumpModule(Module);
}

void llvm_create_object() {
	char* error_msg;
	char* out_file = "one.o";

	char* host_triple = LLVMGetDefaultTargetTriple();
	char* triple = host_triple;

	LLVMCodeGenOptLevel opt_level = LLVMCodeGenLevelDefault;
	LLVMTargetRef target = NULL;
	LLVMInitializeAllTargetInfos();
	LLVMInitializeAllTargets();
	LLVMInitializeAllAsmPrinters();
	LLVMInitializeAllTargetMCs();

	if(LLVMGetTargetFromTriple(triple, &target, &error_msg)) {
		printf("Error:");
		printf(error_msg);
		LLVMDisposeMessage(error_msg); // aka: free, ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html
		return;
	}

	LLVMTargetMachineRef target_machine = LLVMCreateTargetMachine(target, triple, "", "",
												opt_level, LLVMRelocPIC, LLVMCodeModelDefault); // ref: https://llvm.org/doxygen/c_2TargetMachine_8h.html#a9b0b2b1efd30fad999f2b2a7fdbf8492

	if(LLVMTargetMachineEmitToFile(target_machine, Module, out_file, LLVMObjectFile, &error_msg)) { // ref: https://llvm.org/doxygen/c_2TargetMachine_8h.html#ad9195990094524f9a0cfc80ef213e675
		printf("Error:");
		printf("Failed to output object: %s", error_msg);
		LLVMDisposeMessage(error_msg); // aka: free, ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html
		return;
	}
}

void llvm_module_free() {
	LLVMDisposeModule(Module); // aka: delete in cpp, ref: https://llvm.org/doxygen/IR_2Core_8cpp_source.html
}

void llvm_builder_free(LLVMBuilderRef Builder) {
	LLVMDisposeBuilder(Builder); // Module is a global var!
}

int main() {
	// deploy IR commands
	llvm_create_program();

	// print IR
	llvm_module_print();

	// save the output
	llvm_create_object();

	// free 
	llvm_module_free(Module);
	return 0;
}

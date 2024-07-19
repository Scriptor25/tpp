#pragma once

#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <memory>

namespace tpp
{
	class Builder
	{
	public:
		Builder();

		llvm::LLVMContext &Context() const;
		llvm::Module &Module() const;
		llvm::IRBuilder<> &IRBuilder() const;

		llvm::Value *GenIR(const ExprPtr &ptr);

	private:
		llvm::Value *GenIR(const DefFunctionExpression &e);
		llvm::Value *GenIR(const DefVariableExpression &e);
		llvm::Value *GenIR(const ForExpression &e);
		llvm::Value *GenIR(const WhileExpression &e);
		llvm::Value *GenIR(const IfExpression &e);
		llvm::Value *GenIR(const GroupExpression &e);
		llvm::Value *GenIR(const BinaryExpression &e);
		llvm::Value *GenIR(const CallExpression &e);
		llvm::Value *GenIR(const IndexExpression &e);
		llvm::Value *GenIR(const MemberExpression &e);
		llvm::Value *GenIR(const IDExpression &e);
		llvm::Value *GenIR(const NumberExpression &e);
		llvm::Value *GenIR(const CharExpression &e);
		llvm::Value *GenIR(const StringExpression &e);
		llvm::Value *GenIR(const VarArgsExpression &e);
		llvm::Value *GenIR(const UnaryExpression &e);
		llvm::Value *GenIR(const ObjectExpression &e);
		llvm::Value *GenIR(const ArrayExpression &e);

	private:
		std::unique_ptr<llvm::LLVMContext> m_Context;
		std::unique_ptr<llvm::Module> m_Module;
		std::unique_ptr<llvm::IRBuilder<>> m_Builder;
	};
}

#pragma once

#include <TPP/Backend/Value.hpp>
#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <llvm/IR/Function.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/LLVMContext.h>
#include <llvm/IR/Module.h>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace tpp
{
	struct FunctionInfo
	{
		llvm::Function *Function;

		TypePtr Result;
		std::vector<TypePtr> Args;
		bool IsVarArg;
	};

	class Builder
	{
	public:
		Builder();

		llvm::LLVMContext &Context() const;
		llvm::Module &Module() const;
		llvm::IRBuilder<> &IRBuilder() const;

		bool IsGlobal() const;

		ValuePtr GenIR(const ExprPtr &ptr);
		llvm::Type *GenIR(const TypePtr &ptr);

		llvm::Value *CreateAlloca(llvm::Type *type);

		llvm::Value *GetDefault(const TypePtr &type);
		ValuePtr CreateCast(const ValuePtr &value, const TypePtr &type);

	private:
		void Push();
		void Pop();

		TypePtr GetHigherOrder(const TypePtr &a, const TypePtr &b);

		std::pair<ValuePtr, ValuePtr> CreateHigherOrderCast(const ValuePtr &a, const ValuePtr &b);

		ValuePtr CreateAssign(const ValuePtr &dest, const ValuePtr &src);

		ValuePtr CreateLT(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateGT(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateLE(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateGE(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateEQ(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateNE(const ValuePtr &lhs, const ValuePtr &rhs);

		ValuePtr CreateAdd(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateSub(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateMul(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateDiv(const ValuePtr &lhs, const ValuePtr &rhs);
		ValuePtr CreateRem(const ValuePtr &lhs, const ValuePtr &rhs);

		ValuePtr GenIR(const DefStructExpression &e);
		ValuePtr GenIR(const DefFunctionExpression &e);
		ValuePtr GenIR(const DefVariableExpression &e);
		ValuePtr GenIR(const ForExpression &e);
		ValuePtr GenIR(const WhileExpression &e);
		ValuePtr GenIR(const IfExpression &e);
		ValuePtr GenIR(const GroupExpression &e);
		ValuePtr GenIR(const BinaryExpression &e);
		ValuePtr GenIR(const CallExpression &e);
		ValuePtr GenIR(const IndexExpression &e);
		ValuePtr GenIR(const MemberExpression &e);
		ValuePtr GenIR(const IDExpression &e);
		ValuePtr GenIR(const NumberExpression &e);
		ValuePtr GenIR(const CharExpression &e);
		ValuePtr GenIR(const StringExpression &e);
		ValuePtr GenIR(const VarArgsExpression &e);
		ValuePtr GenIR(const UnaryExpression &e);
		ValuePtr GenIR(const ObjectExpression &e);
		ValuePtr GenIR(const ArrayExpression &e);

		const char *TypeToString(llvm::Type *type);

	private:
		std::unique_ptr<llvm::LLVMContext> m_Context;
		std::unique_ptr<llvm::Module> m_Module;
		std::unique_ptr<llvm::IRBuilder<>> m_Builder;

		llvm::Function *m_Global;

		std::map<llvm::Type *, DefStructExpression> m_Types;
		std::map<llvm::Function *, FunctionInfo> m_FunctionInfos;

		std::vector<std::map<std::string, ValuePtr>> m_Stack;
		std::map<std::string, ValuePtr> m_Variables;
	};
}

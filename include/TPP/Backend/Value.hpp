#pragma once

#include <TPP/Frontend/Frontend.hpp>
#include <llvm/IR/Value.h>
#include <memory>

namespace tpp
{
	class Builder;

	typedef std::shared_ptr<struct Value> ValuePtr;
	typedef std::shared_ptr<struct RValue> RValuePtr;
	typedef std::shared_ptr<struct LValue> LValuePtr;

	struct Value
	{
		Value(Builder &builder, const TypePtr &type);
		virtual ~Value();

		virtual llvm::Value *Get() const = 0;

		TypePtr GetType() const;
		llvm::Type *GetIRType() const;

		Builder &MBuilder;
		TypePtr MType;
		llvm::Type *IRType;
	};

	struct RValue : Value
	{
		static RValuePtr Create(Builder &builder, const TypePtr &type, llvm::Value *value);

		RValue(Builder &builder, const TypePtr &type, llvm::Value *value);

		llvm::Value *Get() const override;

		llvm::Value *MValue;
	};

	struct LValue : Value
	{
		static LValuePtr Alloca(Builder &builder, const TypePtr &type, const ValuePtr &value);
		static LValuePtr Alloca(Builder &builder, const TypePtr &type, llvm::Value *value);
		static LValuePtr Create(Builder &builder, const TypePtr &type, llvm::Value *ptr);

		LValue(Builder &builder, const TypePtr &type, llvm::Value *ptr);

		llvm::Value *Get() const override;

		llvm::Value *GetPtr() const;
		void Store(const ValuePtr &value) const;
		void Store(llvm::Value *value) const;

		llvm::Value *Ptr;
	};
}

#include <TPP/Backend/Builder.hpp>
#include <TPP/Backend/Value.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <memory>

tpp::Value::Value(Builder &builder, const TypePtr &type) : MBuilder(builder), MType(type), IRType(builder.GenIR(type)) {}

tpp::Value::~Value() = default;

tpp::TypePtr tpp::Value::GetType() const { return MType; }

llvm::Type *tpp::Value::GetIRType() const { return IRType; }

tpp::RValuePtr tpp::RValue::Create(Builder &builder, const TypePtr &type, llvm::Value *value)
{
	if (!value) value = builder.GetDefault(type);
	return std::make_shared<RValue>(builder, type, value);
}

tpp::RValue::RValue(Builder &builder, const TypePtr &type, llvm::Value *value) : Value(builder, type), MValue(value) {}

llvm::Value *tpp::RValue::Get() const { return MValue; }

tpp::LValuePtr tpp::LValue::Alloca(Builder &builder, const TypePtr &type, const ValuePtr &value)
{
	auto val = !value ? nullptr : type != value->GetType() ? builder.CreateCast(value, type)->Get() : value->Get();
	return Alloca(builder, type, val);
}

tpp::LValuePtr tpp::LValue::Alloca(Builder &builder, const TypePtr &type, llvm::Value *value)
{
	if (builder.IsGlobal()) error(SourceLocation::UNKNOWN, "cannot allocate local variable in global space");
	if (!value) value = builder.GetDefault(type);

	auto ptr = builder.CreateAlloca(builder.GenIR(type));
	if (value) builder.IRBuilder().CreateStore(value, ptr);
	return std::make_shared<LValue>(builder, type, ptr);
}

tpp::LValuePtr tpp::LValue::Create(Builder &builder, const TypePtr &type, llvm::Value *ptr) { return std::make_shared<LValue>(builder, type, ptr); }

tpp::LValue::LValue(Builder &builder, const TypePtr &type, llvm::Value *ptr) : Value(builder, type), Ptr(ptr) {}

llvm::Value *tpp::LValue::Get() const { return MBuilder.IRBuilder().CreateLoad(GetIRType(), Ptr); }

llvm::Value *tpp::LValue::GetPtr() const { return Ptr; }

void tpp::LValue::Store(const ValuePtr &value) const
{
	ValuePtr val = value;
	if (val->GetType() != GetType()) val = MBuilder.CreateCast(val, GetType());
	Store(value->Get());
}

void tpp::LValue::Store(llvm::Value *value) const { MBuilder.IRBuilder().CreateStore(value, Ptr); }

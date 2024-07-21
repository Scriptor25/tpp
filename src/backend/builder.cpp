#include "llvm/Support/raw_ostream.h"
#include <TPP/Backend/Builder.hpp>
#include <TPP/Backend/Value.hpp>
#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/SourceLocation.hpp>
#include <TPP/Frontend/Type.hpp>
#include <cstddef>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Verifier.h>
#include <memory>
#include <vector>

tpp::Builder::Builder()
{
	m_Context = std::make_unique<llvm::LLVMContext>();
	m_Module = std::make_unique<llvm::Module>("module", *m_Context);
	m_Builder = std::make_unique<llvm::IRBuilder<>>(*m_Context);

	{
		auto function_type = llvm::FunctionType::get(IRBuilder().getVoidTy(), false);
		m_Global = llvm::cast<llvm::Function>(Module().getOrInsertFunction(".global", function_type).getCallee());
		auto entry_block = llvm::BasicBlock::Create(Context(), "entry", m_Global);
		IRBuilder().SetInsertPoint(entry_block);
	}
}

llvm::LLVMContext &tpp::Builder::Context() const { return *m_Context; }

llvm::Module &tpp::Builder::Module() const { return *m_Module; }

llvm::IRBuilder<> &tpp::Builder::IRBuilder() const { return *m_Builder; }

bool tpp::Builder::IsGlobal() const { return m_Stack.empty(); }

tpp::ValuePtr tpp::Builder::GenIR(const ExprPtr &ptr)
{
	if (!ptr) return nullptr;

	if (auto p = std::dynamic_pointer_cast<DefStructExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<DefFunctionExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<DefVariableExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<ForExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<WhileExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<IfExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<GroupExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<BinaryExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<CallExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<IndexExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<MemberExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<IDExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<NumberExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<CharExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<StringExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<VarArgsExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<UnaryExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<ObjectExpression>(ptr)) return GenIR(*p);
	if (auto p = std::dynamic_pointer_cast<ArrayExpression>(ptr)) return GenIR(*p);

	error(ptr->Location, "no GenIR implementation for this expression");
}

llvm::Type *tpp::Builder::GenIR(const TypePtr &ptr)
{
	if (auto p = std::dynamic_pointer_cast<ArrayType>(ptr))
	{
		auto base = GenIR(p->Base);
		return llvm::PointerType::get(base, 0);
	}

	if (auto p = std::dynamic_pointer_cast<FunctionType>(ptr))
	{
		auto result = GenIR(p->Result);
		std::vector<llvm::Type *> args(p->Args.size());
		for (size_t i = 0; i < args.size(); ++i) args[i] = GenIR(p->Args[i]);
		return llvm::FunctionType::get(result, args, p->IsVarArg);
	}

	if (ptr == Type::Get("void")) return IRBuilder().getVoidTy();
	if (ptr == Type::Get("i1")) return IRBuilder().getInt1Ty();
	if (ptr == Type::Get("i8")) return IRBuilder().getInt8Ty();
	if (ptr == Type::Get("i16")) return IRBuilder().getInt16Ty();
	if (ptr == Type::Get("i32")) return IRBuilder().getInt32Ty();
	if (ptr == Type::Get("i64")) return IRBuilder().getInt64Ty();
	if (ptr == Type::Get("i128")) return IRBuilder().getInt128Ty();
	if (ptr == Type::Get("f16")) return IRBuilder().getHalfTy();
	if (ptr == Type::Get("f32")) return IRBuilder().getFloatTy();
	if (ptr == Type::Get("f64")) return IRBuilder().getDoubleTy();

	if (auto type = llvm::StructType::getTypeByName(Context(), ptr->Name)) return type;

	error(SourceLocation::UNKNOWN, "no such type: %s", ptr->Name.c_str());
}

llvm::Value *tpp::Builder::CreateAlloca(llvm::Type *type)
{
	auto backup_block = IRBuilder().GetInsertBlock();
	IRBuilder().SetInsertPointPastAllocas(backup_block->getParent());
	auto ptr = IRBuilder().CreateAlloca(type);
	IRBuilder().SetInsertPoint(backup_block);
	return ptr;
}

llvm::Value *tpp::Builder::GetDefault(const TypePtr &type) { error(SourceLocation::UNKNOWN, "no default: %s", type->Name.c_str()); }

tpp::ValuePtr tpp::Builder::CreateCast(const ValuePtr &value, const TypePtr &type)
{
	if (!value) error(SourceLocation::UNKNOWN, "cannot cast null value");
	if (!type) error(SourceLocation::UNKNOWN, "cannot cast value to null type");

	auto value_type = value->GetIRType();
	auto ir_type = GenIR(type);

	if (value_type == ir_type) return value;

	if (value_type->isPointerTy())
	{
		if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreatePtrToInt(value->Get(), ir_type));
	}
	else if (value_type->isIntegerTy())
	{
		if (ir_type->isPointerTy()) return RValue::Create(*this, type, IRBuilder().CreateIntToPtr(value->Get(), ir_type));
		if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateIntCast(value->Get(), ir_type, true));
		if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateSIToFP(value->Get(), ir_type));
	}
	else if (value_type->isFloatingPointTy())
	{
		if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateFPToSI(value->Get(), ir_type));
		if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFPCast(value->Get(), ir_type));
	}

	error(SourceLocation::UNKNOWN, "no such cast: %s -> %s", TypeToString(value_type), TypeToString(ir_type));
}

void tpp::Builder::Push() { m_Stack.push_back(m_Variables); }

void tpp::Builder::Pop()
{
	m_Variables = m_Stack.back();
	m_Stack.pop_back();
}

tpp::TypePtr tpp::Builder::GetHigherOrder(const TypePtr &a, const TypePtr &b)
{
	if (a == b) return a;

	auto ir_a = GenIR(a);
	auto ir_b = GenIR(b);

	if (ir_a == ir_b) return a;

	if (ir_a->isFloatingPointTy())
	{
		if (ir_b->isFloatingPointTy())
		{
			if (ir_a->getPrimitiveSizeInBits() > ir_b->getPrimitiveSizeInBits()) return a;
			return b;
		}
		if (ir_b->isIntOrPtrTy()) return a;
	}

	if (ir_a->isIntegerTy())
	{
		if (ir_b->isFloatingPointTy()) return b;
		if (ir_b->isPointerTy()) return b;
		if (ir_b->isIntegerTy())
		{
			if (ir_a->getIntegerBitWidth() > ir_b->getIntegerBitWidth()) return a;
			return b;
		}
	}

	if (ir_a->isPointerTy())
	{
		if (ir_b->isFloatingPointTy()) return b;
		if (ir_b->isIntegerTy()) return a;
	}

	error(SourceLocation::UNKNOWN, "cannot determine higher order: %s <-> %s", a->Name.c_str(), b->Name.c_str());
}

std::pair<tpp::ValuePtr, tpp::ValuePtr> tpp::Builder::CreateHigherOrderCast(const ValuePtr &a, const ValuePtr &b)
{
	auto type = GetHigherOrder(a->GetType(), b->GetType());

	if (type == a->GetType()) return { a, CreateCast(b, type) };
	if (type == b->GetType()) return { CreateCast(a, type), b };
	return { CreateCast(a, type), CreateCast(b, type) };
}

tpp::ValuePtr tpp::Builder::CreateAssign(const ValuePtr &dest, const ValuePtr &src)
{
	auto lvalue = std::dynamic_pointer_cast<LValue>(dest);
	if (!lvalue) error(SourceLocation::UNKNOWN, "cannot assign to rvalue");

	auto src_value = CreateCast(src, dest->GetType());
	lvalue->Store(src_value);
	return lvalue;
}

tpp::ValuePtr tpp::Builder::CreateLT(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpSLT(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpOLT(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateGT(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpSGT(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpOGT(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateLE(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpSLE(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpOLE(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateGE(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpSGE(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpOGE(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateEQ(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpEQ(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpOEQ(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateNE(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateICmpNE(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, Type::Get("i1"), IRBuilder().CreateFCmpONE(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateAdd(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateAdd(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFAdd(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateSub(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateSub(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFSub(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateMul(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateMul(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFMul(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateDiv(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateSDiv(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFDiv(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::CreateRem(const ValuePtr &lhs, const ValuePtr &rhs)
{
	auto type = lhs->GetType();
	auto ir_type = lhs->GetIRType();

	if (ir_type->isIntegerTy()) return RValue::Create(*this, type, IRBuilder().CreateSRem(lhs->Get(), rhs->Get()));
	if (ir_type->isFloatingPointTy()) return RValue::Create(*this, type, IRBuilder().CreateFRem(lhs->Get(), rhs->Get()));

	return {};
}

tpp::ValuePtr tpp::Builder::GenIR(const DefStructExpression &e)
{
	auto struct_type = llvm::StructType::getTypeByName(Context(), e.MName.String());

	if (e.Fields.empty()) // Opaque Type
	{
		if (struct_type) return nullptr;

		struct_type = llvm::StructType::create(Context(), e.MName.String());
		m_Types[struct_type] = e;
		return nullptr;
	}

	if (struct_type)
	{
		if (!struct_type->isOpaque()) error(e.Location, "non-opaque type cannot be redefined");

		std::vector<llvm::Type *> elements(e.Fields.size());
		for (size_t i = 0; i < elements.size(); ++i) elements[i] = GenIR(e.Fields[i].MType);

		struct_type->setBody(elements);

		m_Types[struct_type] = e;
		return nullptr;
	}

	std::vector<llvm::Type *> elements(e.Fields.size());
	for (size_t i = 0; i < elements.size(); ++i) elements[i] = GenIR(e.Fields[i].MType);
	struct_type = llvm::StructType::create(Context(), elements, e.MName.String());

	m_Types[struct_type] = e;
	return nullptr;
}

tpp::ValuePtr tpp::Builder::GenIR(const DefFunctionExpression &e)
{
	auto result_type = e.Result ? GenIR(e.Result) : IRBuilder().getVoidTy();

	std::vector<TypePtr> arg_types(e.Args.size());
	for (size_t i = 0; i < arg_types.size(); ++i) arg_types[i] = e.Args[i].Type;

	auto function_type = Type::GetFunction(e.Result, arg_types, e.IsVarArg);

	auto ir_function_type = llvm::cast<llvm::FunctionType>(GenIR(function_type));
	auto callee = Module().getOrInsertFunction(e.MName.String(), ir_function_type);
	auto function = llvm::cast<llvm::Function>(callee.getCallee());

	m_FunctionInfos[function] = { function, e.Result, arg_types, e.IsVarArg };

	if (!function) error(e.Location, "failed to create function");

	if (!e.Body) return nullptr;
	if (!function->empty()) error(e.Location, "function cannot be redefined");

	auto backup_block = IRBuilder().GetInsertBlock();

	auto entry_block = llvm::BasicBlock::Create(Context(), "entry", function);
	IRBuilder().SetInsertPoint(entry_block);

	Push();
	{
		size_t i = 0;
		for (auto &arg : function->args())
		{
			auto name = e.Args[i].Name;
			arg.setName(name);
			m_Variables[name] = LValue::Alloca(*this, e.Args[i].Type, &arg);
			++i;
		}
	}

	auto result = GenIR(e.Body);
	if (result_type == IRBuilder().getVoidTy()) { IRBuilder().CreateRetVoid(); }
	else
	{
		if (result->GetIRType() != result_type) result = CreateCast(result, e.Result);
		IRBuilder().CreateRet(result->Get());
	}

	if (llvm::verifyFunction(*function, &llvm::errs()))
	{
		function->print(llvm::errs());
		error(e.Location, "failed to verify function");
	}

	Pop();
	IRBuilder().SetInsertPoint(backup_block);
	return RValue::Create(*this, function_type, function);
}

tpp::ValuePtr tpp::Builder::GenIR(const DefVariableExpression &e)
{
	auto init = e.Init ? GenIR(e.Init) : nullptr;
	auto type = e.Type ? e.Type : init->GetType();

	ValuePtr value;
	if (IsGlobal())
	{
		auto ptr = Module().getOrInsertGlobal(e.MName.String(), GenIR(type));
		auto lvalue = LValue::Create(*this, type, ptr);
		if (init) lvalue->Store(init);
		value = lvalue;
	}
	else { value = LValue::Alloca(*this, type, init); }
	return m_Variables[e.MName.String()] = value;
}

tpp::ValuePtr tpp::Builder::GenIR(const ForExpression &e)
{
	Push();

	auto from = GenIR(e.From);
	auto to = GenIR(e.To);
	ValuePtr step;

	auto counter_type = GetHigherOrder(from->GetType(), to->GetType());
	if (e.Step)
	{
		step = GenIR(e.Step);
		counter_type = GetHigherOrder(counter_type, step->GetType());
	}
	else { step = RValue::Create(*this, counter_type, from->Get()); }

	auto counter = LValue::Alloca(*this, counter_type, from);
	if (!e.Id.empty()) m_Variables[e.Id] = counter;

	auto function = IRBuilder().GetInsertBlock()->getParent();
	auto condition_block = llvm::BasicBlock::Create(Context(), "condition", function);
	auto loop_block = llvm::BasicBlock::Create(Context(), "loop", function);
	auto end_block = llvm::BasicBlock::Create(Context(), "end", function);

	IRBuilder().CreateBr(condition_block);
	IRBuilder().SetInsertPoint(condition_block);

	auto [lhs, rhs] = CreateHigherOrderCast(counter, to);
	auto condition = CreateLT(lhs, rhs);

	IRBuilder().CreateCondBr(condition->Get(), loop_block, end_block);
	IRBuilder().SetInsertPoint(loop_block);

	auto result = GenIR(e.Body);

	IRBuilder().CreateBr(end_block);
	IRBuilder().SetInsertPoint(end_block);

	Pop();
	return result;
}

tpp::ValuePtr tpp::Builder::GenIR(const WhileExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const IfExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const GroupExpression &e)
{
	Push();
	ValuePtr result;
	for (const auto &ptr : e.Body) { result = GenIR(ptr); }
	Pop();
	return result;
}

tpp::ValuePtr tpp::Builder::GenIR(const BinaryExpression &e)
{
	auto lhs = GenIR(e.Lhs);
	auto rhs = GenIR(e.Rhs);

	if (e.Operator == "=") { return CreateAssign(lhs, rhs); }
	if (e.Operator == "===") { error(e.Location, "TODO"); }

	auto [left, right] = CreateHigherOrderCast(lhs, rhs);

	ValuePtr result;
	if (e.Operator == "<" && (result = CreateLT(left, right))) return result;
	if (e.Operator == ">" && (result = CreateGT(left, right))) return result;
	if (e.Operator == "<=" && (result = CreateLE(left, right))) return result;
	if (e.Operator == ">=" && (result = CreateGE(left, right))) return result;
	if (e.Operator == "==" && (result = CreateEQ(left, right))) return result;
	if (e.Operator == "!=" && (result = CreateNE(left, right))) return result;

	auto pos = e.Operator.find("=");
	bool assign = pos != std::string::npos;
	auto op = assign ? e.Operator.substr(0, pos) : e.Operator;

	if (op == "+") { result = CreateAdd(left, right); }
	else if (op == "-") { result = CreateSub(left, right); }
	else if (op == "*") { result = CreateMul(left, right); }
	else if (op == "/") { result = CreateDiv(left, right); }
	else if (op == "%") { result = CreateRem(left, right); }

	if (!result) error(e.Location, "no such operation: %s %s %s", lhs->GetType()->Name.c_str(), e.Operator.c_str(), rhs->GetType()->Name.c_str());

	if (assign) { error(e.Location, "TODO"); }

	return result;
}

tpp::ValuePtr tpp::Builder::GenIR(const CallExpression &e)
{
	auto function = Module().getFunction(e.Callee.String());
	const auto &info = m_FunctionInfos[function];

	std::vector<llvm::Value *> args(e.Args.size());
	for (size_t i = 0; i < args.size(); ++i) args[i] = GenIR(e.Args[i])->Get();

	return RValue::Create(*this, info.Result, IRBuilder().CreateCall(llvm::FunctionCallee(function), args));
}

tpp::ValuePtr tpp::Builder::GenIR(const IndexExpression &e)
{
	auto array = GenIR(e.Array);
	auto index = CreateCast(GenIR(e.Index), Type::Get("i64"));

	auto array_type = std::dynamic_pointer_cast<ArrayType>(array->GetType());
	if (!array_type) error(e.Location, "cannot index into non-array type: %s", array->GetType()->Name.c_str());
	auto element_type = array_type->Base;

	auto ptr = IRBuilder().CreateGEP(GenIR(element_type), array->Get(), { IRBuilder().getInt64(0), index->Get() });
	return LValue::Create(*this, element_type, ptr);
}

tpp::ValuePtr tpp::Builder::GenIR(const MemberExpression &e)
{
	auto object = GenIR(e.Object);

	if (auto t = std::dynamic_pointer_cast<ArrayType>(object->GetType()))
	{
		if (e.Member == "size") error(e.Location, "TODO");
	}

	error(e.Location, "no such member: %s.%s", object->GetType()->Name.c_str(), e.Member.c_str());
}

tpp::ValuePtr tpp::Builder::GenIR(const IDExpression &e) { return m_Variables[e.MName.String()]; }

tpp::ValuePtr tpp::Builder::GenIR(const NumberExpression &e)
{
	auto value = llvm::ConstantFP::get(GenIR(e.GetType()), e.Value);
	return RValue::Create(*this, e.GetType(), value);
}

tpp::ValuePtr tpp::Builder::GenIR(const CharExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const StringExpression &e)
{
	auto ptr = IRBuilder().CreateGlobalStringPtr(e.Value);
	return RValue::Create(*this, e.GetType(), ptr);
}

tpp::ValuePtr tpp::Builder::GenIR(const VarArgsExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const UnaryExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const ObjectExpression &e) { error(e.Location, "TODO"); }

tpp::ValuePtr tpp::Builder::GenIR(const ArrayExpression &e) { error(e.Location, "TODO"); }

const char *tpp::Builder::TypeToString(llvm::Type *type)
{
	switch (type->getTypeID())
	{
	case llvm::Type::HalfTyID: return "Half";
	case llvm::Type::BFloatTyID: return "BFloat";
	case llvm::Type::FloatTyID: return "Float";
	case llvm::Type::DoubleTyID: return "Double";
	case llvm::Type::X86_FP80TyID: return "X86_FP80";
	case llvm::Type::FP128TyID: return "FP128";
	case llvm::Type::PPC_FP128TyID: return "PPC_FP128";
	case llvm::Type::VoidTyID: return "Void";
	case llvm::Type::LabelTyID: return "Label";
	case llvm::Type::MetadataTyID: return "Metadata";
	case llvm::Type::X86_MMXTyID: return "X86_MMX";
	case llvm::Type::X86_AMXTyID: return "X86_AMX";
	case llvm::Type::TokenTyID: return "Token";
	case llvm::Type::IntegerTyID: return "Integer";
	case llvm::Type::FunctionTyID: return "Function";
	case llvm::Type::PointerTyID: return "Pointer";
	case llvm::Type::StructTyID: return "Struct";
	case llvm::Type::ArrayTyID: return "Array";
	case llvm::Type::FixedVectorTyID: return "FixedVector";
	case llvm::Type::ScalableVectorTyID: return "ScalableVector";
	case llvm::Type::TypedPointerTyID: return "TypedPointer";
	case llvm::Type::TargetExtTyID: return "TargetExt";
	default: error(SourceLocation::UNKNOWN, "missing switch case");
	}
}

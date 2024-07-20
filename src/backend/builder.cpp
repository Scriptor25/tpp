#include "TPP/Frontend/SourceLocation.hpp"
#include <TPP/Backend/Builder.hpp>
#include <TPP/Frontend/Expression.hpp>
#include <TPP/Frontend/Frontend.hpp>
#include <TPP/Frontend/Type.hpp>
#include <cstddef>
#include <llvm/IR/DerivedTypes.h>
#include <vector>

tpp::Builder::Builder()
{
	m_Context = std::make_unique<llvm::LLVMContext>();
	m_Module = std::make_unique<llvm::Module>("module", *m_Context);
	m_Builder = std::make_unique<llvm::IRBuilder<>>(*m_Context);
}

llvm::LLVMContext &tpp::Builder::Context() const { return *m_Context; }

llvm::Module &tpp::Builder::Module() const { return *m_Module; }

llvm::IRBuilder<> &tpp::Builder::IRBuilder() const { return *m_Builder; }

llvm::Value *tpp::Builder::GenIR(const ExprPtr &ptr)
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

llvm::Type *tpp::Builder::GenIR(const TypePtr &ptr) { error(SourceLocation::UNKNOWN, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const DefStructExpression &e)
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
		if (!struct_type->isOpaque()) error(e.Location, "non-opaque struct type cannot be redefined");

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

llvm::Value *tpp::Builder::GenIR(const DefFunctionExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const DefVariableExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const ForExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const WhileExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const IfExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const GroupExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const BinaryExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const CallExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const IndexExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const MemberExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const IDExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const NumberExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const CharExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const StringExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const VarArgsExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const UnaryExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const ObjectExpression &e) { error(e.Location, "TODO"); }

llvm::Value *tpp::Builder::GenIR(const ArrayExpression &e) { error(e.Location, "TODO"); }

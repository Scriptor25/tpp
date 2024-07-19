#include <TPP/Backend/Builder.hpp>

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

#ifndef _ZT_CFG_H_
#define _ZT_CFG_H_
#include <cstdint>
#include <list>
#include <vector>
#include <map>
#include "Type.h"
#include <sstream>
namespace ztCompiler {
	class basic_block;
	class cfg;
	class value;
	class phi_node;
	class instruction;

	/*!
	 * \class basic_block
	 *
	 * \brief basic block是一个简化版的instructions，它将按照顺序进行执行。
	 *		  basic block一个是value，因为它由branchs和switch tables等instructions
	 *		  进行referencing。
	 *		  
	 *		  basic block的类型是type::label，因为基本块表示一个branch可以jump的label。
	 *		  一个well-formed基本块由一系列non-terminating instructions组成，随后跟单个
	 *		  terminator_instruction指令。terminator_instruction可能不会出现在basic block
	 *		  的中间。
	 *
	 */
	class basic_block : public value {
	public:
		basic_block(const basic_block&) = delete;
		basic_block& operator=(const basic_block&) = delete;
		basic_block(std::string name)
			:bb_name_(name) {}
		~basic_block() {
			while (!instructions_.empty()) {
				instructions_.front()->get_parent()
			}
		}

		using predecessor_iterator = std::vector<basic_block*>::iterator;
		using successor_iterator   = std::vector<basic_block*>::iterator;

		void add_predecessor(basic_block* bb) { predecessors_.push_back(bb); }
		void add_successor(basic_block* bb)   { return successors_.push_back(bb); }
// 		void remove_predecessor(basic_block* bb);
// 		void remove_successor(basic_block* bb);
		bool contain(instruction* instr) {
			for (auto* iter : instructions_) {
				if (iter == instr) {
					return true;
				}
			}
			return false;
		}
		uint32_t             num_of_predecessor() const { return predecessors_.size(); }
		predecessor_iterator predecessor_begin()        { return predecessors_.begin(); }
		predecessor_iterator predecessor_end()          { return predecessors_.end(); }
		successor_iterator   successor_begin()          { return successors_.begin(); }
		successor_iterator   successor_end()            { return successors_.end(); }
		size_t				 get_predecessor_nums()     { return predecessors_.size(); }
		size_t				 get_successor_nums()		{ return successors_.size(); }
		
		using instruction_iterator = std::list<instruction*>::iterator;
		using instruction_riterator = std::list<instruction*>::reverse_iterator;
		const std::string& get_block_name() const       { return bb_name_; }
		void               set_block_id(uint32_t id)    { block_id_ = id; }
		uint32_t           get_block_id() const         { return block_id_; }
		uint32_t           get_instruction_begin() const { return instructions_.begin(); }
		uint32_t           get_instruction_end() const { return instructions_.end(); }

		using phi_node_iterator = std::list<phi_node*>::iterator;
		phi_node_iterator phi_node_begin() { return phi_nodes_.begin(); }
		phi_node_iterator phi_node_end()   { return phi_nodes_.end(); }
		uint32_t          phi_nums()       { return phi_nodes_.size(); }
	private:
		std::string	                bb_name_;
		std::vector<basic_block*>   predecessors_;
		std::vector<basic_block*>   successors_;
		std::list<phi_node*>        phi_nodes_;
		std::list<instruction*>     instructions_;
		uint32_t                    block_id_;
		//uint32_t loop_index_;
	};

	class cfg {
		friend class basic_block;
	public:
		using basic_block_list = std::list<basic_block*>;
		using basic_block_set = std::set<basic_block*>;

		using string_to_value = std::map<std::string, value*>;
		using block_to_value = std::map<basic_block*, value*>;
		
		using incomplete_Phis = std::map<basic_block*, string_to_value>;
		using current_Def = std::map<std::string, block_to_value>;

		cfg();
		~cfg();

		basic_block* create_basic_block(const std::string& bb_name) {
			block_id_++;
			basic_block* r = new basic_block(bb_name);
			blocks_.push_back(r);
			return r;
		}
		
		
		basic_block* get_entry_block() { return bb_start_; }
		
// 		sealBlock(block) :
// 			for variable in incompletePhis[block] :
// 				addPhiOperands(variable, incompletePhis[block][variable])
// 				sealedBlocks.add(block)
//************************************************
//		从上述伪代码中可以得知，incompletePhis是一个二维数组，包含有block
//		和variable。而根据paper可以得知，其中：
//		block    =========> basic block
//		variable =========> for each basic block we keep a mapping from each source variable to
//							its current defining expression.

		//************************************
		// \method name:    seal_block
		// \parameter:		basic_block * block
		//
		// \brief:			用于处理incomplete CFGs
		//************************************
		void seal_block(basic_block* block) {
			static_assert(block != nullptr, "basic block cannot be null");
			if (!sealed_blocks_.empty()) {
				return;
			}

			auto& phis = incomplete_phis_[block];
			for (auto& var : phis) {
				add_phi_operands(var->first, var->second);
			}
			incomplete_phis_.erase(phis);
			sealed_blocks_.insert(block);

		}
		

// 		writeVariable(variable, block, value) :
// 			currentDef[variable][block] ← value
// 		
// 		readVariable(variable, block) :
// 			if currentDef[variable] contains block :
// 				# local value numbering
// 				return currentDef[variable][block]
// 			# global value numbering
// 			return readVariableRecursive(variable, block)
//*******************************************************
//		由上述伪代码，可以得知，currentDef是一个二维数组，包含variable和block


		//************************************
		// \method name:    write_variable
		//
		// \brief:			
		//************************************
		void write_variable(std::string varianle_name, basic_block* block, value* val) {
			static_assert(block != nullptr&&val != nullptr, "basic block or value cannot be null");
			current_def_[varianle_name][block] = val;
		}

		value* read_variable(std::string variable_name, basic_block* block) {
			static_assert(block != nullptr);
			std::map<basic_block*, value> cur_def = current_def_[variable_name];
			if (cur_def.find(block) != cur_def.end()) {
				return cur_def[block];
			}
			return read_variable_recursive(variable_name, block);
		}

		std::string generate_phi_name(const std::string& phi_name) {
			std::stringstream stream;
			stream << phi_name << '(' << phi_numbers_[phi_name]++ << ')';
			//
		}
	protected:
		std::map<std::string, size_t> phi_numbers_;
		incomplete_Phis incomplete_phis_;
		current_Def current_def_;
		unsigned block_id_;
		basic_block* bb_start_;
		basic_block* bb_end_;
		basic_block_list blocks_;
		basic_block_set sealed_blocks_;


		value* read_variable_recursive(std::string variable_name, basic_block* block) {
			static_assert(block != nullptr, "block is cannot be null");
			value* val = nullptr;
			if (sealed_blocks_.find(block) != sealed_blocks_.end()) {
				// # Incomplete CFG
				val = create_instruction_before<phi_node>(block, generate_phi_name(variable_name));
			}else if (block.num_of_predecessor() == 1) {
				// # Optimize the common case of one predecessor: No phi needed
				val = read_variable(variable_name, block->predecessors_[0]);
			}else {
				// # Break potential cycles with operandless phi
				val = create_instruction_before<phi_node>(block, generate_phi_name(variable_name));
				write_variable(variable_name, block, val);
				val = add_phi_operands(variable_name, val);
			}
			write_variable(variable_name, block, val);
			return val;
		}

		value* add_phi_operands(std::string variable_name, phi_node* phi) {
			// # Determine operands from predecessors
			//获取当前phi节点的predecessor
			basic_block* pred = phi->get_parent();
			for (auto iter = pred->predecessor_begin(); iter != pred->predecessor_end(); iter++) {
				phi->append_operand(read_variable(variable_name, iter));
			}
			return try_remove_trivial_phi(phi);
		}
		
		value* try_remove_trivial_phi(phi_node* phi) {
			value* same = nullptr;
			for (auto iter = phi->operand_bagin(); iter != phi->operand_end(); iter++) {
				value* op = iter->get_value();
				if (op == same || op == phi) 
					//  # Unique value or self−reference
					continue;
				if (same != nullptr) {
					// # The phi merges at least two values: not trivial
					return phi;
				}
				same = op;
			}
			if (same = nullptr) {
				//  # The phi is unreachable or in the start block
				same = create_instruction_before<instruction::instruction_type::undef_type>();
			}
			// 先update整个variable definition set，即将当前phi的所有predecessors存入其中
			basic_block*  phi_predecessors= phi->get_parent();
			write_variable(variable_name, phi_predecessors, same);
			// # Remember all users except the phi itself
			std::set<phi_node*> users;
			for (auto iter = phi->use_begin(); iter != phi->use_end(); iter++) {
				instruction* _use = dynamic_cast<instruction*>((*iter)->get_user());
				if (_use != phi && _use->is_phi()) {
					users.insert(dynamic_cast<phi*>(_use));
				}
			}
			//  # Reroute all uses of phi to same and remove phi
			phi->replace_by(same);

			// # Try to recursively remove all phi users, which might have become trivial
			for (auto* u : users) {
				if (u->is_phi())
					try_remove_trivial_phi(u);
			}
			return same;
		}

		template<typename T,typename... Args>
		T* create_instruction(Args... args) {
			return new T(args...);
		}

		template<typename T,typename... Args>
		T* create_instruction_before(basic_block* block, Args... args) {
			static_assert(block != nullptr, "basic block cannot be null");

		}
	};

}

#endif
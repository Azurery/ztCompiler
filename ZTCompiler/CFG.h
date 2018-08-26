#ifndef _ZT_CFG_H_
#define _ZT_CFG_H_
#include <cstdint>
#include <list>
#include <vector>
#include <unordered_map>
#include "Type.h"

namespace ztCompiler {
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
		cfg();
		~cfg();

		basic_block* create_basic_block(const std::string& bb_name) {
			block_id_++;
			basic_block* r = new basic_block(bb_name);
			blocks_.push_back(r);
			return r;
		}
		
		basic_block* get_entry_block() { return bb_start_; }
		void seal_block(basic_block* block) {
			static_assert(block != nullptr, "basic block is null");
			if (!sealed_blocks_.empty()) {
				return;
			}


		}
		

	protected:
		using basic_block_list = std::list<basic_block*>;
		using basic_block_set = std::set<basic_block*>;
		unsigned block_id_;
		basic_block* bb_start_;
		basic_block* bb_end_;
		basic_block_list blocks_;
		basic_block_set sealed_blocks_;

		value* read_variable_recursive(std::string varible, basic_block* block) {
			static_assert(block != nullptr, "block is null");
			value* val = nullptr;
			if()
		}

		value* add_phi_operands(std::string varible, phi_node* phi) {

		}

		value* try_remove_trivial_phi(phi_node* phi) {

		}
	};

}

#endif
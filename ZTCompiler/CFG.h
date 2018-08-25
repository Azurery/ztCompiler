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

	class basic_block : public value {
	public:
		using predecessor_iterator = std::vector<basic_block*>::iterator;
		using successor_iterator = std::vector<basic_block*>::iterator;

		void add_predecessor(basic_block* bb);
		void add_successor(basic_block* bb);
		void add_predecessor(basic_block* bb);
		void add_successor(basic_block* bb);
		uint32_t num_of_predecessor() const { return predecessors_.size(); }
		predecessor_iterator predecessor_begin() { return predecessors_.begin(); }
		predecessor_iterator predecessor_end() { return predecessors_.end(); }
		successor_iterator successor_begin() { return successors_.begin(); }
		successor_iterator successor_end() { return successors_.end(); }
		
		const std::string& get_block_name() const { return bb_name_; }

		void set_block_id(uint32_t id) { block_id_ = id; }
		uint32_t get_block_id() const { return block_id_; }
		uint32_t get_instruction_begin() const { return instuction_start_; }
		uint32_t get_instruction_end() const { return instruction_end_; }

		using phi_node_iterator = std::list<phi_node*>::iterator;
		phi_node_iterator phi_node_begin() { return phi_nodes_.begin(); }
		phi_node_iterator phi_node_end() { return phi_nodes_.end(); }
		uint32_t phi_nums() { return phi_nodes_.size(); }
	private:
		std::string bb_name_;
		std::vector<basic_block*> predecessors_;
		std::vector<basic_block*> successors_;
		std::list<phi_node*> phi_nodes_;
		std::list<instruction*> instructions_;
		uint32_t block_id_;
		//uint32_t loop_index_;
		
		///instruction interval [start,end)
		uint32_t instrcution_start_;
		uint32_t instruction_end_;
	};


}

#endif
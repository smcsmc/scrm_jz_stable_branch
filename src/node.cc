#include "node.h"
//#include<iostream>
#include<sstream>
#include<fstream>
#include<iomanip>
#include<iostream>


Node::Node() { init(); };
Node::Node(double height) { init(height, true); };
Node::Node(double height, bool local) { init(height, local); };
Node::Node(double height, bool local, size_t last_update) { 
  init(height, local, last_update); };
Node::Node(double height, bool local, size_t last_update, size_t samples_below)
  { init(height, local, last_update, samples_below); };
Node::Node(double height, bool local, size_t last_update, size_t samples_below, double length_below)
  { init(height, local, last_update, samples_below, length_below); };
Node::Node(double height, bool local, size_t last_update, size_t samples_below, double length_below, size_t label)
  { init(height, local, last_update, samples_below, length_below, label); };
  
//Node::~Node() {delete[] marginal_likelihood;}
Node::~Node() {};

void Node::init(double height, bool local, size_t last_update,
                size_t samples_below, double length_below, size_t label) {
  this->set_height(height);
  this->set_local(local);
  this->set_parent(NULL);
  this->set_second_child(NULL);
  this->set_first_child(NULL);
  this->set_last_update(last_update);
  this->set_samples_below(samples_below);
  this->set_length_below(length_below);
  this->set_previous(NULL);
  this->set_next(NULL);
  this->set_label(label);
  this->mutation_state=false;
  this->marginal_likelihood[0]=0;
  this->marginal_likelihood[1]=0;
  this->set_mut_num(0);
}

Node* Node::parent() const {
  //if (this->parent_ == NULL) 
  //  throw std::logic_error("Trying to access parent of ultimate root"); 
  return this->parent_; 
}

void Node::change_child(Node* from, Node* to) {
  if ( this->first_child() == from )
    this->set_first_child(to);
  else if ( this->second_child() == from )
    this->set_second_child(to);
  else throw std::invalid_argument("Can't find child node to replace");
}

void Node::remove_child(Node* child) {
  if ( this->first_child() == child ) {
    this->set_first_child(this->second_child());
    this->set_second_child(NULL);
    return;
  } 

  if ( this->second_child() == child ) {
    this->set_second_child(NULL);
    return;
  }

  throw std::invalid_argument("Can't find child to delete");
}

bool Node::in_sample() const {
  return ( this->label() != 0 ); 
}






/**
 * Extract the string to represent the subtree that is descendant from node.

*/
std::string writeTree(Node * node, int npop, double bl_above_parent){
	if (!node->local()){
		//node->first_child()->tree_topo_bl.clear();
		//node->second_child()->tree_topo_bl.clear();
		//node->tree_topo_bl="("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+");";
		//return node->tree_topo_bl;
		return "("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+");";
		}
	else{
	
		if(node->first_child() == NULL && ((node->label())>0)){ // real tip node
		//if(node->label()>0){
			std::ostringstream label_strm;
			label_strm<<node->label();
			std::ostringstream bl_strm;
			bl_strm<< (node->parent_height() - node->height() + bl_above_parent)/4/npop;
			//node->tree_topo_bl = label_strm.str()+":"+bl_strm.str();
			//return node->tree_topo_bl;
			return label_strm.str()+":"+bl_strm.str();
		}
		else if (node->is_root()){ // check if this is the root
				//node->first_child()->tree_topo_bl.clear();
				//node->second_child()->tree_topo_bl.clear();
				//node->tree_topo_bl="("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+");";
				//return node->tree_topo_bl;
				return "("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+");";
				}
		else{ // this is an interior node, but need to check if it is real, i.e. any of its children is a local
			if (node->first_child()->local() && node->second_child()->local()){ // both children are local
		//node->first_child()->tree_topo_bl.clear();
		//node->second_child()->tree_topo_bl.clear();
				std::ostringstream bl_strm;
				bl_strm<< (node->parent_height() - node->height() + bl_above_parent)/4/npop;
				//node->tree_topo_bl="("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+"):"+bl_strm.str();;
				//return node->tree_topo_bl;	
				return "("+ writeTree(node->first_child(),npop,0) +","+ writeTree(node->second_child(),npop,0)+"):"+bl_strm.str();;
			}
			else if(node->first_child()->local() && !node->second_child()->local()) { // first child is local, 
		//node->first_child()->tree_topo_bl.clear();
		//node->second_child()->tree_topo_bl.clear();
				double local_bl_above_parent = bl_above_parent + node->parent_height() - node->height();
				//node->tree_topo_bl=writeTree(node->first_child(),npop, local_bl_above_parent);
				//return node->tree_topo_bl;	
				return writeTree(node->first_child(),npop, local_bl_above_parent);
			}
			else {// !node->first_child()->local() && node->second_child()->local() // second child is local, 
			//node->first_child()->tree_topo_bl.clear();
		//node->second_child()->tree_topo_bl.clear();
				double local_bl_above_parent = bl_above_parent + node->parent_height() - node->height();
				//node->tree_topo_bl=writeTree(node->second_child(),npop, local_bl_above_parent);
				//return node->tree_topo_bl;
				return writeTree(node->second_child(),npop, local_bl_above_parent);
			}
			
		}
	}
}

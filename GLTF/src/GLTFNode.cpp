#include "GLTFNode.h"

GLTF::Node::Node() {
	this->children = new std::vector<GLTF::Node*>();
}
#include "GLTFAsset.h"

GLTF::Scene* GLTF::Asset::getDefaultScene() {
	GLTF::Scene* scene;
	if (this->scene < 0) {
		scene = new GLTF::Scene();
		this->scenes.push_back(scene);
		this->scene = 0;
	}
	else {
		scene = this->scenes[this->scene];
	}
	return scene;
}

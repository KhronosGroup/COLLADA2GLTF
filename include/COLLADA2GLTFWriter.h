#pragma once

#include <map>
#include <vector>

#include "COLLADABU.h"
#include "COLLADAFW.h"
#include "GLTFAsset.h"

namespace COLLADA2GLTF {
	class Writer : public COLLADAFW::IWriter {
	private:
		GLTF::Asset* _asset;
		GLTF::BufferView* _indicesBufferView = NULL;
		GLTF::BufferView* _attributesBufferView = NULL;
		std::map<COLLADAFW::UniqueId, GLTF::Mesh*> _meshInstances;

		bool writeNodeToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::Node* node);
		bool writeNodesToGroup(std::vector<GLTF::Node*>* group, const COLLADAFW::NodePointerArray& nodes);

		bool writeMesh(const COLLADAFW::Mesh* mesh);
	public:
		Writer(GLTF::Asset* asset);

		/** Deletes the entire scene.
			 @param errorMessage A message containing informations about the error that occurred.
			 */
		void cancel(const std::string& errorMessage);

		/** Prepare to receive data.*/
		void start();

		/** Remove all objects that don't have an object. Deletes unused visual scenes.*/
		void finish();

		/** When this method is called, the writer must write the global document asset->
		 @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeGlobalAsset(const COLLADAFW::FileInfo* asset);

		/** Writes the entire visual scene.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeVisualScene(const COLLADAFW::VisualScene* visualScene);

		/** Writes the scene.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeScene(const COLLADAFW::Scene* scene);

		/** Handles all nodes in the library nodes.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeLibraryNodes(const COLLADAFW::LibraryNodes* libraryNodes);

		/** Writes the geometry.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeGeometry(const COLLADAFW::Geometry* geometry);

		/** Writes the material.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeMaterial(const COLLADAFW::Material* material);

		/** Writes the effect.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeEffect(const COLLADAFW::Effect* effect);

		/** Writes the camera.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeCamera(const COLLADAFW::Camera* camera);

		/** Writes the image.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeImage(const COLLADAFW::Image* image);

		/** Writes the light.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeLight(const COLLADAFW::Light* light);

		/** Writes the animation.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeAnimation(const COLLADAFW::Animation* animation);

		/** Writes the animation.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeAnimationList(const COLLADAFW::AnimationList* animationList);

		/** Writes the skin controller data.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeSkinControllerData(const COLLADAFW::SkinControllerData* skinControllerData);

		/** Writes the controller.
		 @return True on succeeded, false otherwise.*/
		virtual bool writeController(const COLLADAFW::Controller* Controller);

		/** When this method is called, the writer must write the formulas. All the formulas of the entire
		 COLLADA file are contained in @a formulas.
		 @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeFormulas(const COLLADAFW::Formulas* formulas);

		/** When this method is called, the writer must write the kinematics scene.
		 @return The writer should return true, if writing succeeded, false otherwise.*/
		virtual bool writeKinematicsScene(const COLLADAFW::KinematicsScene* kinematicsScene);
	};
}

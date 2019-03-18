#include "GLTFNode.h"

#include <cmath>

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Node::TransformMatrix::TransformMatrix() {
	this->type = GLTF::Node::Transform::MATRIX;
	this->matrix[0] = 1;
	this->matrix[1] = 0;
	this->matrix[2] = 0;
	this->matrix[3] = 0;
	this->matrix[4] = 0;
	this->matrix[5] = 1;
	this->matrix[6] = 0;
	this->matrix[7] = 0;
	this->matrix[8] = 0;
	this->matrix[9] = 0;
	this->matrix[10] = 1;
	this->matrix[11] = 0;
	this->matrix[12] = 0;
	this->matrix[13] = 0;
	this->matrix[14] = 0;
	this->matrix[15] = 1;
}

GLTF::Node::TransformMatrix::TransformMatrix(float a00, float a01, float a02, float a03,
	float a10, float a11, float a12, float a13,
	float a20, float a21, float a22, float a23,
	float a30, float a31, float a32, float a33) : GLTF::Node::TransformMatrix() {
	this->matrix[0] = a00;
	this->matrix[1] = a10;
	this->matrix[2] = a20;
	this->matrix[3] = a30;
	this->matrix[4] = a01;
	this->matrix[5] = a11;
	this->matrix[6] = a21;
	this->matrix[7] = a31;
	this->matrix[8] = a02;
	this->matrix[9] = a12;
	this->matrix[10] = a22;
	this->matrix[11] = a32;
	this->matrix[12] = a03;
	this->matrix[13] = a13;
	this->matrix[14] = a23;
	this->matrix[15] = a33;
}

GLTF::Node::TransformTRS::TransformTRS() {
	this->type = GLTF::Node::Transform::TRS;
}

void GLTF::Node::TransformMatrix::premultiply(GLTF::Node::TransformMatrix* transform) {
	premultiply(transform, this);
}

void GLTF::Node::TransformMatrix::premultiply(GLTF::Node::TransformMatrix* transform, GLTF::Node::TransformMatrix* destination) {
	float matrix[16];
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			float sum = 0;
			for (int k = 0; k < 4; k++) {
				sum += this->matrix[i * 4 + k] * transform->matrix[k * 4 + j];
			}
			matrix[i * 4 + j] = sum;
		}
	}
	for (int i = 0; i < 16; i++) {
		destination->matrix[i] = matrix[i];
	}
}


void GLTF::Node::TransformMatrix::scaleUniform(float scale) {
	for (int i = 0; i < 11; i++) {
		this->matrix[i] *= scale;
	}
}

bool GLTF::Node::TransformMatrix::isIdentity() {
	return matrix[0] == 1 && matrix[1] == 0 && matrix[2] == 0 && matrix[3] == 0 &&
		matrix[4] == 0 && matrix[5] == 1 && matrix[6] == 0 && matrix[7] == 0 &&
		matrix[8] == 0 && matrix[9] == 0 && matrix[10] == 1 && matrix[11] == 0 &&
		matrix[12] == 0 && matrix[13] == 0 && matrix[14] == 0 && matrix[15] == 1;
}

bool GLTF::Node::TransformTRS::isIdentityTranslation() {
	return translation[0] == 0 && 
		translation[1] == 0 && 
		translation[2] == 0;
}

bool GLTF::Node::TransformTRS::isIdentityRotation() {
	return rotation[0] == 0 &&
		rotation[1] == 0 &&
		rotation[2] == 0 &&
		rotation[3] == 1;
}

bool GLTF::Node::TransformTRS::isIdentityScale() {
	return scale[0] == 1 &&
		scale[1] == 1 &&
		scale[2] == 1;
}


GLTF::Node::TransformTRS* GLTF::Node::TransformMatrix::getTransformTRS() {
	GLTF::Node::TransformTRS* trs = new GLTF::Node::TransformTRS();
	getTransformTRS(trs);
	return trs;
}

GLTF::Node::Transform* GLTF::Node::TransformMatrix::clone()
{
    auto result = new TransformMatrix();
    memcpy(result->matrix, matrix, sizeof(float) * 16);
    return result;
}

const int rotationMatrixNext[3] = { 1, 2, 0 };
void GLTF::Node::TransformMatrix::getTransformTRS(GLTF::Node::TransformTRS* trs) {
	// get translation
	trs->translation[0] = matrix[12];
	trs->translation[1] = matrix[13];
	trs->translation[2] = matrix[14];

	// get rotation
	float root;
	float m00 = matrix[0];
	float m11 = matrix[5];
	float m22 = matrix[10];
	float trace = m00 + m11 + m22;

	float x, y, z, w;

	if (trace > 0) {
		root = sqrtf((float)(trace + 1.0));
		w = (float)(0.5 * root);
		root = (float)(0.5 / root);

		x = (matrix[6] - matrix[9]) * root;
		y = (matrix[8] - matrix[2]) * root;
		z = (matrix[1] - matrix[4]) * root;
	} else {
		int i = 0;
		if (m11 > m00) {
			i = 1;
		}
		if (m22 > m00 && m22 > m11) {
			i = 2;
		}
		int j = rotationMatrixNext[i];
		int k = rotationMatrixNext[j];

		root = sqrtf((float)(matrix[i * 4 + i] - matrix[j * 4 + j] - matrix[k * 4 + k] + 1.0));
		trs->rotation[i] = (float)(0.5 * root);
		root = (float)(0.5 / root);
		w = (matrix[k * 4 + j] - matrix[j * 4 + k]) * root;
		trs->rotation[j] = (matrix[j * 4 + i] + matrix[i * 4 + j]) * root;
		trs->rotation[k] = (matrix[k * 4 + i] + matrix[i * 4 + k]) * root;

		x = -trs->rotation[0];
		y = -trs->rotation[1];
		z = -trs->rotation[2];
	}
	trs->rotation[0] = -x;
	trs->rotation[1] = -y;
	trs->rotation[2] = -z;
	trs->rotation[3] = -w;

	// get scale
	trs->scale[0] = sqrtf(matrix[0] * matrix[0] + matrix[1] * matrix[1] + matrix[2] * matrix[2]);
	trs->scale[1] = sqrtf(matrix[4] * matrix[4] + matrix[5] * matrix[5] + matrix[6] * matrix[6]);
	trs->scale[2] = sqrtf(matrix[8] * matrix[8] + matrix[9] * matrix[9] + matrix[10] * matrix[10]);
}

GLTF::Node::TransformMatrix* GLTF::Node::TransformTRS::getTransformMatrix() {
	GLTF::Node::TransformMatrix* result = new GLTF::Node::TransformMatrix();
	float scaleX = scale[0];
	float scaleY = scale[1];
	float scaleZ = scale[2];

	float rotationX = rotation[0];
	float rotationY = rotation[1];
	float rotationZ = rotation[2];
	float rotationW = rotation[3];

	float x2 = rotationX * rotationX;
	float xy = rotationX * rotationY;
	float xz = rotationX * rotationZ;
	float xw = rotationX * rotationW;
	float y2 = rotationY * rotationY;
	float yz = rotationY * rotationZ;
	float yw = rotationY * rotationW;
	float z2 = rotationZ * rotationZ;
	float zw = rotationZ * rotationW;
	float w2 = rotationW * rotationW;

	float m00 = x2 - y2 - z2 + w2;
	float m01 = (float)(2.0 * (xy - zw));
	float m02 = (float)(2.0 * (xz + yw));
	
	float m10 = (float)(2.0 * (xy + zw));
	float m11 = -x2 + y2 - z2 + w2;
	float m12 = (float)(2.0 * (yz - xw));

	float m20 = (float)(2.0 * (xz - yw));
	float m21 = (float)(2.0 * (yz + xw));
	float m22 = -x2 - y2 + z2 + w2;

	result->matrix[0] = m00 * scaleX;
	result->matrix[1] = m10 * scaleX;
	result->matrix[2] = m20 * scaleX;
	result->matrix[3] = 0.0;
	result->matrix[4] = m01 * scaleY;
	result->matrix[5] = m11 * scaleY;
	result->matrix[6] = m21 * scaleY;
	result->matrix[7] = 0.0;
	result->matrix[8] = m02 * scaleZ;
	result->matrix[9] = m12 * scaleZ;
	result->matrix[10] = m22 * scaleZ;
	result->matrix[11] = 0.0;
	result->matrix[12] = translation[0];
	result->matrix[13] = translation[1];
	result->matrix[14] = translation[2];
	result->matrix[15] = 1.0;

	return result;
}

GLTF::Node::Transform* GLTF::Node::TransformTRS::clone()
{
    auto result = new TransformTRS();

    memcpy(result->translation, translation, sizeof(float) * 3);
    memcpy(result->rotation, rotation, sizeof(float) * 4);
    memcpy(result->scale, scale, sizeof(float) * 3);

    return result;
}

GLTF::Node::~Node() {
    delete transform;
}

std::string GLTF::Node::typeName() {
	return "node";
}

GLTF::Object* GLTF::Node::clone(GLTF::Object* clone) {
	GLTF::Node* node = dynamic_cast<GLTF::Node*>(clone);
	if (node != NULL) {
		node->camera = camera;
		for (GLTF::Node* child : children) {
			GLTF::Node* cloneChild = new GLTF::Node();
			child->clone(cloneChild);
			node->children.push_back(cloneChild);
		}
		node->skin = skin;
		node->jointName = jointName;
		node->mesh = mesh;
		node->light = light;
		node->transform = transform->clone();
		GLTF::Object::clone(clone);
	}
	return node;
}

void GLTF::Node::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	
	if (mesh != NULL) {
		if (options->version == "1.0") {
			jsonWriter->Key("meshes");
			jsonWriter->StartArray();
			jsonWriter->String(mesh->getStringId().c_str());
			jsonWriter->EndArray();
		}
		else {
			jsonWriter->Key("mesh");
			jsonWriter->Int(mesh->id);
		}
	}
	if (children.size() > 0) {
		jsonWriter->Key("children");
		jsonWriter->StartArray();
		for (GLTF::Node* child : children) {
			if (options->version == "1.0") {
				jsonWriter->String(child->getStringId().c_str());
			}
			else {
				jsonWriter->Int(child->id);
			}
		}
		jsonWriter->EndArray();
	}
	if (options->version == "1.0" && jointName != "") {
		jsonWriter->Key("jointName");
		jsonWriter->String(jointName.c_str());
	}
	if (options->materialsCommon && light != NULL) {
		jsonWriter->Key("extensions");
		jsonWriter->StartObject();
		jsonWriter->Key("KHR_materials_common");
		jsonWriter->StartObject();
		jsonWriter->Key("light");
		if (options->version == "1.0") {
			jsonWriter->String(light->getStringId().c_str());
		}
		else {
			jsonWriter->Int(light->id);
		}
		jsonWriter->EndObject();
		jsonWriter->EndObject();
	}
	if (transform != NULL) {
		if (transform->type == GLTF::Node::Transform::MATRIX) {
			GLTF::Node::TransformMatrix* transformMatrix = (GLTF::Node::TransformMatrix*)transform;
			if (!transformMatrix->isIdentity()) {
				jsonWriter->Key("matrix");
				jsonWriter->StartArray();
				for (int i = 0; i < 16; i++) {
					jsonWriter->Double(transformMatrix->matrix[i]);
				}
				jsonWriter->EndArray();
			}
		} else if (transform->type == GLTF::Node::Transform::TRS) {
			GLTF::Node::TransformTRS* transformTRS = (GLTF::Node::TransformTRS*)transform;
			if (!transformTRS->isIdentityTranslation()) {
				jsonWriter->Key("translation");
				jsonWriter->StartArray();
				for (int i = 0; i < 3; i++) {
					jsonWriter->Double(transformTRS->translation[i]);
				}
				jsonWriter->EndArray();
			}

			if (!transformTRS->isIdentityRotation()) {
				jsonWriter->Key("rotation");
				jsonWriter->StartArray();
				for (int i = 0; i < 4; i++) {
					jsonWriter->Double(transformTRS->rotation[i]);
				}
				jsonWriter->EndArray();
			}

			if (!transformTRS->isIdentityScale()) {
				jsonWriter->Key("scale");
				jsonWriter->StartArray();
				for (int i = 0; i < 3; i++) {
					jsonWriter->Double(transformTRS->scale[i]);
				}
				jsonWriter->EndArray();
			} 
		}
	}
	if (skin != NULL) {
		jsonWriter->Key("skin");
		if (options->version == "1.0") {
			jsonWriter->String(skin->getStringId().c_str());
			if (skin->skeleton != NULL) {
				jsonWriter->Key("skeletons");
				jsonWriter->StartArray();
				jsonWriter->String(skin->skeleton->getStringId().c_str());
				jsonWriter->EndArray();
			}
		}
		else {
			jsonWriter->Int(skin->id);
		}
	}
	if (camera != NULL) {
		jsonWriter->Key("camera");
		if (options->version == "1.0") {
			jsonWriter->String(camera->getStringId().c_str());
		}
		else {
			jsonWriter->Int(camera->id);
		}
	}
	GLTF::Object::writeJSON(writer, options);
}

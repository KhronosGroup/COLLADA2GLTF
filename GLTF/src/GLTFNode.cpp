#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

int GLTF::Node::INSTANCE_COUNT = 0;

GLTF::Node::Node() {
	this->id = "node_" + std::to_string(GLTF::Node::INSTANCE_COUNT);
	GLTF::Accessor::INSTANCE_COUNT++;
}

GLTF::Node::Node(std::string id) {
	this->id = id;
}

GLTF::Node::TransformMatrix::TransformMatrix() {
	this->type = GLTF::Node::Transform::MATRIX;
}

GLTF::Node::TransformTRS::TransformTRS() {
	this->type = GLTF::Node::Transform::TRS;
}

GLTF::Node::TransformTRS* GLTF::Node::TransformMatrix::getTransformTRS() {
	GLTF::Node::TransformTRS* trs = new GLTF::Node::TransformTRS();
	getTransformTRS(trs);
	return trs;
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
		root = sqrtf(trace + 1.0);
		w = 0.5 * root;
		root = 0.5 / root;

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

		root = sqrtf(matrix[i * 4 + i] - matrix[j * 4 + j] - matrix[k * 4 + k]);
		trs->rotation[i] = 0.5 * root;
		root = 0.5 / root;
		w = (matrix[k * 4 + j] - matrix[j * 4 + k]) * root;
		trs->rotation[j] = (matrix[j * 4 + i] + matrix[i * 4 + j]) * root;
		trs->rotation[k] = (matrix[k * 4 + i] + matrix[i * 4 + k]) * root;

		x = -trs->rotation[0];
		y = -trs->rotation[1];
		z = -trs->rotation[2];
	}
	trs->rotation[0] = x;
	trs->rotation[1] = y;
	trs->rotation[2] = z;
	trs->rotation[3] = w;

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
	float m01 = 2.0 * (xy - zw);
	float m02 = 2.0 * (xz + yw);
	
	float m10 = 2.0 * (xy + zw);
	float m11 = -x2 + y2 - z2 + w2;
	float m12 = 2.0 * (yz - xw);

	float m20 = 2.0 * (xz - yw);
	float m21 = 2.0 * (yz + xw);
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

void GLTF::Node::writeJSON(void* writer) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	
	jsonWriter->Key("meshes");
	jsonWriter->StartArray();
	for (GLTF::Mesh* mesh : meshes) {
		jsonWriter->String(mesh->id.c_str());
	}
	jsonWriter->EndArray();
	jsonWriter->Key("children");
	jsonWriter->StartArray();
	for (GLTF::Node* child : children) {
		jsonWriter->String(child->id.c_str());
	}
	jsonWriter->EndArray();
	if (transform != NULL) {
		if (transform->type == GLTF::Node::Transform::MATRIX) {
			GLTF::Node::TransformMatrix* transformMatrix = (GLTF::Node::TransformMatrix*)transform;
			jsonWriter->Key("matrix");
			jsonWriter->StartArray();
			for (int i = 0; i < 16; i++) {
				jsonWriter->Double(transformMatrix->matrix[i]);
			}
			jsonWriter->EndArray();
		} else if (transform->type == GLTF::Node::Transform::TRS) {
			GLTF::Node::TransformTRS* transformTRS = (GLTF::Node::TransformTRS*)transform;
			jsonWriter->Key("translation");
			jsonWriter->StartArray();
			for (int i = 0; i < 3; i++) {
				jsonWriter->Double(transformTRS->translation[i]);
			}
			jsonWriter->EndArray();

			jsonWriter->Key("rotation");
			jsonWriter->StartArray();
			for (int i = 0; i < 4; i++) {
				jsonWriter->Double(transformTRS->rotation[i]);
			}
			jsonWriter->EndArray();

			jsonWriter->Key("scale");
			jsonWriter->StartArray();
			for (int i = 0; i < 3; i++) {
				jsonWriter->Double(transformTRS->scale[i]);
			}
			jsonWriter->EndArray();
		}
	}
}
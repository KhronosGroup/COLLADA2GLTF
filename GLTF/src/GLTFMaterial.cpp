#include "GLTFMaterial.h"
#include "GLTFNode.h"

#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"

GLTF::Material::Material() {
	this->values = new GLTF::Material::Values();
	this->type = GLTF::Material::MATERIAL;
}

bool GLTF::Material::hasTexture() {
	return this->values->diffuseTexture != NULL;
}

std::string GLTF::Material::typeName() {
	return "material";
}

void GLTF::Material::Values::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (ambient != NULL || ambientTexture != NULL) {
		jsonWriter->Key("ambient");
		if (ambientTexture != NULL && options->version == "1.0") {
			jsonWriter->String(ambientTexture->getStringId().c_str());
		}
		else {
			jsonWriter->StartArray();
			if (ambientTexture != NULL) {
				jsonWriter->Int(ambientTexture->id);
			}
			else {
				for (int i = 0; i < 4; i++) {
					jsonWriter->Double(ambient[i]);
				}
			}
			jsonWriter->EndArray();
		}
	}

	if (diffuse != NULL || diffuseTexture != NULL) {
		jsonWriter->Key("diffuse");
		if (diffuseTexture != NULL && options->version == "1.0") {
			jsonWriter->String(diffuseTexture->getStringId().c_str());
		}
		else {
			jsonWriter->StartArray();
			if (diffuseTexture != NULL) {
				jsonWriter->Int(diffuseTexture->id);
			}
			else {
				for (int i = 0; i < 4; i++) {
					jsonWriter->Double(diffuse[i]);
				}
			}
			jsonWriter->EndArray();
		}
	}

	if (emission != NULL || emissionTexture != NULL) {
		jsonWriter->Key("emission");
		if (emissionTexture != NULL && options->version == "1.0") {
			jsonWriter->String(emissionTexture->getStringId().c_str());
		}
		else {
			jsonWriter->StartArray();
			if (emissionTexture != NULL) {
				jsonWriter->Int(emissionTexture->id);
			}
			else {
				for (int i = 0; i < 4; i++) {
					jsonWriter->Double(emission[i]);
				}
			}
			jsonWriter->EndArray();
		}
	}

	if (specular != NULL || specularTexture != NULL) {
		jsonWriter->Key("specular");
		if (specularTexture != NULL && options->version == "1.0") {
			jsonWriter->String(specularTexture->getStringId().c_str());
		}
		else {
			jsonWriter->StartArray();
			if (specularTexture != NULL) {
				jsonWriter->Int(specularTexture->id);
			}
			else {
				for (int i = 0; i < 4; i++) {
					jsonWriter->Double(specular[i]);
				}
			}
			jsonWriter->EndArray();
		}
	}

	if (shininess != NULL) {
		jsonWriter->Key("shininess");
		if (options->version != "1.0") {
			jsonWriter->StartArray();
		}
		jsonWriter->Double(this->shininess[0]);
		if (options->version != "1.0") {
			jsonWriter->EndArray();
		}
	}

	if (transparency != NULL) {
		jsonWriter->Key("transparency");
		if (options->version != "1.0") {
			jsonWriter->StartArray();
		}
		jsonWriter->Double(this->transparency[0]);
		if (options->version != "1.0") {
			jsonWriter->EndArray();
		}
	}
}

void GLTF::Material::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (this->values) {
		jsonWriter->Key("values");
		jsonWriter->StartObject();
		this->values->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (this->technique) {
		jsonWriter->Key("technique");
		if (options->version == "1.0") {
			jsonWriter->String(technique->getStringId().c_str());
		}
		else {
			jsonWriter->Int(technique->id);
		}
	}
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::MaterialPBR::Texture::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = static_cast<rapidjson::Writer<rapidjson::StringBuffer>*>(writer);
	if (scale != 1) {
		jsonWriter->Key("scale");
		jsonWriter->Double(scale);
	}
	if (texture) {
		jsonWriter->Key("index");
		jsonWriter->Int(texture->id);
	}
	if (texCoord > 0) {
		jsonWriter->Key("texCoord");
		jsonWriter->Int(texCoord);
	}
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::MaterialPBR::MetallicRoughness::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (baseColorFactor) {
		jsonWriter->Key("baseColorFactor");
		jsonWriter->StartArray();
		for (int i = 0; i < 4; i++) {
			jsonWriter->Double(baseColorFactor[i]);
		}
		jsonWriter->EndArray();
	}
	if (baseColorTexture) {
		jsonWriter->Key("baseColorTexture");
		jsonWriter->StartObject();
		baseColorTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (metallicFactor >= 0) {
		jsonWriter->Key("metallicFactor");
		jsonWriter->Double(metallicFactor);
	}
	if (roughnessFactor >= 0) {
		jsonWriter->Key("roughnessFactor");
		jsonWriter->Double(roughnessFactor);
	}
	if (metallicRoughnessTexture) {
		jsonWriter->Key("metallicRoughnessTexture");
		jsonWriter->StartObject();
		metallicRoughnessTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::MaterialPBR::SpecularGlossiness::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (diffuseFactor) {
		jsonWriter->Key("diffuseFactor");
		jsonWriter->StartArray();
		for (int i = 0; i < 4; i++) {
			jsonWriter->Double(diffuseFactor[i]);
		}
		jsonWriter->EndArray();
	}
	if (diffuseTexture) {
		jsonWriter->Key("diffuseTexture");
		jsonWriter->StartObject();
		diffuseTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (specularFactor) {
		jsonWriter->Key("specularFactor");
		jsonWriter->StartArray();
		for (int i = 0; i < 3; i++) {
			jsonWriter->Double(specularFactor[i]);
		}
		jsonWriter->EndArray();
	}
	if (glossinessFactor) {
		jsonWriter->Key("glossinessFactor");
		jsonWriter->Double(glossinessFactor[0]);
	}
	if (specularGlossinessTexture) {
		jsonWriter->Key("specularGlossinessTexture");
		jsonWriter->StartObject();
		specularGlossinessTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	GLTF::Object::writeJSON(writer, options);
}

void GLTF::MaterialPBR::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (metallicRoughness) {
		jsonWriter->Key("pbrMetallicRoughness");
		jsonWriter->StartObject();
		metallicRoughness->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (emissiveFactor) {
		jsonWriter->Key("emissiveFactor");
		jsonWriter->StartArray();
		for (int i = 0; i < 3; i++) {
			jsonWriter->Double(emissiveFactor[i]);
		}
		jsonWriter->EndArray();
	}
	if (emissiveTexture) {
		jsonWriter->Key("emissiveTexture");
		jsonWriter->StartObject();
		emissiveTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (normalTexture) {
		jsonWriter->Key("normalTexture");
		jsonWriter->StartObject();
		normalTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (occlusionTexture) {
		jsonWriter->Key("occlusionTexture");
		jsonWriter->StartObject();
		occlusionTexture->writeJSON(writer, options);
		jsonWriter->EndObject();
	}
	if (options->specularGlossiness) {
		jsonWriter->Key("extensions");
		jsonWriter->StartObject();
		jsonWriter->Key("KHR_materials_pbrSpecularGlossiness");
		jsonWriter->StartObject();
		specularGlossiness->writeJSON(writer, options);
		jsonWriter->EndObject();
		jsonWriter->EndObject();
	}

	if (!this->alphaMode.empty()) {
		jsonWriter->Key("alphaMode");
		jsonWriter->String(this->alphaMode.c_str());

		if (this->alphaMode == "MASK" && !std::isnan(this->alphaCutoff)) {
			jsonWriter->Key("alphaCutoff");
			jsonWriter->Double(this->alphaCutoff);
		}
	}

	if (options->doubleSided || this->doubleSided) {
		jsonWriter->Key("doubleSided");
		jsonWriter->Bool(true);
	}

	GLTF::Object::writeJSON(writer, options);
}

std::string GLTF::MaterialCommon::Light::typeName() {
	return "light";
}

void GLTF::MaterialCommon::Light::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	if (type != MaterialCommon::Light::UNKOWN) {
		switch (type) {
		case MaterialCommon::Light::DIRECTIONAL:
			jsonWriter->Key("type");
			jsonWriter->String("directional");
			jsonWriter->Key("directional");
			break;
		case MaterialCommon::Light::AMBIENT:
			jsonWriter->Key("type");
			jsonWriter->String("ambient");
			jsonWriter->Key("ambient");
			break;
		case MaterialCommon::Light::POINT:
			jsonWriter->Key("type");
			jsonWriter->String("point");
			jsonWriter->Key("point");
			break;
		}
		jsonWriter->StartObject();
		if (type == MaterialCommon::Light::POINT) {
			jsonWriter->Key("constantAttenuation");
			jsonWriter->Double(constantAttenuation);
			jsonWriter->Key("linearAttenuation");
			jsonWriter->Double(linearAttenuation);
			jsonWriter->Key("quadraticAttenuation");
			jsonWriter->Double(quadraticAttenuation);
		}
		jsonWriter->Key("color");
		jsonWriter->StartArray();
		for (int i = 0; i < 4; i++) {
			jsonWriter->Double(color[i]);
		}
		jsonWriter->EndArray();
		jsonWriter->EndObject();
	}
	GLTF::Object::writeJSON(writer, options);
}

GLTF::MaterialCommon::MaterialCommon() {
	this->values = new GLTF::Material::Values();
	this->type = GLTF::Material::MATERIAL_COMMON;
}

const char* GLTF::MaterialCommon::getTechniqueName() {
	switch (this->technique) {
	case BLINN:
		return "BLINN";
	case PHONG:
		return "PHONG";
	case LAMBERT:
		return "LAMBERT";
	case CONSTANT:
		return "CONSTANT";
	}
	return NULL;
}

GLTF::Material* GLTF::MaterialCommon::getMaterial(std::vector<GLTF::MaterialCommon::Light*> lights, GLTF::Options* options) {
	return getMaterial(lights, false, options);
}

GLTF::Material* GLTF::MaterialCommon::getMaterial(std::vector<GLTF::MaterialCommon::Light*> lights, bool hasColor, GLTF::Options* options) {
	GLTF::Material* material = new GLTF::Material();
	material->values = values;
	material->name = name;
	material->stringId = stringId;
	GLTF::Technique* technique = new GLTF::Technique();
	material->technique = technique;
	GLTF::Program* program = new GLTF::Program();
	technique->program = program;
	GLTF::Shader* fragmentShader = new GLTF::Shader();
	program->fragmentShader = fragmentShader;
	GLTF::Shader* vertexShader = new GLTF::Shader();
	program->vertexShader = vertexShader;

	bool hasNormals = this->technique != GLTF::MaterialCommon::Technique::CONSTANT;
	bool hasSkinning = jointCount > 0;
	bool hasTexture = false;

	std::string vertexShaderSource = "precision highp float;\n";
	std::string fragmentShaderSource = "precision highp float;\n";

	// Add matrices
	technique->parameters["modelViewMatrix"] = new GLTF::Technique::Parameter("MODELVIEW", GLTF::Constants::WebGL::FLOAT_MAT4);
	technique->uniforms["u_modelViewMatrix"] = "modelViewMatrix";
	vertexShaderSource += "uniform mat4 u_modelViewMatrix;\n";
	technique->parameters["projectionMatrix"] = new GLTF::Technique::Parameter("PROJECTION", GLTF::Constants::WebGL::FLOAT_MAT4);
	technique->uniforms["u_projectionMatrix"] = "projectionMatrix";
	vertexShaderSource += "uniform mat4 u_projectionMatrix;\n";
	if (hasNormals) {
		technique->parameters["normalMatrix"] = new GLTF::Technique::Parameter("MODELVIEWINVERSETRANSPOSE", GLTF::Constants::WebGL::FLOAT_MAT3);
		technique->uniforms["u_normalMatrix"] = "normalMatrix";
		vertexShaderSource += "uniform mat3 u_normalMatrix;\n";
	}
	if (hasSkinning) {
		technique->parameters["jointMatrix"] = new GLTF::Technique::Parameter("JOINTMATRIX", GLTF::Constants::WebGL::FLOAT_MAT4, jointCount);
		technique->uniforms["u_jointMatrix"] = "jointMatrix";
		vertexShaderSource += "uniform mat4 u_jointMatrix[" + std::to_string(jointCount) + "];\n";
	}
	
	// Add parameters and uniforms from values
	if (values->ambient != NULL) {
		technique->parameters["ambient"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->uniforms["u_ambient"] = "ambient";
		fragmentShaderSource += "uniform vec4 u_ambient;\n";
	}
	else if (values->ambientTexture != NULL) {
		technique->parameters["ambient"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::SAMPLER_2D);
		technique->uniforms["u_ambient"] = "ambient";
		fragmentShaderSource += "uniform sampler2D u_ambient;\n";
		hasTexture = true;
	}
	if (values->diffuse != NULL) {
		technique->parameters["diffuse"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->uniforms["u_diffuse"] = "diffuse";
		fragmentShaderSource += "uniform vec4 u_diffuse;\n";
	}
	else if (values->diffuseTexture != NULL) {
		technique->parameters["diffuse"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::SAMPLER_2D);
		technique->uniforms["u_diffuse"] = "diffuse";
		fragmentShaderSource += "uniform sampler2D u_diffuse;\n";
		hasTexture = true;
	}
	if (values->emission != NULL) {
		technique->parameters["emission"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->uniforms["u_emission"] = "emission";
		fragmentShaderSource += "uniform vec4 u_emission;\n";
	}
	else if (values->emissionTexture != NULL) {
		technique->parameters["emission"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::SAMPLER_2D);
		technique->uniforms["u_emission"] = "emission";
		fragmentShaderSource += "uniform sampler2D u_emission;\n";
		hasTexture = true;
	}
	if (values->specular != NULL) {
		technique->parameters["specular"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->uniforms["u_specular"] = "specular";
		fragmentShaderSource += "uniform vec4 u_specular;\n";
	}
	else if (values->specularTexture != NULL) {
		technique->parameters["specular"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::SAMPLER_2D);
		technique->uniforms["u_specular"] = "specular";
		fragmentShaderSource += "uniform sampler2D u_specular;\n";
		hasTexture = true;
	}
	if (values->shininess != NULL) {
		technique->parameters["shininess"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT);
		technique->uniforms["u_shininess"] = "shininess";
		fragmentShaderSource += "uniform float u_shininess;\n";
	}
	if (values->transparency != NULL) {
		technique->parameters["transparency"] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT);
		technique->uniforms["u_transparency"] = "transparency";
		fragmentShaderSource += "uniform float u_transparency;\n";
	}

	std::vector<std::string> ambientLights;
	std::map<std::string, GLTF::MaterialCommon::Light::Type> nonAmbientLights;
	// Add parameters and uniforms from lights
	for (size_t i = 0; i < lights.size(); i++) {
		GLTF::MaterialCommon::Light* light = lights[i];
		std::string name = "light" + std::to_string(i);
		std::string colorName = name + "Color";
		technique->parameters[colorName] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC3, light->color, 3);
		technique->uniforms["u_" + colorName] = colorName;
		fragmentShaderSource += "uniform vec3 u_" + colorName + ";\n";
		if (light->type == Light::AMBIENT) {
			ambientLights.push_back(name);
		}
		else {
			nonAmbientLights[name] = light->type;
			if (light->node != NULL) {
				GLTF::Node* node = (GLTF::Node*)light->node;
				std::string transformName = name + "Transform";
				GLTF::Technique::Parameter* nodeTransform = new GLTF::Technique::Parameter("MODELVIEW", GLTF::Constants::WebGL::FLOAT_MAT4);
				nodeTransform->node = node->id;
				nodeTransform->nodeString = node->getStringId();
				technique->parameters[transformName] = nodeTransform;
				technique->uniforms["u_" + transformName] = transformName;
				vertexShaderSource += "uniform mat4 u_" + transformName + ";\n";
				if (light->type == GLTF::MaterialCommon::Light::Type::POINT) {
					std::string attenuationName = name + "Attenuation";
					float* attenuation = new float[3];
					attenuation[0] = light->constantAttenuation;
					attenuation[1] = light->linearAttenuation;
					attenuation[2] = light->quadraticAttenuation;
					technique->parameters[attenuationName] = new GLTF::Technique::Parameter(GLTF::Constants::WebGL::FLOAT_VEC3, attenuation, 3);
					technique->uniforms["u_" + attenuationName] = attenuationName;
					fragmentShaderSource += "uniform vec3 u_" + attenuationName + ";\n";
				}
			}
		}
	}

	// Add parameters and attributes
	technique->parameters["position"] = new GLTF::Technique::Parameter("POSITION", GLTF::Constants::WebGL::FLOAT_VEC3);
	technique->attributes["a_position"] = "position";
	program->attributes.insert("a_position");
	if (hasNormals) {
		technique->parameters["normal"] = new GLTF::Technique::Parameter("NORMAL", GLTF::Constants::WebGL::FLOAT_VEC3);
		technique->attributes["a_normal"] = "normal";
		program->attributes.insert("a_normal");
	}
	if (hasTexture) {
		technique->parameters["texcoord0"] = new GLTF::Technique::Parameter("TEXCOORD_0", GLTF::Constants::WebGL::FLOAT_VEC3);
		technique->attributes["a_texcoord0"] = "texcoord0";
		program->attributes.insert("a_texcoord0");
	}
	if (hasColor) {
		technique->parameters["color0"] = new GLTF::Technique::Parameter("COLOR_0", GLTF::Constants::WebGL::FLOAT_VEC3);
		technique->attributes["a_color0"] = "color0";
		program->attributes.insert("a_color0");
	}
	if (hasSkinning) {
		technique->parameters["joint"] = new GLTF::Technique::Parameter("JOINT", GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->attributes["a_joint"] = "joint";
		program->attributes.insert("a_joint");
		technique->parameters["weight"] = new GLTF::Technique::Parameter("WEIGHT", GLTF::Constants::WebGL::FLOAT_VEC4);
		technique->attributes["a_weight"] = "weight";
		program->attributes.insert("a_weight");
	}

	if (transparent) {
		technique->enableStates.insert(GLTF::Constants::WebGL::CULL_FACE);
		technique->enableStates.insert(GLTF::Constants::WebGL::DEPTH_TEST);
		technique->depthMask = new bool[1];
		technique->depthMask[0] = false;
		technique->blendEquationSeparate.push_back(GLTF::Constants::WebGL::FUNC_ADD);
		technique->blendEquationSeparate.push_back(GLTF::Constants::WebGL::FUNC_ADD);
		technique->blendFuncSeparate.push_back(GLTF::Constants::WebGL::ONE);
		technique->blendFuncSeparate.push_back(GLTF::Constants::WebGL::ONE_MINUS_SRC_ALPHA);
		technique->blendFuncSeparate.push_back(GLTF::Constants::WebGL::ONE);
		technique->blendFuncSeparate.push_back(GLTF::Constants::WebGL::ONE_MINUS_SRC_ALPHA);
	}
	else if (doubleSided || options->doubleSided) {
		technique->enableStates.insert(GLTF::Constants::WebGL::DEPTH_TEST);
	}
	else {
		technique->enableStates.insert(GLTF::Constants::WebGL::CULL_FACE);
		technique->enableStates.insert(GLTF::Constants::WebGL::DEPTH_TEST);
	}

	// Add attributes with semantics
	std::string vertexShaderMain = "";
	if (hasSkinning) {
		vertexShaderMain += "\
    mat4 skinMat = a_weight.x * u_jointMatrix[int(a_joint.x)];\n\
    skinMat += a_weight.y * u_jointMatrix[int(a_joint.y)];\n\
    skinMat += a_weight.z * u_jointMatrix[int(a_joint.z)];\n\
    skinMat += a_weight.w * u_jointMatrix[int(a_joint.w)];\n";
	}

	// Add position always
	vertexShaderSource += "\
attribute vec3 a_position;\n\
varying vec3 v_position;\n";
	if (hasSkinning) {
		vertexShaderMain += "    vec4 pos = u_modelViewMatrix * skinMat * vec4(a_position,1.0);\n";
	}
	else {
		vertexShaderMain += "    vec4 pos = u_modelViewMatrix * vec4(a_position,1.0);\n";
	}
	vertexShaderMain += "\
    v_position = pos.xyz;\n\
    gl_Position = u_projectionMatrix * pos;\n";
	fragmentShaderSource += "varying vec3 v_position;\n";

	// Add normal if we don't have constant lighting
	if (hasNormals) {
		vertexShaderSource += "\
attribute vec3 a_normal;\n\
varying vec3 v_normal;\n";
		if (hasSkinning) {
			vertexShaderMain += "    v_normal = u_normalMatrix * mat3(skinMat) * a_normal;\n";
		}
		else {
			vertexShaderMain += "    v_normal = u_normalMatrix * a_normal;\n";
		}

		fragmentShaderSource += "varying vec3 v_normal;\n";
	}

	// Add texture coordinates if the material uses them
	std::string v_texcoord;
	if (hasTexture) {
		v_texcoord = "v_texcoord0";
		vertexShaderSource += "\
attribute vec2 a_texcoord0;\n\
varying vec2 " + v_texcoord + ";\n";
		vertexShaderMain += "    " + v_texcoord + " = a_texcoord0;\n";
		fragmentShaderSource += "varying vec2 " + v_texcoord + ";\n";
	}

	// Add color if a color attribute exists
	if (hasColor) {
		vertexShaderSource += "\
attribute vec3 a_color0;\n\
varying vec3 v_color0;\n";
		vertexShaderMain += "    v_color0 = a_color0;\n";
		fragmentShaderSource += "varying vec3 v_color0;\n";
	}

	if (hasSkinning) {
		vertexShaderSource += "\
attribute vec4 a_joint;\n\
attribute vec4 a_weight;\n";
	}

	bool hasSpecular = hasNormals && ((this->technique == Technique::BLINN) || (this->technique == Technique::PHONG)) &&
		(values->specular != NULL || values->specularTexture != NULL) && values->shininess != NULL;

	// Generate lighting code blocks
	std::string fragmentLightingBlock = "";
	for (std::string lightBaseName : ambientLights) {
		fragmentLightingBlock += "\
    {\n\
        ambientLight += u_" + lightBaseName + "Color;\n\
    }\n";
	}
	if (hasNormals) {
		for (auto const light : nonAmbientLights) {
			std::string lightBaseName = light.first;
			GLTF::MaterialCommon::Light::Type lightType = light.second;
			fragmentLightingBlock += "    {\n";
			std::string varyingDirectionName = "v_" + lightBaseName + "Direction";
			std::string varyingPositionName = "v_" + lightBaseName + "Position";

			if (lightType != Light::POINT) {
				vertexShaderSource += "varying vec3 " + varyingDirectionName + ";\n";
				fragmentShaderSource += "varying vec3 " + varyingDirectionName + ";\n";

				vertexShaderMain += "    " + varyingDirectionName + " = mat3(u_" + lightBaseName + "Transform) * vec3(0., 0., 1.);\n";
				if (lightType == Light::DIRECTIONAL) {
					fragmentLightingBlock += "    vec3 l = normalize(" + varyingDirectionName + ");\n";
				}
			}

			if (lightType != Light::DIRECTIONAL) {
				vertexShaderSource += "varying vec3 " + varyingPositionName + ";\n";
				fragmentShaderSource += "varying vec3 " + varyingPositionName + ";\n";

				vertexShaderMain += "    " + varyingPositionName + " = u_" + lightBaseName + "Transform[3].xyz;\n";
				fragmentLightingBlock += "\
    vec3 VP = " + varyingPositionName + " - v_position;\n\
    vec3 l = normalize(VP);\n\
    float range = length(VP);\n\
    float attenuation = 1.0 / (u_" + lightBaseName + "Attenuation.x + \
(u_" + lightBaseName + "Attenuation.y * range) + \
(u_" + lightBaseName + "Attenuation.z * range * range));\n";
			}
			else {
				fragmentLightingBlock += "    float attenuation = 1.0;\n";
			}

			if (lightType == Light::SPOT) {
				fragmentLightingBlock += "\
    float spotDot = dot(l, normalize(" + varyingDirectionName + "));\n\
    if (spotDot < cos(u_" + lightBaseName + "FallOff.x * 0.5))\n\
    {\n\
        attenuation = 0.0;\n\
    }\n\
    else\n\
    {\n\
        attenuation *= max(0.0, pow(spotDot, u_" + lightBaseName + "FallOff.y));\n\
    }\n";
			}

			fragmentLightingBlock += "    diffuseLight += u_" + lightBaseName + "Color * max(dot(normal, l), 0.) * attenuation;\n";

			if (hasSpecular) {
				if (this->technique == Technique::BLINN) {
					fragmentLightingBlock += "\
    vec3 h = normalize(l + viewDir);\n\
    float specularIntensity = max(0., pow(max(dot(normal, h), 0.), u_shininess)) * attenuation;\n";
				} 
				else { // PHONG
					fragmentLightingBlock += "\
    vec3 reflectDir = reflect(-l, normal);\n\
    float specularIntensity = max(0., pow(max(dot(reflectDir, viewDir), 0.), u_shininess)) * attenuation;\n";
				}
			}
			fragmentLightingBlock += "\
    specularLight += u_" + lightBaseName + "Color * specularIntensity;\n\
    }\n";
		}
	}

	if (ambientLights.size() == 0) {
		// Add an ambient light if we don't have one
		fragmentLightingBlock += "    ambientLight += vec3(0.2, 0.2, 0.2);\n";
	}

	if (nonAmbientLights.size() == 0 && this->technique != Technique::CONSTANT) {
		fragmentLightingBlock += "\
    vec3 l = vec3(0.0, 0.0, 1.0);\n\
    diffuseLight += vec3(1.0, 1.0, 1.0) * max(dot(normal, l), 0.); \n";

		if (hasSpecular) {
			if (this->technique == Technique::BLINN) {
				fragmentLightingBlock += "\
    vec3 h = normalize(l + viewDir);\n\
    float specularIntensity = max(0., pow(max(dot(normal, h), 0.), u_shininess));\n";
			}
			else { // PHONG
				fragmentLightingBlock += "\
    vec3 reflectDir = reflect(-l, normal);\n\
    float specularIntensity = max(0., pow(max(dot(reflectDir, viewDir), 0.), u_shininess));\n";
			}

			fragmentLightingBlock += "\
    specularLight += vec3(1.0, 1.0, 1.0) * specularIntensity;\n";
		}
	}

	vertexShaderSource += "\
void main(void) {\n\
" + vertexShaderMain + "\
}\n";

	fragmentShaderSource += "\
void main(void) {\n";
	
	std::string colorCreationBlock;
	if (!hasColor) {
		colorCreationBlock = "    vec3 color = vec3(0.0, 0.0, 0.0);\n";
	}
	else {
		colorCreationBlock = "    vec3 color = v_color0;\n";
	}
	if (hasNormals) {
		fragmentShaderSource += "    vec3 normal = normalize(v_normal);\n";
		if (doubleSided || options->doubleSided) {
			fragmentShaderSource += "\
    if (gl_FrontFacing == false)\n\
    {\n\
        normal = -normal;\n\
	}\n";
		}
	}

	std::string finalColorComputation;
	if (this->technique != Technique::CONSTANT) {
		if (values->diffuse != NULL || values->diffuseTexture != NULL) {
			if (values->diffuseTexture != NULL) {
				fragmentShaderSource += "    vec4 diffuse = texture2D(u_diffuse, " + v_texcoord + ");\n";
			}
			else {
				fragmentShaderSource += "    vec4 diffuse = u_diffuse;\n";
			}
			fragmentShaderSource += "    vec3 diffuseLight = vec3(0.0, 0.0, 0.0);\n";
			colorCreationBlock += "    color += diffuse.rgb * diffuseLight;\n";
		}

		if (hasSpecular) {
			if (values->specularTexture != NULL) {
				fragmentShaderSource += "    vec3 specular = texture2D(u_specular, " + v_texcoord + ").rgb;\n";
			}
			else {
				fragmentShaderSource += "    vec3 specular = u_specular.rgb;\n";
			}
			fragmentShaderSource += "    vec3 specularLight = vec3(0.0, 0.0, 0.0);\n";
			colorCreationBlock += "    color += specular * specularLight;\n";
		}

		if (values->transparency != NULL) {
			finalColorComputation = "    gl_FragColor = vec4(color * diffuse.a * u_transparency, diffuse.a * u_transparency);\n";
		}
		else {
			finalColorComputation = "    gl_FragColor = vec4(color * diffuse.a, diffuse.a);\n";
		}
	}
	else {
		if (values->transparency != NULL) {
			finalColorComputation = "    gl_FragColor = vec4(color * u_transparency, u_transparency);\n";
		}
		else {
			finalColorComputation = "    gl_FragColor = vec4(color, 1.0);\n";
		}
	}

	if (values->emission != NULL || values->emissionTexture != NULL) {
		if (values->emissionTexture != NULL) {
			fragmentShaderSource += "    vec3 emission = texture2D(u_emission, " + v_texcoord + ").rgb;\n";
		}
		else {
			fragmentShaderSource += "    vec3 emission = u_emission.rgb;\n";
		}
		colorCreationBlock += "    color += emission;\n";
	}

	if (values->ambient != NULL || values->ambientTexture != NULL || this->technique != Technique::CONSTANT) {
		if (values->ambientTexture != NULL) {
			fragmentShaderSource += "    vec3 ambient = texture2D(u_ambient, " + v_texcoord + ").rgb;\n";
		}
		else if (values->ambient != NULL) {
			fragmentShaderSource += "    vec3 ambient = u_ambient.rgb;\n";
		}
		else {
			fragmentShaderSource += "    vec3 ambient = diffuse.rgb;\n";
		}
		colorCreationBlock += "    color += ambient * ambientLight;\n";
	}
	fragmentShaderSource += "\
    vec3 viewDir = -normalize(v_position);\n\
    vec3 ambientLight = vec3(0.0, 0.0, 0.0);\n";

	// Add in light computations
	fragmentShaderSource += fragmentLightingBlock;

	fragmentShaderSource += colorCreationBlock;
	fragmentShaderSource += finalColorComputation;
	fragmentShaderSource += "}\n";

	fragmentShader->type = GLTF::Constants::WebGL::FRAGMENT_SHADER;
	fragmentShader->source = fragmentShaderSource;

	vertexShader->type = GLTF::Constants::WebGL::VERTEX_SHADER;	
	vertexShader->source = vertexShaderSource;

	return material;
}

std::string GLTF::MaterialCommon::getTechniqueKey(GLTF::Options* options) {
	std::string id = "";
	if (values->ambient != NULL) {
		id += "AMBIENT;";
	}
	if (values->diffuse != NULL) {
		id += "DIFFUSE;";
	}
	else if (values->diffuseTexture != NULL) {
		id += "DIFFUSETEXTURE;";
	}
	if (values->emission != NULL) {
		id += "EMISSION;";
	}
	if (values->specular != NULL) {
		id += "SPECULAR;";
	}
	if (values->shininess != NULL) {
		id += "SHININESS;";
	}
	if (values->transparency != NULL) {
		id += "TRANSPARENCY;";
	}
	if (doubleSided || options->doubleSided) {
		id += "DOUBLESIDED;";
	}
	if (transparent) {
		id += "TRANSPARENT;";
	}
	if (jointCount) {
		id += "SKINNED;";
	}
	return id;
}

GLTF::MaterialPBR::MaterialPBR() {
	this->type = GLTF::Material::PBR_METALLIC_ROUGHNESS;
	this->metallicRoughness = new GLTF::MaterialPBR::MetallicRoughness();
	this->specularGlossiness = new GLTF::MaterialPBR::SpecularGlossiness();
}

GLTF::MaterialPBR* GLTF::MaterialCommon::getMaterialPBR(GLTF::Options* options) {
	GLTF::MaterialPBR* material = new GLTF::MaterialPBR();
	material->metallicRoughness->metallicFactor = 0;
	bool hasTransparency = false;
	if (values->diffuse) {
		if (values->diffuse[3] < 1.0) {
			hasTransparency = true;
		}
		material->metallicRoughness->baseColorFactor = values->diffuse;
		if (options->specularGlossiness) {
			material->specularGlossiness->diffuseFactor = values->diffuse;
		}
	}
	if (values->diffuseTexture) {
		GLTF::MaterialPBR::Texture* texture = new GLTF::MaterialPBR::Texture();
		texture->texCoord = values->diffuseTexCoord;
		texture->texture = values->diffuseTexture;
		material->metallicRoughness->baseColorTexture = texture;
		if (options->specularGlossiness) {
			material->specularGlossiness->diffuseTexture = texture;
		}
	}

	if (values->emission) {
		if (values->emission[3] < 1.0) {
			hasTransparency = true;
		}
		material->emissiveFactor = values->emission;
	}
	if (values->emissionTexture) {
		GLTF::MaterialPBR::Texture* texture = new GLTF::MaterialPBR::Texture();
		texture->texCoord = values->emissionTexCoord;
		texture->texture = values->emissionTexture;
		material->emissiveTexture = texture;
		material->emissiveFactor = new float[3]{ 1.0, 1.0, 1.0 };
	}

	if (values->ambientTexture) {
		GLTF::MaterialPBR::Texture* texture = new GLTF::MaterialPBR::Texture();
		texture->texCoord = values->ambientTexCoord;
		texture->texture = values->ambientTexture;
		material->occlusionTexture = texture;
	}

	if (options->specularGlossiness) {
		if (values->specular) {
			if (values->specular[3] < 1.0) {
				hasTransparency = true;
			}
			material->specularGlossiness->specularFactor = values->specular;
		}
		if (values->specularTexture) {
			GLTF::MaterialPBR::Texture* texture = new GLTF::MaterialPBR::Texture();
			texture->texCoord = values->specularTexCoord;
			texture->texture = values->specularTexture;
			material->specularGlossiness->specularGlossinessTexture = texture;
		}
		if (values->shininess) {
			if (values->shininess[0] < 1.0) {
				material->specularGlossiness->glossinessFactor = values->shininess;
			}
			else {
				material->specularGlossiness->glossinessFactor = new float[1] {1.0};
			}
		}
	}

	if (values->bumpTexture) {
		GLTF::MaterialPBR::Texture* texture = new GLTF::MaterialPBR::Texture();
		texture->texture = values->bumpTexture;
		material->normalTexture = texture;
	}

	if (values->transparency) {
		if (!material->metallicRoughness->baseColorFactor) {
			float* baseColorFactor = new float[4];
			baseColorFactor[0] = 1.0;
			baseColorFactor[1] = 1.0;
			baseColorFactor[2] = 1.0;
			material->metallicRoughness->baseColorFactor = baseColorFactor;
		}
		float transparency = material->metallicRoughness->baseColorFactor[3];
		transparency *= values->transparency[0];
		material->metallicRoughness->baseColorFactor[3] = transparency;
		if (transparency < 1.0) {
			hasTransparency = true;
		}
	}

	if (hasTransparency) {
		material->alphaMode = "BLEND";
	}

	if (options->doubleSided || doubleSided) {
		material->doubleSided = true;
	}
	material->name = name;
	return material;
}

void GLTF::MaterialCommon::writeJSON(void* writer, GLTF::Options* options) {
	rapidjson::Writer<rapidjson::StringBuffer>* jsonWriter = (rapidjson::Writer<rapidjson::StringBuffer>*)writer;
	jsonWriter->Key("extensions");
	jsonWriter->StartObject();
	jsonWriter->Key("KHR_materials_common");
	jsonWriter->StartObject();
	if (options->doubleSided || this->doubleSided) {
		jsonWriter->Key("doubleSided");
		jsonWriter->Bool(true);
	}
	if (this->jointCount > 0) {
		jsonWriter->Key("jointCount");
		jsonWriter->Int(this->jointCount);
	}
	jsonWriter->Key("technique");
	jsonWriter->String(this->getTechniqueName());
	jsonWriter->Key("transparent");
	jsonWriter->Bool(this->transparent);
	GLTF::Material::writeJSON(writer, options);
	jsonWriter->EndObject();
	jsonWriter->EndObject();
	GLTF::Object::writeJSON(writer, options);
}

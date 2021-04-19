#include "stdafx.h"
#include "Model.h"

bool Model::Initialize(const std::string& filePath, ID3D11Device* device, ID3D11DeviceContext* deviceContext)
{
	_device = device;
	_deviceContext = deviceContext;
	_animIndex = 0;
	for (int i = 0; i < 100; i++)
	{
		_cb_vs_vertexshader_skeleton->_data.boneTransform[i] = XMMatrixIdentity();
	}
	COM_THROW_IF_FAILED(!LoadModel(filePath));
	try
	{
		if ()
			return false;
	}
	catch (COMException& exception)
	{
		ErrorLogger::Log(exception);
		return false;
	}

	for (int i = 0; i < 100; i++)
	{
		_currentBone[i] = XMMatrixIdentity();
	}
	return true;
}

void Model::Update(const XMMATRIX& worldMatrix, const XMMATRIX& viewProjectionMatrix, float* currentTime)
{

	CB_VS_vertexshader_skeleton cb;

	_cb_vs_vertexshader_skeleton->CopyData()
	

	if (currentTime)
	{
		if (_animIndex < _animations.size())
		{
			SetAnimBoneTransform(*currentTime);
		}
		else
		{
			assert("anim이 있는데 시간을 안넣어줬다.");
		}
	}

	_deviceContext->VSSetConstantBuffers(0, 1, _cb_vs_vertexshader_skeleton->GetAddressOf());
	static int test = 0;
	for (int i = 0; i < _meshes.size(); i++)
	{
		//Update Constant buffer with WVP Matrix
		_cb_vs_vertexshader_skeleton->_data.worldMatrix = _meshes[i].GetTransformMatirx() * worldMatrix;
		_cb_vs_vertexshader_skeleton->_data.wvpMatrix = _cb_vs_vertexshader_skeleton->_data.worldMatrix * viewProjectionMatrix; //Calculate World-View-Projection Matrix


		for (int j = 0; j < 100; j++)
		{
			_cb_vs_vertexshader_skeleton->_data.boneTransform[j] = _currentBone[j];
		}

		_cb_vs_vertexshader_skeleton->ApplyChanges();
		_meshes[i].Update();
	}
}

bool Model::GetIsAnim()
{
	return !_animations.empty();
}


bool Model::LoadModel(const std::string& filePath)
{
	Assimp::Importer importer;
	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);
	/*const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate |
		aiProcess_ConvertToLeftHanded);*/
	if (scene == nullptr)
		return false;
	if (_meshes.empty())
	{
		_directory = StringHelper::GetDirectoryFromPath(filePath);
		int upAxis, frontAxis, coordAxis = 0;
		_axisMatrix = XMMatrixIdentity();
		if (scene->mMetaData)
		{
			scene->mMetaData->mNumProperties;
			for (int i = 0; i < scene->mMetaData->mNumProperties; i++)
			{
				aiString metaKey = scene->mMetaData->mKeys[i];
				aiMetadataEntry metaValue = scene->mMetaData->mValues[i];
				int a = 1;
			}

			scene->mMetaData->Get<int>("UpAxis", upAxis);
			scene->mMetaData->Get<int>("FrontAxis", frontAxis);
			scene->mMetaData->Get<int>("CoordAxis", coordAxis);
			int upAxisSign, frontAxisSign, coordAxisSign = 1;
			scene->mMetaData->Get<int>("UpAxisSign", upAxisSign);
			scene->mMetaData->Get<int>("FrontAxisSign", frontAxisSign);
			scene->mMetaData->Get<int>("CoordAxisSign", coordAxisSign);
			aiVector3D upVec = upAxis == 0 ? aiVector3D(upAxisSign, 0, 0) : upAxis == 1 ? aiVector3D(0, upAxisSign, 0) : aiVector3D(0, 0, upAxisSign);
			aiVector3D forwardVec = frontAxis == 0 ? aiVector3D(frontAxisSign, 0, 0) : frontAxis == 1 ? aiVector3D(0, frontAxisSign, 0) : aiVector3D(0, 0, frontAxisSign);
			aiVector3D rightVec = coordAxis == 0 ? aiVector3D(coordAxisSign, 0, 0) : coordAxis == 1 ? aiVector3D(0, coordAxisSign, 0) : aiVector3D(0, 0, coordAxisSign);
			_axisMatrix = XMMATRIX(rightVec.x, rightVec.y, rightVec.z, 0.0f,
				upVec.x, upVec.y, upVec.z, 0.0f,
				forwardVec.x, forwardVec.y, forwardVec.z, 0.0f,
				0.0f, 0.0f, 0.0f, 1.0f);
		}
		ProcessNode(scene, scene->mRootNode, _axisMatrix, &_rootNode);
	}

	for (unsigned int i = 0; i < scene->mNumAnimations; i++)
	{
		_animations.push_back(AnimData());
		AnimData& animData = _animations.back();
		animData.filePath = filePath;
		animData.tickPerSecond = scene->mAnimations[i]->mTicksPerSecond;
		animData.duration = scene->mAnimations[i]->mDuration;
		for (unsigned int j = 0; j < scene->mAnimations[i]->mNumChannels; j++)
		{
			NodeAnimData& curNodeAnimData = animData.nodeAnimDatas[scene->mAnimations[i]->mChannels[j]->mNodeName.C_Str()];
			for (unsigned int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumScalingKeys; k++)
			{
				curNodeAnimData.scalingKeys.push_back(KeyVectorData());
				KeyVectorData& curKeyData = curNodeAnimData.scalingKeys.back();
				curKeyData.time = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mTime;
				curKeyData.vectorData = scene->mAnimations[i]->mChannels[j]->mScalingKeys[k].mValue;
			}

			for (unsigned int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumRotationKeys; k++)
			{
				curNodeAnimData.rotationKeys.push_back(KeyQuaterData());
				KeyQuaterData& curKeyData = curNodeAnimData.rotationKeys.back();
				curKeyData.time = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mTime;
				curKeyData.quaterData = scene->mAnimations[i]->mChannels[j]->mRotationKeys[k].mValue;
			}

			for (unsigned int k = 0; k < scene->mAnimations[i]->mChannels[j]->mNumPositionKeys; k++)
			{
				curNodeAnimData.positionKeys.push_back(KeyVectorData());
				KeyVectorData& curKeyData = curNodeAnimData.positionKeys.back();
				curKeyData.time = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mTime;
				curKeyData.vectorData = scene->mAnimations[i]->mChannels[j]->mPositionKeys[k].mValue;
			}
		}
	}
	//importer.FreeScene();
	return true;
}

void Model::ProcessNode(const aiScene* scene, aiNode* node, const XMMATRIX& parentTransformMatrix, NodeData* animData)
{
	animData->name = node->mName.C_Str();
	animData->transfomation = XMMatrixTranspose(XMMATRIX(&node->mTransformation.a1));
	XMMATRIX nodeTransformMatrix = animData->transfomation * parentTransformMatrix;
	for (UINT i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		_meshes.push_back(ProcessMesh(scene, mesh, nodeTransformMatrix));
	}
	for (UINT i = 0; i < node->mNumChildren; i++)
	{
		animData->childs.push_back(NodeData());
		ProcessNode(scene, node->mChildren[i], nodeTransformMatrix, &animData->childs.back());
	}
}

Mesh Model::ProcessMesh(const aiScene* scene, aiMesh* mesh, const XMMATRIX& transformMatrix)
{
	std::vector<Vertex3D_Skeleton> vertices;
	std::vector<DWORD> indices;

	vertices.reserve(mesh->mNumVertices);
	for (UINT i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex3D_Skeleton vertex;

		vertex._pos.x = mesh->mVertices[i].x;
		vertex._pos.y = mesh->mVertices[i].y;
		vertex._pos.z = mesh->mVertices[i].z;

		if (mesh->mNormals)
		{
			vertex._normal.x = mesh->mNormals[i].x;
			vertex._normal.y = mesh->mNormals[i].y;
			vertex._normal.z = mesh->mNormals[i].z;
		}


		if (mesh->mTextureCoords[0])
		{
			vertex._texCoord.x = (float)mesh->mTextureCoords[0][i].x;
			vertex._texCoord.y = (float)mesh->mTextureCoords[0][i].y;
		}

		vertices.push_back(vertex);
	}


	std::vector<UINT> boneCounts;
	boneCounts.resize(vertices.size(), 0);

	for (UINT i = 0; i < mesh->mNumBones; i++)
	{
		aiBone* bone = mesh->mBones[i];
		XMMATRIX offsetMatrix = XMMatrixTranspose(XMMATRIX(&bone->mOffsetMatrix.a1));
		_boneInfo[bone->mName.C_Str()] = { i, offsetMatrix };
		for (UINT j = 0; j < bone->mNumWeights; j++)
		{
			UINT id = bone->mWeights[j].mVertexId;
			float weight = bone->mWeights[j].mWeight;
			boneCounts[id]++;
			assert(weight != 0);

			switch (boneCounts[id])
			{
			case 1:
				vertices[id]._boneIds1.x = i;
				vertices[id]._boneWeights1.x = weight;
				break;
			case 2:
				vertices[id]._boneIds1.y = i;
				vertices[id]._boneWeights1.y = weight;
				break;
			case 3:
				vertices[id]._boneIds1.z = i;
				vertices[id]._boneWeights1.z = weight;
				break;
			case 4:
				vertices[id]._boneIds1.w = i;
				vertices[id]._boneWeights1.w = weight;
				break;
			case 5:
				vertices[id]._boneIds2.x = i;
				vertices[id]._boneWeights2.x = weight;
				break;
			case 6:
				vertices[id]._boneIds2.y = i;
				vertices[id]._boneWeights2.y = weight;
				break;
			case 7:
				vertices[id]._boneIds2.z = i;
				vertices[id]._boneWeights2.z = weight;
				break;
			case 8:
				vertices[id]._boneIds2.w = i;
				vertices[id]._boneWeights2.w = weight;
				break;
			default:
			{
				assert(0);
			}
			break;
			}
		}
	}

	indices.reserve(mesh->mNumFaces);
	static vector<int> indexNum;

	bool isTri = true;
	for (UINT i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];

		indexNum.push_back(face.mNumIndices);
		for (UINT j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
		isTri = (face.mNumIndices != 2);
	}

	std::vector<Texture> textures;
	aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
	std::vector<Texture> diffuseTextures = LoadMaterialTextures(material, aiTextureType::aiTextureType_DIFFUSE, scene);
	textures.insert(textures.end(), diffuseTextures.begin(), diffuseTextures.end());
	Mesh ret(_device, _deviceContext, vertices, indices, textures, transformMatrix, isTri);
	return ret;
}

//animation
void Model::SetAnimBoneTransform(float animationTime)
{
	static float pretick = 0;
	float tick = animationTime * 0.001f * _animations[_animIndex].tickPerSecond;
	tick = fmod(tick, _animations[_animIndex].duration);
	if (pretick > tick)
	{
		_animIndex++;
		if (_animations.size() == _animIndex)
		{
			_animIndex = 0;
		}
	}
	ProcessNodeAnim(tick, &_rootNode, _axisMatrix);
	pretick = tick;
}

void Model::ProcessNodeAnim(float tick, NodeData* animData, const XMMATRIX& parentTransform)
{
	NodeAnimData nodeAnimData = _animations[_animIndex].nodeAnimDatas[animData->name];

	XMMATRIX nodeTransformMatrix = animData->transfomation;
	CalNodeTransformMatrix(&nodeAnimData, nodeTransformMatrix, tick);

	XMMATRIX globalTransform = nodeTransformMatrix * parentTransform;


	if (_boneInfo.find(animData->name) != _boneInfo.end())
	{
		XMMATRIX finalTransform = _boneInfo[animData->name].second * globalTransform;
		_currentBone[_boneInfo[animData->name].first] = finalTransform;
	}
	size_t childNum = animData->childs.size();
	for (size_t i = 0; i < childNum; i++)
	{
		ProcessNodeAnim(tick, &animData->childs[i], globalTransform);
	}
}
void Model::CalNodeTransformMatrix(NodeAnimData* nodeAnim, XMMATRIX& nodeTransform, float tick)
{
	if (nodeAnim && !nodeAnim->scalingKeys.empty() && !nodeAnim->rotationKeys.empty() && !nodeAnim->positionKeys.empty())
	{
		aiVector3D scaling;
		aiQuaternion rotationQ;
		aiVector3D translation;
		CalcInterpolatedValueFromKey(tick, nodeAnim, scaling, rotationQ, translation);
		XMMATRIX scalingM = XMMatrixScaling(scaling.x, scaling.y, scaling.z);
		XMMATRIX rotationM = XMMatrixRotationQuaternion({ rotationQ.x, rotationQ.y, rotationQ.z, rotationQ.w });
		XMMATRIX translationM = XMMatrixTranslation(translation.x, translation.y, translation.z);
		nodeTransform = scalingM * rotationM * translationM;
	}
}
void Model::CalcInterpolatedValueFromKey(float animationTime, NodeAnimData* nodeAnimData, aiVector3D& scaling, aiQuaternion& rotationQ, aiVector3D& translation)
{
	size_t numKeys = nodeAnimData->scalingKeys.size();
	if (numKeys == 1)
	{
		scaling = nodeAnimData->scalingKeys[0].vectorData;
	}
	else
	{
		unsigned int keyIndex = FindKeyIndex(animationTime, nodeAnimData->scalingKeys);
		unsigned int nextKeyIndex = keyIndex + 1;

		if (nextKeyIndex < numKeys)
		{
			float deltaTime = nodeAnimData->scalingKeys[nextKeyIndex].time - nodeAnimData->scalingKeys[keyIndex].time;
			float factor = (animationTime - nodeAnimData->scalingKeys[keyIndex].time) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);

			const aiVector3D& startValue = nodeAnimData->scalingKeys[keyIndex].vectorData;
			const aiVector3D& endValue = nodeAnimData->scalingKeys[nextKeyIndex].vectorData;

			scaling.x = startValue.x + (endValue.x - startValue.x) * factor;
			scaling.y = startValue.y + (endValue.y - startValue.y) * factor;
			scaling.z = startValue.z + (endValue.z - startValue.z) * factor;
		}
		else
		{
			assert(0);
		}
	}

	numKeys = nodeAnimData->rotationKeys.size();
	if (numKeys == 1)
	{
		rotationQ = nodeAnimData->rotationKeys[0].quaterData;
	}
	else
	{
		unsigned int keyIndex = FindKeyIndex(animationTime, nodeAnimData->rotationKeys);
		unsigned int nextKeyIndex = keyIndex + 1;

		if (nextKeyIndex < numKeys)
		{
			float deltaTime = nodeAnimData->rotationKeys[nextKeyIndex].time - nodeAnimData->rotationKeys[keyIndex].time;
			float factor = (animationTime - nodeAnimData->rotationKeys[keyIndex].time) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);

			const aiQuaternion& startValue = nodeAnimData->rotationKeys[keyIndex].quaterData;
			const aiQuaternion& endValue = nodeAnimData->rotationKeys[nextKeyIndex].quaterData;
			aiQuaternion::Interpolate(rotationQ, startValue, endValue, factor);
		}
		else
		{
			assert(0);
		}
		rotationQ = rotationQ.Normalize();
	}

	numKeys = nodeAnimData->positionKeys.size();
	if (numKeys == 1)
	{
		translation = nodeAnimData->positionKeys[0].vectorData;
	}
	else
	{
		unsigned int keyIndex = FindKeyIndex(animationTime, nodeAnimData->positionKeys);
		unsigned int nextKeyIndex = keyIndex + 1;

		if (nextKeyIndex < numKeys)
		{
			float deltaTime = nodeAnimData->positionKeys[nextKeyIndex].time - nodeAnimData->positionKeys[keyIndex].time;
			float factor = (animationTime - nodeAnimData->positionKeys[keyIndex].time) / deltaTime;

			assert(factor >= 0.0f && factor <= 1.0f);

			const aiVector3D& startValue = nodeAnimData->positionKeys[keyIndex].vectorData;
			const aiVector3D& endValue = nodeAnimData->positionKeys[nextKeyIndex].vectorData;

			translation.x = startValue.x + (endValue.x - startValue.x) * factor;
			translation.y = startValue.y + (endValue.y - startValue.y) * factor;
			translation.z = startValue.z + (endValue.z - startValue.z) * factor;
		}
		else
		{
			assert(0);
		}
	}
}
unsigned int Model::FindKeyIndex(const float animationTime, vector<KeyVectorData>& vectorDatas)
{
	size_t numKeys = vectorDatas.size();
	assert(numKeys > 0);
	for (size_t i = 0; i < numKeys - 1; i++)
		if (animationTime < (float)vectorDatas[i + 1].time)
			return i;
	return numKeys - 1;
}
unsigned int Model::FindKeyIndex(const float animationTime, vector<KeyQuaterData>& quaterDatas)
{
	size_t numKeys = quaterDatas.size();
	assert(numKeys > 0);
	for (size_t i = 0; i < numKeys - 1; i++)
		if (animationTime < quaterDatas[i + 1].time)
			return i;
	return numKeys - 1;
}

//texture
TextureStorageType Model::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

std::vector<Texture> Model::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storetype = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0) //If there are no textures
	{
		storetype = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(_device, Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(_device, Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storetype = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storetype)
			{
			case TextureStorageType::EmbeddedIndexCompressed:
			{
				int index = GetTextureIndex(&path);
				Texture embeddedIndexedTexture(_device,
					reinterpret_cast<uint8_t*>(pScene->mTextures[index]->pcData),
					pScene->mTextures[index]->mWidth,
					textureType);
				materialTextures.push_back(embeddedIndexedTexture);
				break;
			}
			case TextureStorageType::EmbeddedCompressed:
			{
				const aiTexture* pTexture = pScene->GetEmbeddedTexture(path.C_Str());
				Texture embeddedTexture(_device,
					reinterpret_cast<uint8_t*>(pTexture->pcData),
					pTexture->mWidth,
					textureType);
				materialTextures.push_back(embeddedTexture);
				break;
			}
			case TextureStorageType::Disk:
			{
				std::string filename = _directory + '\\' + path.C_Str();
				Texture diskTexture(_device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}
	if (materialTextures.size() == 0)
	{
		materialTextures.push_back(Texture(_device, Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

int Model::GetTextureIndex(aiString* pStr)
{
	assert(pStr->length >= 2);
	return atoi(&pStr->C_Str()[1]);
}




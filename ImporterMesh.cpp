#include "ImporterMesh.h"

#include "Application.h"

#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

void ImporterMesh::ImportFBX(const char* filePath)
{
	bool result = false;

	char* fileBuffer;
	unsigned lenghBuffer = App->fileSystem->Load(filePath, &fileBuffer);
	const aiScene* scene = aiImportFileFromMemory(fileBuffer, lenghBuffer, aiProcessPreset_TargetRealtime_MaxQuality, "");

	std::string fileName;
	App->fileSystem->SplitFilePath(filePath, nullptr, &fileName, nullptr);

	if (scene->mMeshes != nullptr)
	{
		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			std::string meshName = fileName;
			meshName.append("_" + std::to_string(i));
			Import(scene->mMeshes[i], meshName.c_str());
		}
	}
}

bool ImporterMesh::Import(const aiMesh* aiMesh, const char* meshName)
{
	bool result = false;

	if (aiMesh == nullptr)
		return result;

	Mesh meshStruct;

	// vertices
	meshStruct.verticesNumber = aiMesh->mNumVertices;
	meshStruct.vertices = new float[meshStruct.verticesNumber * 3];
	memcpy(meshStruct.vertices, aiMesh->mVertices, sizeof(float) * meshStruct.verticesNumber * 3);

	// indices
	if (aiMesh->HasFaces())
	{
		meshStruct.indicesNumber = aiMesh->mNumFaces * 3;
		meshStruct.indices = new unsigned[meshStruct.indicesNumber];
		for (unsigned i = 0; i < aiMesh->mNumFaces; ++i)
		{
			memcpy(&meshStruct.indices[i * 3], aiMesh->mFaces[i].mIndices, 3 * sizeof(unsigned));
		}
	}

	// uvs
	if (aiMesh->HasTextureCoords(0))
	{
		meshStruct.uvs = new float[meshStruct.verticesNumber * 2];
		int uvsCount = 0;
		for (unsigned i = 0; i < meshStruct.verticesNumber; i++)
		{
			meshStruct.uvs[uvsCount] = aiMesh->mTextureCoords[0][i].x;
			uvsCount++;
			meshStruct.uvs[uvsCount] = aiMesh->mTextureCoords[0][i].y;
			uvsCount++;
		}
	}

	// normals
	if (aiMesh->HasNormals())
	{
		meshStruct.normals = new float[meshStruct.verticesNumber * 3];
		memcpy(meshStruct.normals, aiMesh->mNormals, sizeof(float) * meshStruct.verticesNumber * 3);
	}

	// colors
	if (aiMesh->HasVertexColors(0))
	{
		meshStruct.colors = new float[meshStruct.verticesNumber * 3];
		memcpy(meshStruct.colors, aiMesh->mColors, sizeof(float) * meshStruct.verticesNumber * 3);
	}

	return Save(meshStruct, meshName);
}

bool ImporterMesh::Load(Mesh* meshStruct, const char* meshName)
{
	bool result = false;

	if (meshStruct == nullptr)
		return result;

	char* buffer;
	std::string meshPath("/Library/Meshes/");
	meshPath.append(meshName);
	unsigned size = App->fileSystem->Load(meshPath.c_str(),  &buffer);

	if (buffer != nullptr && size > 0)
	{
		char* cursor = buffer;

		// indices / vertices / uvs / normals/ colors / 
		unsigned ranges[5];
		unsigned bytes = sizeof(ranges);
		memcpy(ranges, cursor, bytes);
		cursor += bytes;

		meshStruct->indicesNumber = ranges[0];
		meshStruct->verticesNumber = ranges[1];

		// indices
		bytes = sizeof(unsigned) * meshStruct->indicesNumber;
		meshStruct->indices = new unsigned[meshStruct->indicesNumber];
		memcpy(meshStruct->indices, cursor, bytes);
		cursor += bytes;

		// vertices
		bytes = sizeof(float) * meshStruct->verticesNumber * 3;
		meshStruct->vertices = new float[meshStruct->verticesNumber * 3];
		memcpy(meshStruct->vertices, cursor, bytes);
		cursor += bytes;

		//uvs
		if (ranges[2] > 0)
		{
			bytes = sizeof(float)* meshStruct->verticesNumber * 2;
			meshStruct->uvs = new float[meshStruct->verticesNumber * 2];
			memcpy(meshStruct->uvs, cursor, bytes);
			cursor += bytes;
		}

		//Load normals
		if (ranges[3] > 0)
		{
			bytes = sizeof(float)* meshStruct->verticesNumber * 3;
			meshStruct->normals = new float[meshStruct->verticesNumber * 3];
			memcpy(meshStruct->normals, cursor, bytes);
			cursor += bytes;
		}

		// Load colors
		if (ranges[4] > 0)
		{
			bytes = sizeof(float)* meshStruct->verticesNumber * 3;
			meshStruct->colors = new float[meshStruct->verticesNumber * 3];
			memcpy(meshStruct->colors, cursor, bytes);
			cursor += bytes;
		}

		RELEASE_ARRAY(buffer);
		
		result = true;
	}

	return result;
}

bool ImporterMesh::Save(const Mesh& mesh, const char* meshName)
{
	bool result = false;

	// indices / vertices / uvs / normals/ colors / 
	unsigned ranges[5] = {
		mesh.indicesNumber,
		mesh.verticesNumber,
		(mesh.uvs) ? mesh.verticesNumber : 0,
		(mesh.normals) ? mesh.verticesNumber : 0,
		(mesh.colors) ? mesh.verticesNumber : 0
	};

	unsigned size = sizeof(ranges);
	size += sizeof(unsigned) * mesh.indicesNumber;
	size += sizeof(float) * mesh.verticesNumber * 3;
	if (mesh.uvs)
		size += sizeof(float) * mesh.verticesNumber * 2;
	if (mesh.normals)
		size += sizeof(float) * mesh.verticesNumber * 3;
	if (mesh.colors)
		size += sizeof(float) * mesh.verticesNumber * 3;

	// allocate and fill
	char* data = new char[size];
	char* cursor = data;

	// First store ranges
	unsigned bytes = sizeof(ranges);
	memcpy(cursor, ranges, bytes);
	cursor += bytes;

	// Store indices
	bytes = sizeof(unsigned) * mesh.indicesNumber;
	memcpy(cursor, mesh.indices, bytes);
	cursor += bytes;

	// Store vertices
	bytes = sizeof(float) * mesh.verticesNumber * 3;
	memcpy(cursor, mesh.vertices, bytes);
	cursor += bytes;

	// Store uvs
	if (mesh.uvs != nullptr)
	{
		bytes = sizeof(float)* mesh.verticesNumber  * 2;
		memcpy(cursor, mesh.uvs, bytes);
		cursor += bytes;
	}

	// Store normals
	if (mesh.normals != nullptr)
	{
		bytes = sizeof(float)* mesh.verticesNumber * 3;
		memcpy(cursor, mesh.normals, bytes);
		cursor += bytes;
	}

	// Store colors
	if (mesh.colors != nullptr)
	{
		bytes = sizeof(float)* mesh.verticesNumber * 3;
		memcpy(cursor, mesh.colors, bytes);
		cursor += bytes;
	}
	
	std::string fileToSave("/Library/Meshes/");

	fileToSave.append(meshName);
	fileToSave.append(".chamo");

	result = App->fileSystem->Save(fileToSave.c_str(), data, size, false);

	RELEASE_ARRAY(data);

	return result;
}
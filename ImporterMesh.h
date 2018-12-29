#ifndef _ImporterMesh_h
#define _ImporterMesh_h

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "MathGeoLib.h"

#include <string>
#include <vector>

struct aiScene;
struct aiNode;

struct Mesh;

class ImporterMesh
{
public:
	static void ImportFBX(const char* filePath);
	static bool Import(const aiMesh* aiMesh, const char* meshName);
	static bool Load(Mesh* mesh, const char* meshName);
	static bool Save(const Mesh& mesh, const char* meshName);

};

#endif //_ImporterMesh_h
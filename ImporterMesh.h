#ifndef _ImporterMesh_h
#define _ImporterMesh_h

#include <assimp/scene.h>
#include <assimp/cimport.h>
#include <assimp/postprocess.h>

#include "MathGeoLib.h"

#include <string>
#include <vector>
#include <map>
#include <utility>

struct aiScene;
struct aiNode;

struct Mesh;

class ImporterMesh
{
public:
	static void ImportFBX(const char* filePath);
	static bool Import(const aiMesh* aiMesh, const char* meshName);
	static bool Load(Mesh* & mesh, char* meshName);
	static bool Save(const Mesh& mesh, const char* meshName);
	static void RemoveMesh(const char* meshName);
private:
	static void CleanUpStructMesh(Mesh* mesh);
private:
	static std::map<const char*, std::pair<Mesh*, unsigned>> mapMeshUsed;
};

#endif //_ImporterMesh_h
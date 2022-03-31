

union triangle_index
{
	s32 E[3];
	struct
	{
		s32 Vertex;
		s32 UV;
		s32 Normal;
	};
};

enum token_type
{
	type_none,
	type_end_of_file,
	type_vertex,
	type_normal,
	type_triangle,
	type_uv
};

struct mesh_tokenizer
{
	char* At;
};

struct mesh_token
{
	token_type Type;
	s32 Length;
	char* Text;
};

struct triangle_result
{
	s32 Count;
	// TODO(Stephen): Have no idea why Blender may output more than three triangles
	//per line....
	triangle_index Triangles[4]; 
};

b32
IsEndOfFile(char c)
{
	b32 result = (c == 0);
	return(result);
}

b32
IsEndOfLine(char c)
{
	b32 result = (c == '\n' || c == '\r');
	return(result);
}

void
TokenizeLine(mesh_tokenizer* tokenizer)
{
	while(IsEndOfFile(tokenizer->At[0]) == FALSE &&
		  IsEndOfLine(tokenizer->At[0]) == FALSE)
	{
		++tokenizer->At;
	}
}

void
IgnoreLine(mesh_tokenizer* tokenizer)
{
	while(IsEndOfFile(tokenizer->At[0]) == FALSE &&
		  IsEndOfLine(tokenizer->At[0]) == FALSE)
	{
		++tokenizer->At;
	}
	++tokenizer->At;
}

mesh_token
GetNexttoken(mesh_tokenizer* tokenizer)
{
	mesh_token result = {};
	result.Length = 1;
	result.Text = tokenizer->At;
	result.Type = type_none;
	
	switch(tokenizer->At[0])
	{
		case 0: { result.Type = type_end_of_file; }break;
		case 'v':
		{
			if(tokenizer->At[1] == 'n') { result.Type = type_normal; }
			else if(tokenizer->At[1] == 't') { result.Type = type_uv; }
			else { result.Type = type_vertex; }
			TokenizeLine(tokenizer);
			result.Length = tokenizer->At - result.Text;
		}break;
		case 'f':
		{
			result.Type = type_triangle;
			TokenizeLine(tokenizer);
			result.Length = tokenizer->At - result.Text;
		}break;
		case 'o':
		case 's':
		case '#': { IgnoreLine(tokenizer); }break;
		default:
		{
			++tokenizer->At;
		}break;
	}
	
	
	return(result);
}

inline b32
IsAlphaNumeric(char c)
{
	b32 result = ('0' <= c && c <= '9');
	return(result);
}

inline b32
IsAlphaSign(char c)
{
	b32 result = ('-' == c);
	return(result);
}

// NOTE(Stephen): The precision of this conversion appears to work supprisignly well.
//I suppose because we within 6 signifiant digits
f32
ConvertStringToF32(char* text, f32 length)
{
	f32 result = 0;
    s32 currentValue = 0;
    s32 division = 1;
    b32 hasDecimal = FALSE;
    b32 isNegative = FALSE;
    
    for(s32 i = 0; i < length; ++i)
    {
        if(text[i] == '.')
        {
            hasDecimal = TRUE;
        }
        else if(text[i] == '-')
        {
            isNegative = TRUE;
        }
        else
        {
            if(hasDecimal)
                division *= 10;
            
            currentValue *= 10;
            currentValue += (text[i] - '0');
        }
    }
    
    result = currentValue / (f32)division;
    
    if(isNegative)
        result *= -1.0f;
    
    return(result);
}

s32
ConvertStringToS32(char* text, s32 length)
{
    s32 result = 0;
    s32 currentValue = 0;
    b32 isNegative = FALSE;
    
	for(s32 i = 0; i < length; ++i)
    {
        if(text[i] == '-')
        {
            isNegative = TRUE;
        }
        else
        {
            currentValue *= 10;
            currentValue += (text[i] - '0');
        }
        
    }
    
    result = currentValue;
    
    if(isNegative)
        result *= -1;
    
    return(result);
}

v3
ParseVertex(mesh_token token)
{
	v3 result = {};
	s32 coordinate = 0;
	for(s32 i = 0; i < token.Length; ++i)
	{
		char c = token.Text[i];
		b32 isSigned = IsAlphaSign(c);
		b32 isAlphaNumeric = IsAlphaNumeric(c);
		
		if(isAlphaNumeric || isSigned)
		{
			char buffer[16] = {};
			s32 bufferIndex = 0;
			for(; i < token.Length; ++i)
			{
				if(token.Text[i] == ' ') { break; }
				buffer[bufferIndex++] = token.Text[i];
			}
			
			result.E[coordinate] = ConvertStringToF32(buffer, bufferIndex);
			++coordinate;
		}
	}
	
	return(result);
}

triangle_result
ParseTriangle(mesh_token token)
{
	triangle_result result = {};
	s32 attribute = 0;
	result.Count = 0;
	for(s32 i = 0; i < token.Length; ++i)
	{
		char c = token.Text[i];
		b32 isAlphaNumeric = IsAlphaNumeric(c);
		
		if(isAlphaNumeric)
		{
			char buffer[16] = {};
			s32 bufferIndex = 0;
			for(; i < token.Length; ++i)
			{
				if(token.Text[i] == ' ' || token.Text[i] == '/') { break; }
				buffer[bufferIndex++] = token.Text[i];
			}
			
			Assert(result.Count < sizeof(result.Triangles));
			// NOTE(Stephen): Blender outputs triangle indices with base 1 - minus one for base 0
			result.Triangles[result.Count].E[attribute] = ConvertStringToS32(buffer, bufferIndex) - 1;
			attribute = (attribute + 1) % 3 ;
			
			if(token.Text[i] == ' ' || IsEndOfLine(token.Text[i]))
				++result.Count;
		}
	}
	return(result);
}

union vertex_attribute
{
	f32 E[8];
	struct
	{
		v3 Vertex;
		v3 Normal;
		v2 UV;
	};
};

void
ConstructVertexAttributes(memory_block* vertexMemory, memory_block* triangleMemory, 
						  memory_block* uvMemory, memory_block* normalMemory, 
						  memory_block* vertexAttributesMemory)
{
	s32 triangleCount = triangleMemory->Used / sizeof(triangle_index);
	triangle_index* triangles = (triangle_index*)triangleMemory->Base;
	v3* vertices = (v3*)vertexMemory->Base;
	v2* uvs = (v2*)uvMemory->Base;
	v3* normals = (v3*)normalMemory->Base;
	
	for(s32 index = 0; index < triangleCount; index += 3)
	{
		triangle_index* index0 = triangles + index + 0;
		triangle_index* index1 = triangles + index + 1;
		triangle_index* index2 = triangles + index + 2;
		
		vertex_attribute* attributes = MemoryBlock_PushArray(vertexAttributesMemory, 3, vertex_attribute);
		(attributes + 0)->Vertex = vertices[index0->Vertex];
		(attributes + 0)->Normal = normals[index0->Normal];
		(attributes + 0)->UV = uvs[index0->UV];
		
		(attributes + 1)->Vertex = vertices[index1->Vertex];
		(attributes + 1)->Normal = normals[index1->Normal];
		(attributes + 1)->UV = uvs[index1->UV];
		
		(attributes + 2)->Vertex = vertices[index2->Vertex];
		(attributes + 2)->Normal = normals[index2->Normal];
		(attributes + 2)->UV = uvs[index2->UV];
	}
}

void
ComputeTangentVectors(memory_block* tangentMemory, memory_block* vertexAttributesMemory)
{
	
	vertex_attribute* attributes = (vertex_attribute*)vertexAttributesMemory->Base;
	s32 vertexCount = vertexAttributesMemory->Used / sizeof(vertex_attribute);
	v3* tangents = (v3*)calloc(1, (sizeof(v3) * vertexCount) * 2);
	v3* bitangents = (v3*)tangents + vertexCount;
	
	for(s32 i = 0; i < vertexCount; i += 3)
	{
		v3 vertex0 = attributes[i + 0].Vertex;
		v3 vertex1 = attributes[i + 1].Vertex;
		v3 vertex2 = attributes[i + 2].Vertex;
		v2 uv0 = attributes[i + 0].UV;
		v2 uv1 = attributes[i + 1].UV;
		v2 uv2 = attributes[i + 2].UV;
		
		v3 edge0 = vertex1 - vertex0;
		v3 edge1 = vertex2 - vertex0;
		
		f32 x0 = uv1.X - uv0.X;
		f32 x1 = uv2.X - uv0.X;
		f32 y0 = uv1.Y - uv0.Y;
		f32 y1 = uv2.Y - uv0.Y;
		
		f32 f = 1.0f / (x0 * y1 - x1 * y0);
		
		v3 tangent;
		tangent.X = f * (y1 * edge0.X - y0 * edge1.X);
		tangent.Y = f * (y1 * edge0.Y - y0 * edge1.Y);
		tangent.Z = f * (y1 * edge0.Z - y0 * edge1.Z);
		
		v3 bitangent;
		bitangent.X = f * (x0 * edge1.X - x1 * edge0.X);
		bitangent.Y = f * (x0 * edge1.Y - x1 * edge0.Y);
		bitangent.Z = f * (x0 * edge1.Z - x1 * edge0.Z);
		
		tangents[i + 0] += tangent;
		tangents[i + 1] += tangent;
		tangents[i + 2] += tangent;
		
		bitangents[i + 0] += bitangent;
		bitangents[i + 1] += bitangent;
		bitangents[i + 2] += bitangent;
	}
	
	for(s32 i = 0; i < vertexCount; ++i)
	{
		v3 t = tangents[i];
		v3 b = bitangents[i];
		v3 n = attributes[i].Normal;
		
		v4* outTangent = MemoryBlock_PushStruct(tangentMemory, v4);
		outTangent->XYZ = Math_NormalizedV3((t - n * Math_DotProductV3(n, t)));
		outTangent->W = (Math_DotProductV3(Math_CrossProductV3(t, b), n) > 0.0f) ? 1.0f : -1.0f;
	}
	
	free(tangents);
}

void
WriteEngineMesh(memory_block* vertexAttributesMemory, memory_block* tangentMemory, 
				char* inFileName)
{
	char path[64] = {};
	char fileName[16] = {};
	ChangeFileExtension(fileName, inFileName, ENGINE_MESH_EXT);
	GetLocalFileNamePath(ASSET_MESH_DIRECTORY, path, fileName);
	
	loadable_mesh mesh;
	mesh.VertexCount = vertexAttributesMemory->Used / sizeof(vertex_attribute);
	mesh.TangentOffset = sizeof(loadable_mesh) + vertexAttributesMemory->Used;
	
	WriteFile(path, sizeof(loadable_mesh), &mesh, "w");
	WriteFile(path, vertexAttributesMemory->Used, vertexAttributesMemory->Base, "a");
	WriteFile(path, tangentMemory->Used, tangentMemory->Base, "a");
}

void
ProcessAllWaveFrontOBJs(working_directory workingDirectory)
{
	char* memory = (char*)calloc(1, MegaBytes(6));
	memory_block vertexMemory;
	memory_block triangleMemory;
	memory_block uvMemory;
	memory_block normalMemory;
	memory_block tangentMemory;
	memory_block vertexAttributesMemory;
	MemoryBlock_Initialize(&uvMemory, MegaBytes(1), memory);
	MemoryBlock_Initialize(&normalMemory, MegaBytes(1), memory + MegaBytes(1));
	MemoryBlock_Initialize(&vertexMemory, MegaBytes(1), memory +  + MegaBytes(2));
	MemoryBlock_Initialize(&triangleMemory, MegaBytes(1), memory + MegaBytes(3));
	MemoryBlock_Initialize(&tangentMemory, MegaBytes(1), memory + MegaBytes(4));
	MemoryBlock_Initialize(&vertexAttributesMemory, MegaBytes(1), memory + MegaBytes(5));
	
	char filePathBuffer[128] = {};
	char directoryBuffer[256] = {};
	GetLocalDirectoryPath(directoryBuffer, RAW_MESH_FOLDER, workingDirectory);
	DIR* meshDirectory = opendir(directoryBuffer);
	
	if(meshDirectory)
	{
		struct dirent* directoryEntity;
		while((directoryEntity = readdir(meshDirectory)) != 0)
		{
			if(directoryEntity->d_name[0] != '.')
			{
				MemoryBlock_Reset(&normalMemory);
				MemoryBlock_Reset(&uvMemory);
				MemoryBlock_Reset(&vertexMemory);
				MemoryBlock_Reset(&triangleMemory);
				MemoryBlock_Reset(&tangentMemory);
				MemoryBlock_Reset(&vertexAttributesMemory);
				
				GetLocalFileNamePath(directoryBuffer, filePathBuffer, directoryEntity->d_name);
				
				raw_file file = ReadTextFileAndNullTerminate(filePathBuffer);
				mesh_tokenizer tokenizer = { (char*)file.Contents };
				b32 parsing = file.IsValid;
				
				while(parsing)
				{
					mesh_token token = GetNexttoken(&tokenizer);
					
					switch(token.Type)
					{
						case type_end_of_file:
						{
							parsing = FALSE;
						}break;
						case type_vertex:
						{
							v3* vertex = MemoryBlock_PushStruct(&vertexMemory, v3);
							*vertex = ParseVertex(token);
#if 0							
							printf("---------------\n");
							printf("%.*s\n", token.Length, token.Text);
							printf("v %f %f %f\n", vertex->X, vertex->Y, vertex->Z);
							printf("---------------\n");
#endif
						}break;
						case type_triangle:
						{
							triangle_result triangles = ParseTriangle(token);
							triangle_index* indices = MemoryBlock_PushArray(&triangleMemory, 
																			triangles.Count, 
																			triangle_index);
							for(s32 i = 0; i < triangles.Count; ++i)
							{
								indices[i].Vertex = triangles.Triangles[i].Vertex;
								indices[i].UV = triangles.Triangles[i].UV;
								indices[i].Normal = triangles.Triangles[i].Normal;
							}
							
#if 0
							printf("---------------\n");
							printf("%.*s\n", token.Length, token.Text);
							printf("f ");
							for(s32 i = 0; i < triangles.Count; ++i)
							{
								triangle_index index = triangles.Triangles[i];
								printf("%d/%d/%d ", index.Vertex, index.UV, index.Normal);
							}
							printf("\n---------------\n");
#endif
						}break;
						case type_normal:
						{
							v3* normal = MemoryBlock_PushStruct(&normalMemory, v3);
							*normal = ParseVertex(token);
							
#if 0							
							printf("---------------\n");
							printf("%.*s\n", token.Length, token.Text);
							printf("vn %f %f %f\n", normal->X, normal->Y, normal->Z);
							printf("---------------\n");
#endif
						}break;
						case type_uv:
						{
							v2* uv = MemoryBlock_PushStruct(&uvMemory, v2);
							*uv = ParseVertex(token).XY;
#if 0
							printf("---------------\n");
							printf("%.*s\n", token.Length, token.Text);
							printf("vt %f %f\n", uv->X, uv->Y);
							printf("---------------\n");
#endif
						}break;
						EmptyDefaultCase;
					}
				}//while(parsing)
				
				FreeRawFile(&file);
				
				
				ConstructVertexAttributes(&vertexMemory, &triangleMemory, &uvMemory, 
										  &normalMemory, &vertexAttributesMemory);
				ComputeTangentVectors(&tangentMemory, &vertexAttributesMemory);
				WriteEngineMesh(&vertexAttributesMemory, &tangentMemory, 
								directoryEntity->d_name);
				
				
			}//if(directoryEntity->d_name[0] != '.')
		}//while((directoryEntity = readdir(meshDirectory)) != 0)
		closedir(meshDirectory);
	}//if(meshDirectory)
}

/*

Material

*/
#include "material.h"
#include "Cache/resource_cache.h"
#include "Renderer/texture.h"

#include <iostream>
#include <sstream>

namespace NYX {

bool Material::ImportSingleMaterialFromMemory(char *matlib)
{
	int illum = 0;
	int numMaterials = 0;
	char buffer[256] = {0};

	//line endings are controlled by perforce and are based on the local system,
	//so we can safely assume that under windows all input files will have windows EOL
	//and under Unix will have Unix EOL
#ifdef __WIN32__
	char new_line = '\r';
#else
	char new_line = '\n';
#endif

	// on osx, assigning a const char* to the stringstream using the assignement operator << randomly crashes on assignement. 
    // reason not yet known.
#if defined (__MACOSX__)
    string tmp(matlib);
    stringstream memstream(tmp, ios::in | ios::out);
#else
    stringstream memstream( stringstream::in | stringstream::out );
    memstream << matlib;
#endif

	// Load the materials in the MTL file.
	while ( memstream.good() )
	{
		memstream >> buffer;

		switch (buffer[0])
		{
		case 'N': // Ns
			memstream >> mShininess;

			// Wavefront .MTL file shininess is from [0,1000].
			// Scale back to a generic [0,1] range.
			mShininess /= 10.0f;

			if(memstream.peek() == new_line)
			{
				//throw away new line char
				memstream.get(buffer[0]);
#ifdef __WIN32__
				//note that on win we just threw away '\r', but
				//we still need to throw away '\n'
				memstream.get(buffer[0]);
#endif
			}
			break;

		case 'K': // Ka, Kd, or Ks
			switch (buffer[1])
			{
			case 'a': // Ka
				memstream >> mAmbient[0] >>
						     mAmbient[1] >>
						     mAmbient[2];
				mAmbient[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'd': // Kd
				memstream >> mDiffuse[0] >>
							 mDiffuse[1] >>
							 mDiffuse[2];
				mDiffuse[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 's': // Ks
				memstream >> mSpecular[0] >>
						     mSpecular[1] >>
						     mSpecular[2];
				mSpecular[3] = 1.0f;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			default:
				memstream.getline(buffer, 255);
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
			}
			break;

			case 'T': // Tr
				switch (buffer[1])
				{
				case 'r': // Tr
					memstream >> mAlpha;
					mAlpha = 1.0 - mAlpha;
					if(memstream.peek() == new_line)
					{
						//throw away new line char
						memstream.get(buffer[0]);
#ifdef __WIN32__
						//note that on win we just threw away '\r', but
						//we still need to throw away '\n'
						memstream.get(buffer[0]);
#endif
					}
					break;

				default:
					memstream.getline(buffer, 255);
					if(memstream.peek() == new_line)
					{
						//throw away new line char
						memstream.get(buffer[0]);
#ifdef __WIN32__
						//note that on win we just threw away '\r', but
						//we still need to throw away '\n'
						memstream.get(buffer[0]);
#endif
					}
					break;
				}
				break;

			case 'd':
				memstream >> mAlpha;
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'i': // illum
				memstream >> illum;

				if (illum == 1)
				{
					mSpecular[0] = 0.0;
					mSpecular[1] = 0.0;
					mSpecular[2] = 0.0;
					mSpecular[3] = 1.0;
				}

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'm': // map_Kd, map_bump
				if (strstr(buffer, "map_Kd") != 0)
				{
					memstream >> buffer;
					ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(buffer, RES_TEXTURE);
					ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
					mDiffuseTextures.push_back(himg->GetTexture());
				}
				else if (strstr(buffer, "map_bump") != 0)
				{
					memstream >> buffer;
					ResourcePtr hres = ResourceCache::GetInstance()->RequestResource(buffer, RES_TEXTURE);
					ImageResource *himg = dynamic_cast<ImageResource*>(hres.get());
					mBumpTextures.push_back(himg->GetTexture());
				} //TODO: add other texture types
				else
				{
					memstream.getline(buffer, 255);
				}

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;

			case 'n': // newmtl

				if (numMaterials == 1)
				{
					memstream.clear();
					delete matlib;
					return true;
				}

				memstream >> buffer;

				mAmbient[0] = 0.2;
				mAmbient[1] = 0.2;
				mAmbient[2] = 0.2;
				mAmbient[3] = 1.0;
				mDiffuse[0] = 0.8;
				mDiffuse[1] = 0.8;
				mDiffuse[2] = 0.8;
				mDiffuse[3] = 1.0;
				mSpecular[0] = 0.0;
				mSpecular[1] = 0.0;
				mSpecular[2] = 0.0;
				mSpecular[3] = 1.0;
				mShininess = 0.0;
				mAlpha = 1.0;
				mName = buffer;
				mDiffuseTextures.clear();
				mBumpTextures.clear();

				++numMaterials;

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
			
			case 'S': // shader pkg name
			
				memstream >> buffer;

				mShaderName = string(buffer);

				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}

				break;

			default:
				memstream.getline(buffer, 255);
				if(memstream.peek() == new_line)
				{
					//throw away new line char
					memstream.get(buffer[0]);
#ifdef __WIN32__
					//note that on win we just threw away '\r', but
					//we still need to throw away '\n'
					memstream.get(buffer[0]);
#endif
				}
				break;
		}
	}

	memstream.clear();
	delete matlib;

	return true;
}

}

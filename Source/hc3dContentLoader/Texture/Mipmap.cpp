#include "stdafx.h"
#include "Mipmap.h"
//#include <Texture\CImg.h>

using namespace hc3d;
//using namespace cimg_library;

Mipmap::Mipmap()
{
}

std::pair<int, int> Mipmap::LoadPng(png_bytep &buffer, char* fileName) {
	png_image image;

	std::pair<int, int> ret;

	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, fileName))
	{

		image.format = PNG_FORMAT_RGBA;
		ret.first = image.width;
		ret.second = image.height;
		buffer = (png_bytep) malloc(PNG_IMAGE_SIZE(image));

		png_image_finish_read(&image, NULL/*background*/, buffer,
			0/*row_stride*/, NULL/*colormap*/);
	}
	return ret;
}

GLuint Mipmap::LoadImage(IN _bstr_t& fileName) {
		glEnable(GL_TEXTURE_2D);
	  	float g_nMaxAnisotropy[1];

		std::cout << "loading image : " << fileName << std::endl;
		
		png_bytep buffer;
		std::pair<int, int> sizes = LoadPng(buffer, fileName);
		width = sizes.first;
		height = sizes.second;

 		if(!width || !height) throw std::runtime_error(std::string("Can not load '") + std::string(fileName) + std::string("'\n"));
		glGenTextures(1,&tex);
		glBindTexture(GL_TEXTURE_2D,tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

		std::cout << width << std::endl;
		std::cout << height << std::endl;

	//	glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,8);
		gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,width,height,GL_RGBA,GL_UNSIGNED_BYTE,buffer);
        glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT,g_nMaxAnisotropy);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
	        GL_LINEAR_MIPMAP_LINEAR);
		glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAX_ANISOTROPY_EXT,g_nMaxAnisotropy[0]);
		std::cout << "Anisotropy max: "<< g_nMaxAnisotropy[0] << std::endl;
		return tex;
}

GLuint Mipmap::LoadImage(GLint format,int size) {
		glEnable(GL_TEXTURE_2D);
		glGenTextures(1,&tex);
		glBindTexture(GL_TEXTURE_2D,tex);
		glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
		if(format == GL_RGBA) {
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
		}
		else {
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST );
			glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
		}
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
	
	
		if(format == GL_DEPTH_COMPONENT) glTexImage2D     ( GL_TEXTURE_2D, 0, format, size,size, 0,
	                    format, GL_UNSIGNED_BYTE, NULL);
		else glTexImage2D     ( GL_TEXTURE_2D, 0, format, size,size, 0,
	                    format, GL_FLOAT, NULL);
	     return tex;
}

  
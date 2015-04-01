#include "stdafx.h"
#include "Shader.h"

using namespace hc3d;

Shader::Shader(void) 
{
	
}


GLuint Shader::LoadShader(const _bstr_t& fileName1, const _bstr_t& fileName2)
{
	GLenum err = glewInit();
	GLcharARB *vertex_sh;
	GLcharARB *fragment_sh;
	FILE *fp1,*fp2;
	int length_f,length_v;
	fopen_s(&fp1, fileName1,"r");
	if(fp1 == 0) {
		throw std::runtime_error(std::string("FILE '") + std::string(fileName1) + std::string("' CAN NOT BE OPENED\n"));
	}
	fseek(fp1,0,SEEK_END);
	length_v = ftell(fp1);
	fseek(fp1,0,SEEK_SET);

	vertex_sh = new GLcharARB[length_v+1];
	GLuint buf;
	int i = 0;
	while(fread(&buf,1,1,fp1)) {
		vertex_sh[i] = buf;
		i++;
	}
	vertex_sh[i] = '\0';

	
	fopen_s(&fp2, fileName2,"r");
	if(fp2 == 0) {
		throw std::runtime_error(std::string("FILE '") + std::string(fileName2) + std::string("' CAN NOT BE OPENED\n"));
	}
	fseek(fp2,0,SEEK_END);
	length_f = ftell(fp2);
	fseek(fp2,0,SEEK_SET);

	fragment_sh = new GLcharARB[length_f+1];
	i = 0;
	while(fread(&buf,1,1,fp2)) {
		fragment_sh[i] = buf;
		i++;
	}
	fragment_sh[i] = '\0';

	GLuint vertexShader, fragmentShader;
	vertexShader = glCreateShader(GL_VERTEX_SHADER);
	fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	const GLcharARB* v_sh = vertex_sh;
	const GLcharARB* f_sh = fragment_sh;

	glShaderSourceARB(vertexShader, 1, &v_sh, &length_v);
	glShaderSourceARB(fragmentShader, 1, &f_sh, &length_f);

	glCompileShaderARB(vertexShader);
	glCompileShaderARB(fragmentShader);


	GLint blen = 0;
	GLsizei slen = 0;
	glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH , &blen); 
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(vertexShader, blen, &slen, compiler_log);
		std::cout << "compiling vertex shader "<< "'" << fileName1 << "' :\n" << compiler_log;
		free (compiler_log);
	}

	glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH , &blen); 
	if (blen > 1)
	{
		GLchar* compiler_log = (GLchar*)malloc(blen);
		glGetInfoLogARB(fragmentShader, blen, &slen, compiler_log);
		std::cout << "compiling fragment shader "<< "'" << fileName2 << "' :\n" << compiler_log;
		free (compiler_log);
	}

	programObject = glCreateProgram();
	glAttachShader(programObject, vertexShader);
	glAttachShader(programObject, fragmentShader);
	glLinkProgram(programObject); 
	GLint linked;
	glGetProgramiv(programObject, GL_LINK_STATUS, &linked);
	if (linked)
	{
	std::cout << "Shaders "<< "'" << fileName1 << "', '" << fileName2 << "' " <<"linked" << std::endl;
	}
	else std::cout << "Shaders "<< "'" << fileName1 << "', '" << fileName2 << "' " <<" NOT linked" << std::endl;
	return programObject;
}
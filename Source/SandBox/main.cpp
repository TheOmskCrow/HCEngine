// SandBox.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "SandBox.h"

int main(int iArgc, char** cppArgv) {
	//if (iArgc != 1) exit(0);
	//eng.graphics = cppArgv[0][0];
	//eng.graphics = -1;
	glutInit(&iArgc, cppArgv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	//	glutInitWindowSize(400, 400);
	//	glutInitWindowPosition(400, 400);
	//	glutCreateWindow("test");
	//glutGameModeString("1920x1080:60@32");
	//glutFullScreen();
	glutEnterGameMode();
	SandBox::Init();
	glutDisplayFunc(&SandBox::Draw);
	glutReshapeFunc(&SandBox::Reshape);
	glutKeyboardFunc(&SandBox::KeyPress);
	glutKeyboardUpFunc(&SandBox::KeyRelease);
	glutSpecialFunc(&SandBox::SpecKey);
	glutSpecialUpFunc(&SandBox::SpecKeyRelease);
	glutMouseFunc(&SandBox::MousePress);
	glutMouseWheelFunc(&SandBox::MouseWheelFunc);
//	glutPassiveMotionFunc(&SandBox::MouseMove);
//	glutMotionFunc(&SandBox::MouseMotion);
	glutMainLoop();
	return 0;
}


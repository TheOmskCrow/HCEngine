#pragma once
#include <iostream>
#include <vector>
#include <hc3dContentLoader.h>
#include <hc3dObjects.h>
#include <hc3dMath.h>
#include <hc3dToolkit.h>

using namespace hc3d;

class SandBox
{
public:
	SandBox();
	static void Draw();
	static void Init();
	static void Reshape(int, int);
	static void KeyPress(unsigned char, int, int);
	static void KeyRelease(unsigned char, int, int);
	static void SpecKey(int, int, int);
	static void SpecKeyRelease(int, int, int);
	static void MousePress(int, int, int, int);
	static void MouseMove(int, int);
	static void MouseMotion(int, int);
	static void MouseWheelFunc(int, int, int, int);
public:

private:
	static Objects objects;
	static Objects shadowingObjects;
	static Object* gWindow;
	static Object* gWater;
	static Object* gStone;
	static int cursorX, cursorY;
	static bool hidedWindow;
private:

};


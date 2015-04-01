#include "stdafx.h"
#include "Info.h"
#include "Camera.h"
#include <iostream>

#define YBORDER 20
#define XBORDER 10
#define STRSIZE 30.0

using namespace std;
using namespace hc3d;

void Info::AddOutput(std::string outputStr, Vector3D color) {
	ConsoleLine newLine;
	ConsoleWord newWord;
	newWord.word = outputStr;
	newWord.y = -STRSIZE;
	newWord.y *= (float)console_output.size();
	newWord.color = color; Vector3D(0.6f, 0.4f, 0.0f);
	newLine.push_back(newWord);
	console_output.push_back(newLine);
}

void Info::run_script(ConsoleLine input) {
	if (input.size() <= 1) return;
	if (!strcmp(input[1].word.c_str(), "shadow")) {
		if (input.size() == 3) {
			int newSh = atoi(input[2].word.c_str());
			if (newSh == 1 || newSh == 0) {
				shadow = newSh;
				AddOutput("shadow changed to " + input[2].word, Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				AddOutput("PARAMETER CAN NOT BE SET TO THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
			}
		}
		else if (input.size() == 2) {
			char buf[10];
			_itoa(shadow, buf, 10);
			std::string outStr = "shadow = ";
			outStr += buf;
			AddOutput(outStr, Vector3D(0.6f, 0.4f, 0.0f));
		}
		else {
			AddOutput("INVALID PARAMETERS NUMBER", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "shadow_dist")) {
		if (input.size() == 3) {
			double newSh = atof(input[2].word.c_str());
			if (newSh > 0.0) {
				shadowDist = (float) newSh;
				AddOutput("shadow distance changed to " + input[2].word, Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				AddOutput("PARAMETER CAN NOT BE SET TO THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
			}
		}
		else if (input.size() == 2) {
			char buf[100];
			sprintf(buf, "%f", shadowDist);
			std::string outStr = "shadow_dist = ";
			outStr += buf;
			AddOutput(outStr, Vector3D(0.6f, 0.4f, 0.0f));
		}
		else {
			AddOutput("INVALID PARAMETERS NUMBER", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "bloom")) {
		if (input.size() == 3) {
			int newSh = atoi(input[2].word.c_str());
			if (newSh == 0 || newSh == 1) {
				if (newSh == 1)	bloom = true;
				else bloom = false;
				AddOutput("bloom changed to " + input[2].word, Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				AddOutput("PARAMETER CAN NOT BE SET TO THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
			}
		}
		else if (input.size() == 2) {
			char buf[10];
			if (bloom) sprintf(buf, "1");
			else sprintf(buf, "0");
			std::string outStr = "bloom = ";
			outStr += buf;
			AddOutput(outStr, Vector3D(0.6f, 0.4f, 0.0f));
		}
		else {
			AddOutput("INVALID PARAMETERS NUMBER", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "all")) {
		if (input.size() == 3) {
			int newSh = atoi(input[2].word.c_str());
			if (newSh == 0 || newSh == 1) {
				if (newSh == 1)	{
					bloom = true;
					AA = true;
					godRays = true;
					depthOfField = true;
					SSAO = true;
					filmGrain = true;
				}
				else {
					bloom = false;
					AA = false;
					godRays = false;
					depthOfField = false;
					SSAO = false;
					filmGrain = false;
				}
				AddOutput("all postprocess changed to " + input[2].word, Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				AddOutput("PARAMETER CAN NOT BE SET TO THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
			}
		}
		/*else if (input.size() == 2) {
			char buf[10];
			if (bloom) sprintf(buf, "1");
			else sprintf(buf, "0");
			std::string outStr = "bloom = ";
			outStr += buf;
			AddOutput(outStr, Vector3D(0.6, 0.4, 0.0));
		}*/
		else {
			AddOutput("INVALID PARAMETERS NUMBER", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "planet_roll")) {
		if (input.size() == 2) {
			if (atmosphereRun) {
				atmosphereRun = false;
				AddOutput("Planet rolling turned OFF", Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				atmosphereRun = true;
				AddOutput("Planet rolling turned ON", Vector3D(0.2f, 0.4f, 0.8f));
			}
			
		}
		else {
			AddOutput("NO PARAMETRES CAN BE SET FOR THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "wait")) {
		if (input.size() == 3) {
			int newSh = atoi(input[2].word.c_str());
			if (newSh > 0) {
				wait = 1200.0 * newSh;//120 real seconds in 1 game hour
				AddOutput("waiting " + input[2].word + " hours", Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				AddOutput("PARAMETER CAN NOT BE SET TO THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
			}
		}
		/*else if (input.size() == 2) {
		char buf[10];
		if (bloom) sprintf(buf, "1");
		else sprintf(buf, "0");
		std::string outStr = "bloom = ";
		outStr += buf;
		AddOutput(outStr, Vector3D(0.6, 0.4, 0.0));
		}*/
		else {
			AddOutput("INVALID PARAMETERS NUMBER", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else if (!strcmp(input[1].word.c_str(), "scene_build")) {
		if (input.size() == 2) {
			if (sceneBuild) {
				sceneBuild = false;
				AddOutput("Scene building turned OFF", Vector3D(0.2f, 0.4f, 0.8f));
			}
			else {
				sceneBuild = true;
				AddOutput("Scene building turned ON", Vector3D(0.2f, 0.4f, 0.8f));
			}

		}
		else {
			AddOutput("NO PARAMETRES CAN BE SET FOR THIS VALUE", Vector3D(1.0f, 0.0f, 0.0f));
		}
	}
	else {
		AddOutput("UNKNOWN COMMAND", Vector3D(1.0f, 0.0f, 0.0f));
	}
	//console_output += "SCRIPT RUNNED\n";
}
#ifndef MAIN
#define MAIN

#include "imgui.h"

struct guiNode
{
	int n;

	ImVec2 pos;

	guiNode(int n, ImVec2 pos);
};

#endif
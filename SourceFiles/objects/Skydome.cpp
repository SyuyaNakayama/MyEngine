#include "Skydome.h"

void Skydome::Initialize(float scale)
{
	worldTransform.Initialize();
	worldTransform.scale *= scale;
	model = Model::Create("skydome");
}
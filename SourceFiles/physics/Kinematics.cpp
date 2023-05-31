#include "Kinematics.h"
#include <cassert>

void Motion::Initialize(Vector2 p_, Vector2 v_, Vector2 a_, Vector2 f_, float m_)
{
	p = p_;
	v = v_;
	a = a_;
	f = f_;
	m = m_;
}

void Motion::Update()
{
	assert(m); // 0‚ÅŠ„‚é‚Ì‚ð‘jŽ~
	p += v;
	a += f / m;
	v += a;
}

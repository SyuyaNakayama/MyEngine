#pragma once

/*
	SceneNumはゲームシーンの数だが
	Nullシーンも含むため-1して使う
*/
enum class Scene { Null, Title, Select, Tutorial, Play, Clear, SceneNum = Clear };
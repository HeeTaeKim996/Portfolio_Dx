#pragma once

class ModelAnimator;

class AnimationHelper
{
public:
	void AnimInitialize(shared_ptr<ModelAnimator> animator);

	void Crossfade(int animIndex, float slerp = 0.25f);
	void Crossfade_SyncElapsedTime(int animIndex, float slerp = 0.25f);



public:
	weak_ptr<ModelAnimator> _animator;
private:
	int _animIndex = -1;
};


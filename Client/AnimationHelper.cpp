#include "pch.h"
#include "AnimationHelper.h"

#include "Engine/ModelAnimator.h"

void AnimationHelper::AnimInitialize(shared_ptr<ModelAnimator> animator)
{
	_animator = animator;
}

void AnimationHelper::Crossfade(int animIndex, float slerp)
{

	if (_animIndex == animIndex)
		return;

	_animIndex = animIndex;
	_animator.lock()->CrossFade(animIndex, slerp);
}

void AnimationHelper::Crossfade_SyncElapsedTime(int animIndex, float slerp)
{
	if (_animIndex == animIndex)
		return;

	_animIndex = animIndex;
	_animator.lock()->Crossfade_SyncElapsedTime(animIndex, slerp);
}

#pragma once

class IExecute
{
public:
	virtual void Init() abstract;

	virtual void Awake() abstract;
	virtual void Start() abstract;

	virtual void Update() abstract;
	virtual void FixedUpdate() abstract;
	
	virtual void LateUpdate() abstract;
};


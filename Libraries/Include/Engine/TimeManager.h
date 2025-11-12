#pragma once

#if 1
class TimeManager
{
	//DECLARE_SINGLE(TimeManager);
private:
	TimeManager() {}
public:
	static TimeManager* GetInstance()
	{
		static TimeManager instance;
		return &instance;
	}
public:
	void Init();
	void Update();

	uint32 GetFps() { return _fps; }
	float GetDeltaTime() { return _unscaledDeltaTime * _timeScale; }
	void SetTimeScale(float timeScale);
	const float& GetTimeScale() const { return _timeScale; }

private:
	uint64	_frequency = 0;
	uint64	_prevCount = 0;
	float	_unscaledDeltaTime = 0.f;
	float _timeScale = 1.f;

private:
	uint32	_frameCount = 0;
	float	_frameTime = 0.f;
	uint32	_fps = 0;
};
#endif








































































#if 0 // First
class TimeManager
{
	//DECLARE_SINGLE(TimeManager);
private:
	TimeManager() {}
public:
	static TimeManager* GetInstance()
	{
		static TimeManager instance;
		return &instance;
	}

public:
	void Init();
	void Update();

	uint32 GetFps() { return _fps; }
	float GetDeltaTime() { return _deltaTime; }


private:
	uint64 _frequency = 0;
	uint64 _prevCount = 0;
	float _deltaTime = 0.f;

private:
	uint32 _frameCount = 0;
	float _frameTime = 0.f;
	uint32 _fps = 0;

};

#endif // First
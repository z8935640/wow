#ifndef _ChronoTimer_h_
#define _ChronoTimer_h_
#include <chrono>
class ChronoTimer
{
public:
	ChronoTimer()
	{
		FlushTime();
	}
	virtual ~ChronoTimer() {};

	void FlushTime()
	{
		_time = std::chrono::high_resolution_clock::now();
	}
	double GetSecInterval()
	{
		return GetMicroSecInterval() * 0.000001;
	}
	long long GetMicroSecInterval()
	{
		return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::high_resolution_clock::now() - _time).count();
	}
private:
	std::chrono::time_point<std::chrono::high_resolution_clock> _time;
};

#endif // !_ChronoTimer_h_

#pragma once
#include<iostream>
#include<memory>
#include<chrono>
#include<array>
#include<cmath>

/*ScopeTimer*/
/* Syntax: { Timer TimeIt; Function(); } */
/* Amir A. Roohi - Dec 26th -2022 */

class Timer {

public:
	Timer(bool print) 
	{
		Print=print;
		m_StartTimeReference = std::chrono::high_resolution_clock::now();
		// This need to be private variable in the class, so that it's accessible in method Stop
	}

	~Timer()
	{
		Stop();
		// Stop will be called when the destructor is called automatically when the program goes out of scope
	}

	void Stop()
	{
		auto FinishTimePoint = std::chrono::high_resolution_clock::now(); // Get the system high resolution clock at the stop time

		auto Start = std::chrono::time_point_cast<std::chrono::nanoseconds>(m_StartTimeReference).time_since_epoch().count();
		auto Finish = std::chrono::time_point_cast<std::chrono::nanoseconds>(FinishTimePoint).time_since_epoch().count();

		auto Duration = Finish - Start;
		double MilSec = Duration * 0.000001;

		if (Print){ std::cout << " (" << MilSec << " ms)\n"; }

	}


private:
	std::chrono::time_point<std::chrono::high_resolution_clock> m_StartTimeReference;
	bool Print;
};
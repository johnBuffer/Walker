#pragma once
#include <cstdint>

struct Profiler
{
	struct Element
	{
		uint64_t start, total;
		Element()
			: start(0)
			, total(0)
		{}

		void reset()
		{
			start = 0;
			total = 0;
		}

		float asMilliseconds() const
		{
			return total * 0.001f;
		}
	};

	Profiler()
	{
	}

	void start(Element& elem)
	{
	}

	void stop(Element& elem)
	{
	}
};

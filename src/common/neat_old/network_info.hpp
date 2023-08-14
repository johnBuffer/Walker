#pragma once
#include <cstdint>


struct NetworkInfo
{
	uint32_t inputs;
	uint32_t hidden_count;
	uint32_t outputs;

	NetworkInfo()
		: inputs(0)
		, hidden_count(0)
		, outputs(0)
	{}

	NetworkInfo(uint32_t inputs, uint32_t hidden, uint32_t outputs)
		: inputs(inputs)
		, hidden_count(hidden)
		, outputs(outputs)
	{}

	uint32_t getNodesCount() const
	{
		return inputs + hidden_count + outputs;
	}
};


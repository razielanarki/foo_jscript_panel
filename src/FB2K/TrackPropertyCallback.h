#pragma once

class TrackPropertyCallback : public track_property_callback_v2
{
public:
	bool is_group_wanted(const char*) override
	{
		return true;
	}

	void set_property(const char* group, double, const char* name, const char* value) override
	{
		m_data[group][name] = value;
	}

	std::map<std::string, StringMap> m_data;
};

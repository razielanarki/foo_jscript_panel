#pragma once

static constexpr uint32_t kNumColourComponents = 3U;
static constexpr uint32_t kMaxIterations = 12U;
static constexpr uint32_t kMaxCount = 14U;
static constexpr std::array kMultipliers = { 2.0, 4.0, 3.0 };
using ColourValues = std::array<double, kNumColourComponents>;

class KPoint
{
public:
	KPoint(uint32_t point_id, const ColourValues& values, uint32_t pixel_count) : m_point_id(point_id), m_values(values), m_pixel_count(pixel_count), m_cluster_id(UINT_MAX) {}

	ColourValues m_values;
	uint32_t m_cluster_id = 0, m_pixel_count = 0, m_point_id = 0;
};

using KPoints = std::vector<KPoint>;

class Cluster
{
public:
	Cluster() {}

	Cluster(const KPoint& point) : central_values(point.m_values)
	{
		points.emplace_back(point);
	}

	double calc_dist(const KPoint& point) const
	{
		double sum = 0.0;
		for (const uint32_t i : std::views::iota(0U, kNumColourComponents))
		{
			sum += square_then_multiply(central_values[i] - point.m_values[i], kMultipliers[i]);
		}
		return sum;
	}

	double get_frequency(uint32_t colours_length)
	{
		return get_total_points() / static_cast<double>(colours_length);
	}

	double square_then_multiply(double num, double by) const
	{
		return num * num * by;
	}

	int get_colour()
	{
		return to_int(Gdiplus::Color::MakeARGB(UINT8_MAX, get_colour_component(0), get_colour_component(1), get_colour_component(2)));
	}

	uint8_t get_colour_component(uint32_t index)
	{
		return static_cast<uint8_t>(central_values[index]);
	}

	uint32_t get_total_points() const
	{
		return std::accumulate(points.begin(), points.end(), 0U, [](uint32_t t, const KPoint& point)
			{
				return t + point.m_pixel_count;
			});
	}

	void remove_point(uint32_t point_id)
	{
		std::erase_if(points, [point_id](const KPoint& point)
			{
				return point.m_point_id == point_id;
			});
	}

	ColourValues central_values{};
	KPoints points;
};

using Clusters = std::vector<Cluster>;

class KMeans
{
public:
	KMeans(const KPoints& points, uint32_t count) : m_points(points), m_count(count) {}

	Clusters run()
	{
		const uint32_t count = std::min(std::max(m_count, kMaxCount), m_points.size());

		for (const uint32_t i : std::views::iota(0U, count))
		{
			const uint32_t index_point = i * m_points.size() / count;
			m_points[index_point].m_cluster_id = i;
			Cluster cluster(m_points[index_point]);
			m_clusters.emplace_back(cluster);
		}

		for (const uint32_t i : std::views::iota(0U, kMaxIterations))
		{
			bool done = true;

			for (KPoint& point : m_points)
			{
				const uint32_t old_cluster_id = point.m_cluster_id;
				const uint32_t nearest_centre_id = get_nearest_centre_id(point);

				if (old_cluster_id != nearest_centre_id)
				{
					if (old_cluster_id != UINT_MAX)
					{
						m_clusters[old_cluster_id].remove_point(point.m_point_id);
					}

					point.m_cluster_id = nearest_centre_id;
					m_clusters[nearest_centre_id].points.emplace_back(point);
					done = false;
				}
			}

			for (Cluster& cluster : m_clusters)
			{
				for (const uint32_t j : std::views::iota(0U, kNumColourComponents))
				{
					const uint32_t cluster_total_points = cluster.get_total_points();
					if (cluster_total_points == 0) continue;

					const double sum = std::accumulate(cluster.points.begin(), cluster.points.end(), 0.0, [j](double t, const KPoint& point)
						{
							return t + (point.m_values[j] * point.m_pixel_count);
						});

					cluster.central_values[j] = sum / cluster_total_points;
				}
			}

			if (done) break;
		}

		std::ranges::sort(m_clusters, [](const Cluster& a, const Cluster& b)
			{
				return a.get_total_points() > b.get_total_points();
			});

		if (m_count < m_clusters.size()) m_clusters.resize(m_count);
		return m_clusters;
	}

private:
	uint32_t get_nearest_centre_id(const KPoint& point)
	{
		auto view = m_clusters | std::views::transform([point](const Cluster& cluster) { return cluster.calc_dist(point); });
		const auto it = std::ranges::min_element(view);
		return std::ranges::distance(view.begin(), it);
	}

	Clusters m_clusters;
	KPoints m_points;
	uint32_t m_count = 0;
};

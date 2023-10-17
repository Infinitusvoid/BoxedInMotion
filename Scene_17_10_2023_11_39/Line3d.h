#pragma once

struct Line3d
{
	glm::vec3 start;
	glm::vec3 end;
};

namespace Line3d_
{
	float length(const Line3d& line)
	{
		return glm::length(line.end - line.start);
	}

	glm::vec3 direction(const Line3d& line)
	{
		return glm::normalize(line.end - line.start);
	}

	glm::vec3 point_at(const Line3d& line, float t)
	{
		return line.start + glm::clamp(t, 0.0f, 1.0f) * (line.end - line.start);
	}

	std::optional<glm::vec3> Intersects(const Line3d& line, const Line3d& other)
	{
		glm::vec3 dir1 = line.end - line.start;
		glm::vec3 dir2 = other.end - other.start;
		glm::vec3 start2_start1 = other.start - line.start;

		glm::vec3 cross = glm::cross(dir1, dir2);
		if (glm::length(cross) < 1e-6) {
			// The lines are either parallel or collinear, so they don't intersect.
			return std::nullopt;
		}

		float t = glm::dot(glm::cross(start2_start1, dir2), cross) / glm::length(cross) / glm::length(cross);
		float s = glm::dot(glm::cross(start2_start1, dir1), cross) / glm::length(cross) / glm::length(cross);

		if (t >= 0.0f && t <= 1.0f && s >= 0.0f && s <= 1.0f) {
			// Calculate the intersection point.
			glm::vec3 intersection = line.start + t * dir1;
			return intersection;
		}

		return std::nullopt;
	}

	float distance_to_point(const Line3d& line, const glm::vec3& position)
	{
		glm::vec3 line_direction = line.end - line.start;
		glm::vec3 point_to_start = line.start - position;
		return glm::length(glm::cross(line_direction, point_to_start)) / glm::length(line_direction);
	}

	glm::vec3 midpoint(const Line3d& line)
	{
		return line.start + 0.5f * (line.end - line.start);
	}

	float angle_beteen(const Line3d& line, const Line3d& other)
	{
		glm::vec3 dir1 = glm::normalize(line.end - line.start);
		glm::vec3 dir2 = glm::normalize(other.end - other.start);
		return acos(glm::dot(dir1, dir2));
	}
}
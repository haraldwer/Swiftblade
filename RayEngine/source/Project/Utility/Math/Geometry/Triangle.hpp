
#pragma once

namespace Utility
{
	namespace Math
	{

		template<typename PositionType, typename VertexData = PositionType, int position_byte_offset = 0>
		class Triangle
		{
		public:

			struct Vertex
			{
				union
				{
					Memory::Map<PositionType, position_byte_offset> position;
					VertexData interpolated_data;
				};

				operator VertexData& ()
				{
					return interpolated_data;
				}

				Vertex(const VertexData& interpolated_data) : interpolated_data(interpolated_data) {}
				Vertex() {}
				~Vertex() {}

			}vertex[3];


			inline static float Area(const PositionType& v1, const PositionType& v2, const PositionType& v3)
			{
				return 0.5f * (v1.x * (v2.y - v3.y) + v2.x * (v3.y - v1.y) + v3.x * (v1.y - v2.y));
			}

			// TODO: change place of is_inside a and b
			inline std::pair<int, std::array<Vertex, 6>> GetClipPoints(Triangle clipped_triangle)
			{
				int number_of_points = 0;
				std::array<Vertex, 6> clip_points;

				bool is_inside_a[3] = { true, true, true };

				// Index a and b refers to the starting vertex of each triangle used for the line segment.
				for (int a_index = 0; a_index < 3; ++a_index)
				{
					int a_index_end = (a_index + 1) % 3;
					PositionType av1 = { clipped_triangle.vertex[a_index].position().x, clipped_triangle.vertex[a_index].position().y };
					PositionType av2 = { clipped_triangle.vertex[a_index_end].position().x, clipped_triangle.vertex[a_index_end].position().y };
					PositionType a_diff = av2 - av1;

					bool is_inside_b = true;

					for (int b_index = 0; b_index < 3; ++b_index)
					{
						int b_index_end = (b_index + 1) % 3;
						PositionType bv1 = { vertex[b_index].position().x, vertex[b_index].position().y };
						PositionType bv2 = { vertex[b_index_end].position().x, vertex[b_index_end].position().y };
						PositionType b_diff = bv2 - bv1;

						// Check Edge Intersection
						float diff_cross = a_diff.Cross(b_diff);
						if (diff_cross != 0.0f) // TODO: make more stable?
						{
							float intersection_t = (bv1 - av1).Cross(b_diff) / (diff_cross);
							float intersection_u = (bv1 - av1).Cross(a_diff) / (diff_cross);

							if (intersection_t >= 0.0f && intersection_t <= 1.0f && intersection_u >= 0.0f && intersection_u <= 1.0f)
							{
								VertexData& point_data = clip_points[number_of_points];

								point_data.Interpolate(clipped_triangle.vertex[a_index].interpolated_data, clipped_triangle.vertex[a_index_end].interpolated_data, intersection_t);

								number_of_points++;
							}
						}

						// Check If Vertex Is Inside Triangles
						PositionType ba_diff = av1 - bv1;

						float dot = ba_diff.Dot({ -b_diff.y, b_diff.x });
						if (dot < 0.0f)
						{
							is_inside_b = false;
						}

						PositionType ab_diff = bv1 - av1;
						dot = ab_diff.Dot({ -a_diff.y, a_diff.x });
						if (dot < 0.0f)
						{
							is_inside_a[b_index] = false;
						}
					}

					if (is_inside_b)
					{
						VertexData& point_data = clip_points[number_of_points];
						point_data = clipped_triangle.vertex[a_index].interpolated_data;
						number_of_points++;
					}
				}

				for (int b_index = 0; b_index < 3; ++b_index)
				{
					if (is_inside_a[b_index])
					{
						auto position = vertex[b_index].position();

						float clipper_area = Area(clipped_triangle.vertex[0].position, clipped_triangle.vertex[1].position, clipped_triangle.vertex[2].position);
						

						float area_a = Area(clipped_triangle.vertex[1].position, clipped_triangle.vertex[2].position, position);
						float area_b = Area(clipped_triangle.vertex[2].position, clipped_triangle.vertex[0].position, position);
						float area_c = Area(clipped_triangle.vertex[0].position, clipped_triangle.vertex[1].position, position);

						float interpolation_0 = area_a / clipper_area;
						float interpolation_1 = area_b / clipper_area;
						float interpolation_2 = area_c / clipper_area;

						Vertex& vertex_data = clip_points[number_of_points];

						vertex_data.interpolated_data.Reset();
						vertex_data.interpolated_data.Accumulate(clipped_triangle.vertex[0], interpolation_0);
						vertex_data.interpolated_data.Accumulate(clipped_triangle.vertex[1], interpolation_1);
						vertex_data.interpolated_data.Accumulate(clipped_triangle.vertex[2], interpolation_2);

						number_of_points++;
					}
				}

				return { number_of_points, clip_points };
			}

			inline std::pair<int, std::array<Triangle, 4>> Clip(Triangle clipped_triangle)
			{
				bool used_clip_points[6] = {};
				auto clip_points = GetClipPoints(clipped_triangle);

				int number_of_new_triangles = 0;
				std::array<Triangle, 4> new_triangles;

				if (clip_points.first > 2)
				{
					int left_most_vertex_index = 0;
					for (int index = 1; index < clip_points.first; ++index)
					{
						if (clip_points.second[index].position().x < clip_points.second[left_most_vertex_index].position().x)
						{
							left_most_vertex_index = index;
						}
					}

					used_clip_points[left_most_vertex_index] = true;
					int challanger_index = -1;
					int previous_accepted_challanger_index = -1;

					PositionType& left_most_position = clip_points.second[left_most_vertex_index].position;
					PositionType challanger_diff;

					for (int challange_index = 0; challange_index < clip_points.first - 1; ++challange_index)
					{
						for (int index = 0; index < clip_points.first; ++index)
						{
							if (!used_clip_points[index] && index != challanger_index)
							{
								auto diff = clip_points.second[index].position - left_most_position;
								float dot = (challanger_diff.y * diff.x) + (-challanger_diff.x * diff.y);

								if (dot > 0.0 || challanger_index == -1)
								{
									challanger_index = index;
									challanger_diff = diff;
								}
							}
						}

						if (previous_accepted_challanger_index > -1)
						{
							// New Triangle
							auto& triangle = new_triangles[number_of_new_triangles];
							triangle.vertex[0].interpolated_data = clip_points.second[left_most_vertex_index];
							triangle.vertex[1].interpolated_data = clip_points.second[previous_accepted_challanger_index];
							triangle.vertex[2].interpolated_data = clip_points.second[challanger_index];

							number_of_new_triangles++;
						}

						previous_accepted_challanger_index = challanger_index;
						used_clip_points[challanger_index] = true;
						challanger_index = -1;
					}


				}

				return { number_of_new_triangles, new_triangles };
			}

			inline Triangle& operator = (const Triangle& triangle)
			{
				vertex[0].interpolated_data = triangle.vertex[0].interpolated_data;
				vertex[1].interpolated_data = triangle.vertex[1].interpolated_data;
				vertex[2].interpolated_data = triangle.vertex[2].interpolated_data;
				return *this;
			}

			Triangle(const VertexData& v1, const VertexData& v2, const VertexData& v3) : vertex { v1, v2, v3 } {}
			Triangle(const Triangle& triangle) : vertex{ triangle.vertex[0], triangle.vertex[1], triangle.vertex[2] } {}
			Triangle() {}
			~Triangle() {}
		};
	}
}


#pragma once

namespace Utility
{
	namespace Math
	{

		template<typename PositionType, typename VertexData = PositionType, int PositionByteOffset = 0>
		class Triangle
		{
		public:

			struct Vertex
			{
				union
				{
					Memory::Map<PositionType, PositionByteOffset> position;
					VertexData interpolatedData;
				};

				operator VertexData& ()
				{
					return interpolatedData;
				}

				Vertex(const VertexData& InInterpolatedData) : interpolatedData(InInterpolatedData) {}

			} vertex[3];

			static float Area(const PositionType& InV1, const PositionType& InV2, const PositionType& InV3)
			{
				return 0.5f * (InV1.x * (InV2.y - InV3.y) + InV2.x * (InV3.y - InV1.y) + InV3.x * (InV1.y - InV2.y));
			}

			// TODO: change place of is_inside a and b
			std::pair<int, std::array<Vertex, 6>> GetClipPoints(Triangle InClippedTriangle)
			{
				int numberOfPoints = 0;
				std::array<Vertex, 6> clipPoints;

				bool isInsideA[3] = { true, true, true };

				// Index a and b refers to the starting vertex of each triangle used for the line segment.
				for (int aIndex = 0; aIndex < 3; ++aIndex)
				{
					int aIndexEnd = (aIndex + 1) % 3;
					PositionType av1 = { InClippedTriangle.vertex[aIndex].position().x, InClippedTriangle.vertex[aIndex].position().y };
					PositionType av2 = { InClippedTriangle.vertex[aIndexEnd].position().x, InClippedTriangle.vertex[aIndexEnd].position().y };
					PositionType aDiff = av2 - av1;

					bool isInsideB = true;

					for (int bIndex = 0; bIndex < 3; ++bIndex)
					{
						int bIndexEnd = (bIndex + 1) % 3;
						PositionType bv1 = { vertex[bIndex].position().x, vertex[bIndex].position().y };
						PositionType bv2 = { vertex[bIndexEnd].position().x, vertex[bIndexEnd].position().y };
						PositionType bDiff = bv2 - bv1;

						// Check Edge Intersection
						float diffCross = aDiff.Cross(bDiff);
						if (diffCross != 0.0f) // TODO: make more stable?
						{
							float intersectionT = (bv1 - av1).Cross(bDiff) / (diffCross);
							const float intersectionU = (bv1 - av1).Cross(aDiff) / (diffCross);

							if (intersectionT >= 0.0f && intersectionT <= 1.0f && intersectionU >= 0.0f && intersectionU <= 1.0f)
							{
								VertexData& pointData = clipPoints[numberOfPoints];

								pointData.Interpolate(InClippedTriangle.vertex[aIndex].interpolated_data, InClippedTriangle.vertex[aIndexEnd].interpolated_data, intersectionT);

								numberOfPoints++;
							}
						}

						// Check If Vertex Is Inside Triangles
						PositionType baDiff = av1 - bv1;

						float dot = baDiff.Dot({ -bDiff.y, bDiff.x });
						if (dot < 0.0f)
						{
							isInsideB = false;
						}

						PositionType abDiff = bv1 - av1;
						dot = abDiff.Dot({ -aDiff.y, aDiff.x });
						if (dot < 0.0f)
						{
							isInsideA[bIndex] = false;
						}
					}

					if (isInsideB)
					{
						VertexData& pointData = clipPoints[numberOfPoints];
						pointData = InClippedTriangle.vertex[aIndex].interpolated_data;
						numberOfPoints++;
					}
				}

				for (int bIndex = 0; bIndex < 3; ++bIndex)
				{
					if (isInsideA[bIndex])
					{
						auto position = vertex[bIndex].position();

						const float clipperArea = Area(InClippedTriangle.vertex[0].position, InClippedTriangle.vertex[1].position, InClippedTriangle.vertex[2].position);


						const float areaA = Area(InClippedTriangle.vertex[1].position, InClippedTriangle.vertex[2].position, position);
						const float areaB = Area(InClippedTriangle.vertex[2].position, InClippedTriangle.vertex[0].position, position);
						const float areaC = Area(InClippedTriangle.vertex[0].position, InClippedTriangle.vertex[1].position, position);

						float interpolation0 = areaA / clipperArea;
						float interpolation1 = areaB / clipperArea;
						float interpolation2 = areaC / clipperArea;

						Vertex& vertexData = clipPoints[numberOfPoints];

						vertexData.interpolated_data.Reset();
						vertexData.interpolated_data.Accumulate(InClippedTriangle.vertex[0], interpolation0);
						vertexData.interpolated_data.Accumulate(InClippedTriangle.vertex[1], interpolation1);
						vertexData.interpolated_data.Accumulate(InClippedTriangle.vertex[2], interpolation2);

						numberOfPoints++;
					}
				}

				return { numberOfPoints, clipPoints };
			}

			std::pair<int, Array<Triangle, 4>> Clip(const Triangle InClippedTriangle)
			{
				bool usedClipPoints[6] = {};
				auto clipPoints = GetClipPoints(InClippedTriangle);

				int numberOfNewTriangles = 0;
				Array<Triangle, 4> newTriangles;

				if (clipPoints.first > 2)
				{
					int leftMostVertexIndex = 0;
					for (int index = 1; index < clipPoints.first; ++index)
					{
						if (clipPoints.second[index].position().x < clipPoints.second[leftMostVertexIndex].position().x)
						{
							leftMostVertexIndex = index;
						}
					}

					usedClipPoints[leftMostVertexIndex] = true;
					int challangerIndex = -1;
					int previousAcceptedChallangerIndex = -1;

					PositionType& leftMostPosition = clipPoints.second[leftMostVertexIndex].position;
					PositionType challangerDiff;

					for (int challangeIndex = 0; challangeIndex < clipPoints.first - 1; ++challangeIndex)
					{
						for (int index = 0; index < clipPoints.first; ++index)
						{
							if (!usedClipPoints[index] && index != challangerIndex)
							{
								auto diff = clipPoints.second[index].position - leftMostPosition;
								float dot = (challangerDiff.y * diff.x) + (-challangerDiff.x * diff.y);

								if (dot > 0.0 || challangerIndex == -1)
								{
									challangerIndex = index;
									challangerDiff = diff;
								}
							}
						}

						if (previousAcceptedChallangerIndex > -1)
						{
							// New Triangle
							auto& triangle = newTriangles[numberOfNewTriangles];
							triangle.vertex[0].interpolated_data = clipPoints.second[leftMostVertexIndex];
							triangle.vertex[1].interpolated_data = clipPoints.second[previousAcceptedChallangerIndex];
							triangle.vertex[2].interpolated_data = clipPoints.second[challangerIndex];

							numberOfNewTriangles++;
						}

						previousAcceptedChallangerIndex = challangerIndex;
						usedClipPoints[challangerIndex] = true;
						challangerIndex = -1;
					}


				}

				return { numberOfNewTriangles, newTriangles };
			}

			inline Triangle& operator = (const Triangle& InTriangle)
			{
				vertex[0].interpolated_data = InTriangle.vertex[0].interpolated_data;
				vertex[1].interpolated_data = InTriangle.vertex[1].interpolated_data;
				vertex[2].interpolated_data = InTriangle.vertex[2].interpolated_data;
				return *this;
			}

			Triangle(const VertexData& InV1, const VertexData& InV2, const VertexData& InV3) : vertex { InV1, InV2, InV3 } {}
			Triangle(const Triangle& InTriangle) : vertex{ InTriangle.vertex[0], InTriangle.vertex[1], InTriangle.vertex[2] } {}
		};
	}
}

#include "RectImage.h"

void UI::RectImage::Draw()
{
    const Rect rect = GetRect();
    DrawRect(rect);
    
    if (const auto textureRes = Texture.Get())
    {
        if (const auto texture = textureRes->Get())
        {
            auto getCorners = [&](const Rect& InRef)
            {
                Vector<Rect> corners;
                
                // 1
                corners.push_back({
                    InRef.Start,
                    { InRef.Start.x + Edges.Horizontal.x, InRef.Start.y + Edges.Vertical.x }
                });

                // 2
                corners.push_back({
                    { InRef.Start.x + Edges.Horizontal.x, InRef.Start.y },
                    { InRef.End.x - Edges.Horizontal.y, InRef.Start.y + Edges.Vertical.x }
                });

                // 3
                corners.push_back({
                    { InRef.End.x - Edges.Horizontal.y, InRef.Start.y },
                    { InRef.End.x, InRef.Start.y + Edges.Vertical.x }
                });

                // 4
                corners.push_back({
                    { InRef.Start.x, InRef.Start.y + Edges.Vertical.x },
                    { InRef.Start.x + Edges.Horizontal.x, InRef.End.y - Edges.Vertical.y }
                });

                // 5
                corners.push_back({
                    { InRef.Start.x + Edges.Horizontal.x, InRef.Start.y + Edges.Vertical.x },
                    { InRef.End.x - Edges.Horizontal.y, InRef.End.y - Edges.Vertical.y }
                });

                // 6
                corners.push_back({
                    { InRef.End.x - Edges.Horizontal.y, InRef.Start.y + Edges.Vertical.x },
                    { InRef.End.x, InRef.End.y - Edges.Vertical.y }
                });

                // 7
                corners.push_back({
                    { InRef.Start.x, InRef.End.y - Edges.Vertical.y },
                    { InRef.Start.x + Edges.Horizontal.x, InRef.End.y }
                });

                // 8
                corners.push_back({
                    { InRef.Start.x + Edges.Horizontal.x, InRef.End.y - Edges.Vertical.y },
                    { InRef.End.x - Edges.Horizontal.y, InRef.End.y }
                });

                // 9
                corners.push_back({
                    { InRef.End.x - Edges.Horizontal.y, InRef.End.y - Edges.Vertical.y },
                    { InRef.End.x, InRef.End.y }
                });

                return corners;
            };

            const Rect sourceRect = {
                {
                    0.0f,
                    0.0f,
                },
                {
                    static_cast<float>(texture->width),
                    static_cast<float>(texture->height)
                }
            };

            const Vector<Rect> sourceCorners = getCorners(sourceRect);
            const Vector<Rect> destCorners = getCorners(rect);

            for (int i = 0; i < 9; i++)
            {
                const Rect& source = sourceCorners[i];
                const Rect& dest = destCorners[i];
                DrawTexturePro(
                    *texture,
                    {
                        source.Start.x,
                        source.Start.y,
                        source.End.x - source.Start.x,
                        source.End.y - source.Start.y
                    }, {
                        dest.Start.x,
                        dest.Start.y,
                        dest.End.x - dest.Start.x,
                        dest.End.y - dest.Start.y
                    },
                    { 0.0f, 0.0f },
                    0.0f,
                    WHITE);
            }
        }
    }
}

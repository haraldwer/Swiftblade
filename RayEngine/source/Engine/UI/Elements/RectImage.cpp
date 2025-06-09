#include "RectImage.h"

#include "raylib.h"

void UI::RectImage::Draw(Container& InOwner)
{
    const Rect rect = GetRect();
    DrawRect(rect);
    
    if (const auto textureRes = texture.Get())
    {
        if (const auto tex = textureRes->Get())
        {
            auto getCorners = [&](const Rect& InRef)
            {
                Vector<Rect> corners;
                
                // 1
                corners.push_back({
                    InRef.start,
                    { InRef.start.x + edges.horizontal.x, InRef.start.y + edges.vertical.x }
                });

                // 2
                corners.push_back({
                    { InRef.start.x + edges.horizontal.x, InRef.start.y },
                    { InRef.end.x - edges.horizontal.y, InRef.start.y + edges.vertical.x }
                });

                // 3
                corners.push_back({
                    { InRef.end.x - edges.horizontal.y, InRef.start.y },
                    { InRef.end.x, InRef.start.y + edges.vertical.x }
                });

                // 4
                corners.push_back({
                    { InRef.start.x, InRef.start.y + edges.vertical.x },
                    { InRef.start.x + edges.horizontal.x, InRef.end.y - edges.vertical.y }
                });

                // 5
                corners.push_back({
                    { InRef.start.x + edges.horizontal.x, InRef.start.y + edges.vertical.x },
                    { InRef.end.x - edges.horizontal.y, InRef.end.y - edges.vertical.y }
                });

                // 6
                corners.push_back({
                    { InRef.end.x - edges.horizontal.y, InRef.start.y + edges.vertical.x },
                    { InRef.end.x, InRef.end.y - edges.vertical.y }
                });

                // 7
                corners.push_back({
                    { InRef.start.x, InRef.end.y - edges.vertical.y },
                    { InRef.start.x + edges.horizontal.x, InRef.end.y }
                });

                // 8
                corners.push_back({
                    { InRef.start.x + edges.horizontal.x, InRef.end.y - edges.vertical.y },
                    { InRef.end.x - edges.horizontal.y, InRef.end.y }
                });

                // 9
                corners.push_back({
                    { InRef.end.x - edges.horizontal.y, InRef.end.y - edges.vertical.y },
                    { InRef.end.x, InRef.end.y }
                });

                return corners;
            };

            const Rect sourceRect = {
                {
                    0.0f,
                    0.0f,
                },
                {
                    static_cast<float>(tex->width),
                    static_cast<float>(tex->height)
                }
            };

            const Vector<Rect> sourceCorners = getCorners(sourceRect);
            const Vector<Rect> destCorners = getCorners(rect);

            for (int i = 0; i < 9; i++)
            {
                const Rect& source = sourceCorners[i];
                const Rect& dest = ReferenceToViewport(destCorners[i]);
                DrawTexturePro(
                    *tex,
                    {
                        source.start.x,
                        source.start.y,
                        source.end.x - source.start.x,
                        source.end.y - source.start.y
                    }, {
                        dest.start.x,
                        dest.start.y,
                        dest.end.x - dest.start.x,
                        dest.end.y - dest.start.y
                    },
                    { 0.0f, 0.0f },
                    0.0f,
                    ::WHITE);
            }
        }
    }
}

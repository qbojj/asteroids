// TGraphics.h: interface for the CTGraphics class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(TGRAPHICS_H)
#define TGRAPHICS_H

void DrawLine(CDC *pDC, int x1, int y1, int x2, int y2, COLORREF color);
void DrawPolyline(CDC *pDC, POINT* points, int numberPoints, COLORREF color);
void DrawPolygon(CDC *pDC, POINT* points, int numberPoints, COLORREF color);
void DrawEllipse(CDC *pDC, int x1, int y1, int x2, int y2, COLORREF color);
void DrawArc(CDC *pDC, int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4, COLORREF color);
//void DrawPie(CDC *pDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
//void DrawChord(CDC *pDC, int x1, int y1, int x2, int y2, float startAngle, float endAngle, COLORREF color);
//void DrawRoundRect(CDC *pDC, int x1, int y1, int x2, int y2, int width, int height, COLORREF color);
//void DrawBezier(CDC *pDC, POINT* points, int numPoints, COLORREF color);

#endif // !defined(TGRAPHICS_H)

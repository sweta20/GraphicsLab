#include <windows.h>
#include "Fill.h"

DRAWING_DATA gDrawData; // global data

void reDraw(HWND hwnd)
{
  InvalidateRect(hwnd, NULL, 1);
}

void setDrawMode(MODE mode, HWND hwnd)
{
  gDrawData.drawMode = mode;
  setupMenus(hwnd);
}

void createMemoryBitmap(HDC hdc)
{
  gDrawData.hdcMem = CreateCompatibleDC(hdc);
  gDrawData.hbmp = CreateCompatibleBitmap(hdc,
       gDrawData.maxBoundary.cx, gDrawData.maxBoundary.cy);
  SelectObject(gDrawData.hdcMem, gDrawData.hbmp);
  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx,
       gDrawData.maxBoundary.cy, PATCOPY);
}

void initialize(HWND hwnd, HDC hdc)
{
  gDrawData.nCornerPts = 0;
  gDrawData.hDrawPen=CreatePen(PS_SOLID, 1, CLR_BOUNDARY);
  gDrawData.hFillPen=CreatePen(PS_SOLID, 1, CLR_BG);
  gDrawData.hFillRed=CreatePen(PS_SOLID, 1, CLR_RED);
  gDrawData.hFillGreen=CreatePen(PS_SOLID, 1, CLR_GREEN);
  gDrawData.hFillBlue=CreatePen(PS_SOLID, 1, CLR_BLUE);
  gDrawData.hFillYellow=CreatePen(PS_SOLID, 1, CLR_YELLOW);
  gDrawData.hFillCyan=CreatePen(PS_SOLID, 1, CLR_CYAN);

  gDrawData.maxBoundary.cx = GetSystemMetrics(SM_CXSCREEN);
  gDrawData.maxBoundary.cy = GetSystemMetrics(SM_CYSCREEN);
  createMemoryBitmap(hdc);
  setDrawMode(DRAW_MODE, hwnd);
}

void cleanup()
{
  DeleteDC(gDrawData.hdcMem);
}

void reset(HWND hwnd)
{
  gDrawData.nCornerPts = 0;
  gDrawData.drawMode = DRAW_MODE;

  PatBlt(gDrawData.hdcMem, 0, 0, gDrawData.maxBoundary.cx,
         gDrawData.maxBoundary.cy, PATCOPY);

  reDraw(hwnd);
  setupMenus(hwnd);
}

LRESULT CALLBACK DlgAxis(HWND hdlg, UINT mess, WPARAM more, LPARAM pos)
{
	char str[20];
	switch (mess)
	{
    case WM_INITDIALOG:
        SetFocus(GetDlgItem(hdlg, ID_EB1));
        break;
	case WM_COMMAND:
		switch (more)
		{
		case ID_OK:
			GetDlgItemText(hdlg, ID_EB1, str, 20);
			gDrawData.radius[gDrawData.nCornerPts] = (long int)(atof(str));
			if (gDrawData.radius[gDrawData.nCornerPts] < 0)
                MessageBox(hdlg, "Radius cannot be negative!", "Error!", MB_ICONERROR);
			else{
				EndDialog(hdlg, TRUE);
				return 1;
			}
			break;

		case ID_CANCEL:
			EndDialog(hdlg, FALSE);
			break;
		}
		break;
	case WM_CLOSE:
		EndDialog(hdlg, FALSE);
		break;
	default:
		break;
	}
	return 0;
}

void updateBoundary(int x, int y)
{
    if(gDrawData.nCornerPts == 0) {
        gDrawData.lowerRightX = x + gDrawData.radius[0];
        gDrawData.upperLeftX = x - gDrawData.radius[0];
        gDrawData.lowerRightY = y - gDrawData.radius[0];
        gDrawData.upperLeftY = y + gDrawData.radius[0];
    }
    else {
        if(x + gDrawData.radius[gDrawData.nCornerPts] > gDrawData.lowerRightX)
            gDrawData.lowerRightX = x + gDrawData.radius[gDrawData.nCornerPts];
        if(x - gDrawData.radius[gDrawData.nCornerPts] < gDrawData.upperLeftX)
            gDrawData.upperLeftX = x - gDrawData.radius[gDrawData.nCornerPts];
        if(y - gDrawData.radius[gDrawData.nCornerPts] < gDrawData.lowerRightY)
            gDrawData.lowerRightY = y - gDrawData.radius[gDrawData.nCornerPts];
        if(y + gDrawData.radius[gDrawData.nCornerPts] > gDrawData.upperLeftY)
            gDrawData.upperLeftY = y + gDrawData.radius[gDrawData.nCornerPts];
    }
}

HPEN getPen(int x, int y)
{
    int dist1 = (gDrawData.cornerPts[0].x - x)*(gDrawData.cornerPts[0].x - x) + (gDrawData.cornerPts[0].y - y)*(gDrawData.cornerPts[0].y - y) - gDrawData.radius[0]*gDrawData.radius[0];
    int dist2 = (gDrawData.cornerPts[1].x - x)*(gDrawData.cornerPts[1].x - x) + (gDrawData.cornerPts[1].y - y)*(gDrawData.cornerPts[1].y - y) - gDrawData.radius[1]*gDrawData.radius[1];
    int dist3 = (gDrawData.cornerPts[2].x - x)*(gDrawData.cornerPts[2].x - x) + (gDrawData.cornerPts[2].y - y)*(gDrawData.cornerPts[2].y - y) - gDrawData.radius[2]*gDrawData.radius[2];
    bool one = dist1 < 0;
    bool two = dist2 < 0;
    bool three = dist3 < 0;
    if(one && two && three)
        return gDrawData.hFillRed;
    if(one && two)
        return gDrawData.hFillBlue;
    if(two && three)
        return gDrawData.hFillGreen;
    if(one && three)
        return gDrawData.hFillYellow;
    if(one || two || three)
        return gDrawData.hFillCyan;
    return gDrawData.hFillPen;
}

void scanFill()
{
    int y = gDrawData.lowerRightY+1;
    int startX, endX, x;
    while(y < gDrawData.upperLeftY)
    {
        x = gDrawData.upperLeftX+1;
        while(x < gDrawData.lowerRightX)
        {
            while(x < gDrawData.lowerRightX && !(GetPixel(gDrawData.hdcMem, x-1, y) == CLR_BOUNDARY && GetPixel(gDrawData.hdcMem, x, y) == CLR_BG))
                x++;
            startX = x;
            while(x < gDrawData.lowerRightX && !(GetPixel(gDrawData.hdcMem, x+1, y) == CLR_BOUNDARY && GetPixel(gDrawData.hdcMem, x, y) == CLR_BG))
                x++;
            x++;
            endX = x;
            if(startX < gDrawData.lowerRightX && endX <= gDrawData.lowerRightX) {
                SelectObject(gDrawData.hdcMem, getPen((startX+endX)/2, y));
                MoveToEx(gDrawData.hdcMem,startX,y, NULL);
                LineTo(gDrawData.hdcMem,endX,y);
            }
        }
        y++;
    }
}

void drawCircle(HWND hwnd, int x, int y)
{
  /* the coordinates of the points are stored in an array */

  if (gDrawData.nCornerPts < nMaxNoOfCornerPts)
  {
    SelectObject(gDrawData.hdcMem, gDrawData.hDrawPen);
	if(DialogBox(NULL, "MyDB", hwnd, (DLGPROC)DlgAxis)){
        int radius = gDrawData.radius[gDrawData.nCornerPts];
        updateBoundary(x, y);
        Arc(gDrawData.hdcMem, x-radius, y-radius, x+radius, y+radius, x, y-radius, x, y-radius);
        gDrawData.cornerPts[gDrawData.nCornerPts].x = x;
        gDrawData.cornerPts[gDrawData.nCornerPts].y = y;
        gDrawData.nCornerPts++;
        if(gDrawData.nCornerPts == nMaxNoOfCornerPts){
            setDrawMode(DRAWN_MODE, hwnd);
        }
	}
  }
}


void drawImage(HDC hdc)
{
  BitBlt(hdc, 0, 0, gDrawData.maxBoundary.cx,
    gDrawData.maxBoundary.cy, gDrawData.hdcMem, 0, 0, SRCCOPY);
}

void processLeftButtonDown(HWND hwnd, int x, int y)
{
  switch (gDrawData.drawMode)
  {
    case DRAW_MODE:
      drawCircle(hwnd,x,y);
      reDraw(hwnd);
      break;
    default:
      return;
  }
}

void processCommand(int cmd, HWND hwnd)
{
  int response;
  switch(cmd)
  {
    case ID_CLEAR:
      reset(hwnd);
      setDrawMode(DRAW_MODE, hwnd);
      break;
    case ID_EXIT:
      response=MessageBox(hwnd,"Quit the program?", "EXIT", MB_YESNO);
      if(response==IDYES)
        PostQuitMessage(0);
      break;
    default:
      break;
  }
}

LRESULT CALLBACK WindowF(HWND hwnd,UINT message,WPARAM wParam,
                         LPARAM lParam)
{
  HDC hdc;
  PAINTSTRUCT ps;
  int x,y;

  switch(message)
  {
    case WM_CREATE:
      hdc = GetDC(hwnd);
      initialize(hwnd, hdc);
      ReleaseDC(hwnd, hdc);
      break;

    case WM_COMMAND:
      processCommand(LOWORD(wParam), hwnd);
      break;

    case WM_LBUTTONDOWN:
      x = LOWORD(lParam);
      y = HIWORD(lParam);
      processLeftButtonDown(hwnd, x,y);
      break;
    case WM_CHAR:
        switch(wParam)
        {
            case 0x0D:
            if(gDrawData.drawMode == DRAWN_MODE){
                 scanFill();
                 reDraw(hwnd);
             }
             break;
        }
        break;
    case WM_PAINT:
      hdc = BeginPaint(hwnd, &ps);
      drawImage(hdc);
      EndPaint(hwnd, &ps);
      break;

    case WM_DESTROY:
      cleanup();
      PostQuitMessage(0);
      break;

    default:
      break;
  }
  // Call the default window handler
  return DefWindowProc(hwnd, message, wParam, lParam);
}

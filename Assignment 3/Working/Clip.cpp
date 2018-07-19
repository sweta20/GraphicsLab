#include <windows.h>
#include "Line.h"
#include "ClipUtil.h"

void scLineClip(POINT start, POINT end)
{   
 	int p[4],q[4],i,dx,dy,flag=1;
    double u1=0,u2=1,temp;
  	POINT clipStart, clipEnd;

  // groundwork before drawing contiguous line segments for clipping
  setupLineSegmentDrawing(gDrawData.hdcMem, start, end);
  
    dx=end.x-start.x;
    dy=end.y-start.y;
    p[0]=-dx;     q[0]=start.x-gDrawData.clipMin.x;
    p[1]=dx;      q[1]=gDrawData.clipMax.x-start.x;
    p[2]=-dy;     q[2]=start.y-gDrawData.clipMin.y;
    p[3]=dy;      q[3]=gDrawData.clipMax.y-start.y;

    if (p[0]==0)
    {
	    if(q[0]*q[1]<=0)
	        {
	          	drawNextLineSegment(end, CLR_BG);
				return;
			}
		else{  
			clipStart.x = start.x;
			clipStart.y = gDrawData.clipMin.y;
			clipEnd.x = end.x;
			clipEnd.y = gDrawData.clipMax.y;

			drawNextLineSegment(clipStart, CLR_BG);
			drawNextLineSegment(clipEnd, CLR_LINE);
			drawNextLineSegment(end, CLR_BG);
			performCorrectionAtClipPts(gDrawData.hdcMem, clipStart,
			                         CLR_LINE, CLR_BG);
			performCorrectionAtClipPts(gDrawData.hdcMem, clipEnd,
			                         CLR_LINE, CLR_BG);
			return;
			}	
				
	}
    if(p[2]==0)
    {
        if(q[2]*q[3]<=0)
            {
              	drawNextLineSegment(end, CLR_BG);
 				return;
			}
		else{  
			clipStart.x = gDrawData.clipMin.x ;
			clipStart.y = start.y;
			clipEnd.x = gDrawData.clipMax.x;
			clipEnd.y = end.y;

			drawNextLineSegment(clipStart, CLR_BG);
			drawNextLineSegment(clipEnd, CLR_LINE);
			drawNextLineSegment(end, CLR_BG);
			performCorrectionAtClipPts(gDrawData.hdcMem, clipStart,
			                         CLR_LINE, CLR_BG);
			performCorrectionAtClipPts(gDrawData.hdcMem, clipEnd,
			                         CLR_LINE, CLR_BG);
			return;
			}
		}
	if(p[0]!=0 && p[2]!=0)
	{
	    for (i=0;i<4;i++)
	    {
	            if(p[i]<0 && flag)
	            {
	                    temp=(double)q[i]/(double)p[i];
	                    if(temp>u2)
	                            flag=0;
	                    else
	                    if(temp>u1)
	                            u1=temp;
	            }
	            else
	            if(p[i]>0  && flag)
	            {
	                    temp=(double)q[i]/(double)p[i];
	                    if(temp<u1)
	                            flag=0;
	                    else
	                    if(temp<u2)
	                            u2=temp;
	            }
	    }
	}
    if (u1>=u2 || flag==0)
    {
      	drawNextLineSegment(end, CLR_BG); //clip entire line
		return;
	}

    temp=start.x;
    i=start.y;    
	clipStart.x = (long) (temp + u1*dx);
	clipStart.y = (long) (i+u1*dy);
	clipEnd.x = (long) (temp+u2*dx);
	clipEnd.y = (long) (i+u2*dy);
	
	drawNextLineSegment(clipStart, CLR_BG);
	drawNextLineSegment(clipEnd, CLR_LINE);
	drawNextLineSegment(end, CLR_BG);
	performCorrectionAtClipPts(gDrawData.hdcMem, clipStart,
	                         CLR_LINE, CLR_BG);
	performCorrectionAtClipPts(gDrawData.hdcMem, clipEnd,
	                         CLR_LINE, CLR_BG); 
    
}

void clip(HWND hwnd)
{
  scLineClip(gDrawData.lineEndPts[0], gDrawData.lineEndPts[1]);
  reDraw(hwnd);
  setDrawMode(CLIPPED_MODE, hwnd);
}

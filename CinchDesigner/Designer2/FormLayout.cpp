
#include "stdafx.h"
#include <stdio.h>


FormLayout::FormLayout()
{
	i = 0;
}

void FormLayout::addField(FormField field)
{
	fields[i] = field;
	i++;
}

int FormLayout::minHeight()
{
	return minimumHeight;
}

int FormLayout::minWidth()
{
	return 100;
}

void FormLayout::show(HWND parent, HINSTANCE hInst, RECT displayArea)
{	
	int y = LAYOUT_MARGIN;

	RECT window = displayArea;
	
	int maxWidth = 0;
	int accumulatedHeight = LAYOUT_MARGIN;

	int j;
	for(j=0; j<i; j++){
		FormField field = this->fields[j];
		
		RECT control;
		GetWindowRect(field.getControl(), &control);

		RECT label;
		GetWindowRect(field.getLabel(), &label);

		int width = (control.right - control.left) + (label.right - label.left) + MARGIN;
		int height = (control.bottom - control.top) + ROW_MARGIN;
		accumulatedHeight += height;

		if ( width > maxWidth ){
			maxWidth = width;
		}
	}

	int x = LAYOUT_MARGIN;

	int twoColumnTotalWidth = maxWidth + maxWidth + LAYOUT_MARGIN + LAYOUT_MARGIN + MARGIN;
	int finalHeight = 0; 

	bool allowSecondColumn = false;

	if ( accumulatedHeight > (window.bottom - window.top) && twoColumnTotalWidth < displayArea.right - displayArea.left  ){
		//Two column centered
		x = ((window.right - window.left) - twoColumnTotalWidth ) / 2;
		allowSecondColumn = true;
		finalHeight = accumulatedHeight / 2;
	} else {
		//One column centered
		x = ((window.right - window.left) - maxWidth - LAYOUT_MARGIN) / 2;
		finalHeight = accumulatedHeight;
	}


	HWND previousControl = HWND_TOP;
	
	bool secondColumnBeingFilled = false;

	for(j=0; j<i; j++){
		FormField field = this->fields[j];
	
		RECT rect;
		GetWindowRect(field.getControl(), &rect);

		if ( secondColumnBeingFilled == false && allowSecondColumn &&  y + LAYOUT_MARGIN + (rect.bottom - rect.top) > finalHeight ){
			//Move to second column
			y = LAYOUT_MARGIN;
			x += LAYOUT_MARGIN + maxWidth + MARGIN;
			secondColumnBeingFilled = true;
		}

		
		SetWindowPos(field.getLabel(), HWND_TOP, x, y, 0, 0, SWP_NOSIZE);
		SetWindowPos(field.getControl(), previousControl, LABEL_WIDTH+MARGIN+x, y, 0, 0, SWP_NOSIZE); 

		previousControl = field.getControl();
		
		ShowWindow(field.getLabel(), SW_SHOW);
		ShowWindow(field.getControl(), SW_SHOW);

		y += rect.bottom - rect.top + ROW_MARGIN;
		
		minimumHeight = y - 1;
	}
}

void FormLayout::update(HWND parent)
{

}

int FormLayout::getFieldCount(){
	return i;
}

FormField FormLayout::getField(int index){
	return fields[index];
}